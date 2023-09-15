#include "rr_task_scheduler.h"

namespace rayclone {

RoundRobinTaskScheduler::RoundRobinTaskScheduler(int procs)
    : num_procs(procs) {}

std::future<msgpack::sbuffer>
RoundRobinTaskScheduler::ScheduleTask(TaskSpec task_spec) {
  std::promise<msgpack::sbuffer> task_promise;
  std::future<msgpack::sbuffer> task_future = task_promise.get_future();

  // TODO add task_submitter logic

  return task_future;
}
} // namespace rayclone
