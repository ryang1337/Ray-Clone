#pragma once

#include "task_executor.h"
#include <src/protobuf/remote.grpc.pb.h>

#include <agrpc/asio_grpc.hpp>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace rayclone {

class TaskReceiver {
public:
  TaskReceiver() = default;

  TaskReceiver(std::unique_ptr<TaskExecutor> exec)
      : executor(std::move(exec)) {}

  virtual ~TaskReceiver() = default;

  virtual void StartReceivingTasks(int port);

private:
  std::unique_ptr<grpc::Server> server;
  raycloneRPC::RemoteRPC::AsyncService service;
  std::vector<std::unique_ptr<agrpc::GrpcContext>> grpc_contexts;
  std::unique_ptr<TaskExecutor> executor;

  void RegisterRequestHandler(agrpc::GrpcContext &grpc_context,
                              raycloneRPC::RemoteRPC::AsyncService &service);
};

} // namespace rayclone
