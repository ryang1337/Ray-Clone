#pragma once

#include <memory>

#include "task_scheduler.h"
#include "task_spec.h"

namespace rayclone {

class RoundRobinTaskScheduler : public TaskScheduler {
public:
  RoundRobinTaskScheduler(std::size_t procs,
                          std::unique_ptr<TaskSubmitter> client);

  std::future<msgpack::sbuffer> ScheduleTask(TaskSpec task_spec);

private:
  std::size_t next();

  std::size_t num_procs;
  std::size_t current;
};
} // namespace rayclone
