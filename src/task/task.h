#pragma once

#include "task_caller.h"

namespace rayclone {
class Task {
public:
  template <typename Func> Task(Func f) {
    TaskCaller<Func> tc(f);
    return tc;
  }
};
} // namespace rayclone
