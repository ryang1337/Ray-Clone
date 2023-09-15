#pragma once

#include <future>
#include <msgpack.hpp>

#include <src/common_types.h>

#include "task_spec.h"

namespace rayclone {

class TaskScheduler {
public:
  TaskScheduler(){};

  virtual ~TaskScheduler(){};

  virtual std::future<msgpack::sbuffer> ScheduleTask(TaskSpec task_spec) = 0;
};
} // namespace rayclone
