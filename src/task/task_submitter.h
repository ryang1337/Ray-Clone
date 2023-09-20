#pragma once

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <msgpack.hpp>

#include "task_spec.h"
#include <src/protobuf/remote.grpc.pb.h>

#include <future>
#include <memory>
#include <thread>
#include <vector>

namespace asio = boost::asio;

namespace rayclone {

class TaskSubmitter {
public:
  TaskSubmitter();

  virtual ~TaskSubmitter() = default;

  virtual void SubmitTask(const TaskSpec &task_spec,
                          std::promise<msgpack::sbuffer> &task_promise,
                          int proc_num);

private:
  struct GuardedGrpcContext {
    agrpc::GrpcContext context;
    asio::executor_work_guard<agrpc::GrpcContext::executor_type> guard{
        context.get_executor()};
  };

  class RoundRobin {
  public:
    RoundRobin() = default;

    RoundRobin(std::vector<std::unique_ptr<GuardedGrpcContext>>::iterator begin,
               std::size_t size)
        : begin(begin), size(size) {}

    decltype(auto) next() {
      std::size_t cur = current + 1;
      current = cur % size;
      return *std::next(begin, current);
    }

  private:
    std::vector<std::unique_ptr<GuardedGrpcContext>>::iterator begin;
    std::size_t size;
    std::size_t current;
  };

  asio::awaitable<void>
  MakeRequest(agrpc::GrpcContext &grpc_context,
              raycloneRPC::RemoteRPC::Stub &stub, const TaskSpec &task_spec,
              std::promise<msgpack::sbuffer> &task_promise);

  std::size_t thread_count;
  std::vector<std::thread> threads;
  std::vector<raycloneRPC::RemoteRPC::Stub> stubs;
  std::vector<std::unique_ptr<GuardedGrpcContext>> grpc_contexts;
  RoundRobin rr;
  std::vector<std::string> hosts = {"localhost:50051", "localhost:50052",
                                    "localhost:50053"};
};

} // namespace rayclone
