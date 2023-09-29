#include "ray_runtime.h"
#include "function_helper.h"
#include <src/task/rr_task_scheduler.h>

namespace rayclone {
void RayRuntime::Init() {
  RayConfig config = {.grpcPort = "", .is_worker = false};
  Init(config);
}

void RayRuntime::Init(RayConfig config) {
  if (config.is_worker) {
    auto task_executor = std::make_unique<TaskExecutor>();
    task_receiver = std::make_unique<TaskReceiver>(std::move(task_executor));
    FunctionHelper::Instance().LoadUserFunctions();
  } else {
    auto task_submitter = std::make_unique<TaskSubmitter>();
    task_scheduler =
        std::make_unique<RoundRobinTaskScheduler>(3, std::move(task_submitter));
  }
}

std::future<msgpack::sbuffer> RayRuntime::Call(TaskSpec task_spec) {
  return std::move(task_scheduler->ScheduleTask(task_spec));
}
} // namespace rayclone
