#include "task_receiver.h"
#include <src/common_types.h>

namespace rayclone {

void TaskReceiver::StartReceivingTasks(int port) {
  std::string host = std::string("0.0.0.0:") + std::to_string(port);
  int thread_count = std::thread::hardware_concurrency();

  grpc::ServerBuilder builder;
  for (std::size_t i = 0; i < thread_count; i++) {
    grpc_contexts.emplace_back(
        std::make_unique<agrpc::GrpcContext>(builder.AddCompletionQueue()));
  }
  builder.AddListeningPort(host, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  server = builder.BuildAndStart();

  std::vector<std::thread> threads;
  for (size_t i = 0; i < thread_count; i++) {
    threads.emplace_back([&, i]() {
      auto &grpc_context = *grpc_contexts[i];
      RegisterRequestHandler(grpc_context, service);
      grpc_context.run();
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

void TaskReceiver::RegisterRequestHandler(
    agrpc::GrpcContext &grpc_context,
    raycloneRPC::RemoteRPC::AsyncService &service) {
  agrpc::repeatedly_request(
      &raycloneRPC::RemoteRPC::AsyncService::RequestRayRemote, service,
      asio::bind_executor(
          grpc_context,
          [&](grpc::ServerContext &, raycloneRPC::RemoteRequest &request,
              grpc::ServerAsyncResponseWriter<raycloneRPC::RemoteResponse>
                  &writer) -> asio::awaitable<void> {
            raycloneRPC::RemoteResponse response;

            ArgsBufferList args;
            for (const auto &arg : request.function_args()) {
              msgpack::sbuffer buf;
              buf.write(arg.data(), arg.length());
              args.emplace_back(std::move(buf));
            }

            response.set_response(
                executor->Execute(request.function_name(), args).data());

            co_await agrpc::finish(writer, response, grpc::Status::OK);
          }));
}

} // namespace rayclone
