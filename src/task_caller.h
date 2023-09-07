#pragma once

#include "boost/callable_traits.hpp"
#include "common_types.h"
#include "future_store.h"
#include "register_function.h"
#include "serializer.h"

namespace rayclone {
class TaskSpec {
public:
  template <typename... Args>
  TaskSpec(std::string f, Args... args)
      : func_name(f){(args_list.push_back(Serializer::Serialize(args)))...};

private:
  std::string func_name;
  ArgsBufferList args_list;
};

template <typename Func> class TaskCaller {
public:
  TaskCaller(Func f) {
    func_name =
        rayclone::FunctionManager::Instance().FuncPtrToRemoteFunction(f);
  }

  template <typename... Args>
  ObjectRef<boost::callable_traits::return_type_t<Func>>
  Remote(Args &&...args) {
    ObjectRef ob_ref;
    rayclone::FutureStore::Instance().AddFuture(ob_ref);
  }

private:
  std::string func_name;
};
} // namespace rayclone
