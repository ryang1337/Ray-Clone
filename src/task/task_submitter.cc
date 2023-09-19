#include "task_submitter.h"

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

  RoundRobin round_robin_grpc_contexts(grpc_contexts.begin(), thread_count);

  // TODO send request to task_receiver

  for (auto &grpc_context : grpc_contexts) {
    grpc_context->guard.reset();
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

void TaskSubmitter::SubmitTask(TaskSpec task_sepc,
                               std::promise<msgpack::sbuffer> task_promise,
                               int proc_num) {}
} // namespace rayclone
