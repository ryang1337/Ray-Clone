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
  template <class Iterator> class RoundRobin {
  public:
    RoundRobin(Iterator begin, std::size_t size) : begin(begin), size(size) {}

    decltype(auto) next() {
      const auto cur = current.fetch_add(1, std::memory_order_relaxed);
      const auto pos = cur % size;
      return *std::next(begin, pos);
    }

  private:
    Iterator begin;
    std::size_t size;
    std::atomic_size_t current{};
  };

  struct GuardedGrpcContext {
    agrpc::GrpcContext context;
    asio::executor_work_guard<agrpc::GrpcContext::executor_type> guard{
        context.get_executor()};
  };

  TaskSubmitter();

  virtual ~TaskSubmitter() = default;

  virtual void SubmitTask(TaskSpec task_spec,
                          std::promise<msgpack::sbuffer> task_promise,
                          int proc_num);

private:
  std::size_t thread_count;
  std::vector<std::thread> threads;
  std::vector<raycloneRPC::RemoteRPC::Stub> stubs;
  std::vector<std::unique_ptr<GuardedGrpcContext>> grpc_contexts;
  std::vector<std::string> hosts = {"localhost:50051", "localhost:50052",
                                    "localhost:50053"};
};

} // namespace rayclone
