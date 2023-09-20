#include "rr_task_scheduler.h"

namespace rayclone {

RoundRobinTaskScheduler::RoundRobinTaskScheduler(
    std::size_t procs, std::unique_ptr<TaskSubmitter> client)
    : TaskScheduler(std::move(client)), num_procs(procs), current(0) {}

std::future<msgpack::sbuffer>
RoundRobinTaskScheduler::ScheduleTask(TaskSpec task_spec) {
  std::promise<msgpack::sbuffer> task_promise;
  std::future<msgpack::sbuffer> task_future = task_promise.get_future();

  int proc_num = next();

  client_->SubmitTask(task_spec, task_promise, proc_num);

  return task_future;
}

std::size_t RoundRobinTaskScheduler::next() {
  std::size_t new_curr = current + 1;
  current = new_curr % num_procs;
  return current;
}
} // namespace rayclone
