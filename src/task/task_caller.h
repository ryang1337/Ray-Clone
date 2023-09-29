#pragma once

#include <boost/callable_traits.hpp>
#include <memory>

#include <src/common_types.h>
#include <src/future_store.h>
#include <src/object_ref.h>
#include <src/ray_runtime.h>
#include <src/register_function.h>
#include <src/serializer.h>

#include "task_spec.h"

namespace rayclone {

template <typename Func> class TaskCaller {
public:
  TaskCaller(Func f) {
    func_name = FunctionManager::Instance().FuncPtrToRemoteFunction(f);
  }

  template <typename... Args>
  ObjectRef<boost::callable_traits::return_type_t<Func>>
  Remote(Args &&...args) {
    ObjectRef<boost::callable_traits::return_type_t<Func>> ob_ref;
    TaskSpec task_spec(func_name, args...);

    // std::future<msgpack::sbuffer> fut =
    // task_scheduler->ScheduleTask(task_spec);
    std::future<msgpack::sbuffer> fut = RayRuntime::Instance().Call(task_spec);

    FutureStore::Instance().AddFuture(ob_ref, std::move(fut));

    return ob_ref;
  }

private:
  std::string func_name;
};
} // namespace rayclone
