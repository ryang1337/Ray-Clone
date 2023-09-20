#include "task_submitter.h"

namespace asio = boost::asio;

namespace rayclone {

TaskSubmitter::TaskSubmitter() {
  thread_count = std::thread::hardware_concurrency();

  // set up grpc contexts
  for (int i = 0; i < thread_count; i++) {
    grpc_contexts.emplace_back(std::make_unique<GuardedGrpcContext>());
  }

  // run grpc contexts on multiple threads
  threads.reserve(thread_count);
  for (int i = 0; i < thread_count; i++) {
    threads.emplace_back([&, i]() { grpc_contexts[i]->context.run(); });
  }

  // create the stubs that connect to the remote procs
  for (const auto &host : hosts) {
    stubs.emplace_back(
        grpc::CreateChannel(host, grpc::InsecureChannelCredentials()));
  }

  rr = std::move(RoundRobin(grpc_contexts.begin(), thread_count));

  for (auto &grpc_context : grpc_contexts) {
    grpc_context->guard.reset();
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

void TaskSubmitter::SubmitTask(const TaskSpec &task_spec,
                               std::promise<msgpack::sbuffer> &task_promise,
                               int proc_num) {
  auto &grpc_context = rr.next()->context;
  boost::asio::co_spawn(
      grpc_context,
      MakeRequest(grpc_context, stubs[proc_num], task_spec, task_promise),
      boost::asio::detached);
}

asio::awaitable<void> TaskSubmitter::MakeRequest(
    agrpc::GrpcContext &grpc_context, raycloneRPC::RemoteRPC::Stub &stub,
    const TaskSpec &task_spec, std::promise<msgpack::sbuffer> &task_promise) {
  using RPC =
      agrpc::ClientRPC<&raycloneRPC::RemoteRPC::Stub::PrepareAsyncRayRemote>;
  grpc::ClientContext client_context;
  client_context.set_deadline(std::chrono::system_clock::now() +
                              std::chrono::seconds(10));
  RPC::Request request;
  RPC::Response response;

  // prepare the args for the RPC
  request.set_function_name(task_spec.func_name);
  for (auto it = task_spec.args_list.begin(); it != task_spec.args_list.end();
       it++) {
    ArgsBuffer arg = std::move(*(*it));
    request.add_function_args(arg.data());
  }

  // perform the RPC!
  const auto status =
      co_await RPC::request(grpc_context, stub, client_context, request,
                            response, asio::use_awaitable);

  // HACK on network failure we just don't do anything right now
  // same thing happens if client tries to connect before the server
  // starts running
  if (!status.ok()) {
    std::cout << "RPC ERROR\n";
    co_return;
  }

  // set the value in the promise from the RPC result
  std::string res = response.response();
  msgpack::sbuffer buf;
  buf.write(res.data(), res.length());
  task_promise.set_value(std::move(buf));
}
} // namespace rayclone
