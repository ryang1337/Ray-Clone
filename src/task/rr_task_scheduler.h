#pragma once

#include "task_scheduler.h"
#include "task_spec.h"

namespace rayclone {

class RoundRobinTaskScheduler : public TaskScheduler {
public:
  RoundRobinTaskScheduler(int procs);

  std::future<msgpack::sbuffer> ScheduleTask(TaskSpec task_spec);

private:
  int num_procs;
};
} // namespace rayclone
