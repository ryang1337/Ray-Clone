#pragma once

#include <future>
#include <msgpack.hpp>

#include <src/common_types.h>

#include "task_spec.h"
#include "task_submitter.h"

namespace rayclone {

class TaskScheduler {
public:
  TaskScheduler(std::unique_ptr<TaskSubmitter> client)
      : client_(std::move(client)){};

  virtual ~TaskScheduler() = default;

  virtual std::future<msgpack::sbuffer> ScheduleTask(TaskSpec task_spec) = 0;

protected:
  std::unique_ptr<TaskSubmitter> client_;
};
} // namespace rayclone
