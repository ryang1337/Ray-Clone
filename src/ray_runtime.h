#pragma once

#include "ray_config.h"
#include <msgpack.hpp>
#include <src/task/task_receiver.h>
#include <src/task/task_scheduler.h>
#include <src/task/task_spec.h>

#include <memory>

namespace rayclone {

class RayRuntime {
public:
  static RayRuntime &Instance() {
    static RayRuntime ray_runtime;
    return ray_runtime;
  }

  void Init();
  void Init(RayConfig config);

  std::future<msgpack::sbuffer> Call(TaskSpec task_spec);

private:
  RayRuntime() = default;
  ~RayRuntime() = default;
  RayRuntime(RayRuntime const &) = delete;
  RayRuntime(RayRuntime &&) = delete;
  RayRuntime &operator=(RayRuntime const &) = delete;
  RayRuntime &operator=(RayRuntime &&) = delete;

  std::unique_ptr<TaskScheduler> task_scheduler;
  std::unique_ptr<TaskReceiver> task_receiver;
};
} // namespace rayclone
