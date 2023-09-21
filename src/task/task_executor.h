#pragma once

#include <msgpack.hpp>
#include <src/common_types.h>
#include <src/register_function.h>

namespace rayclone {

class TaskExecutor {
public:
  msgpack::sbuffer Execute(std::string func_name, ArgsBufferList args) {
    // TODO load dlls in ray_runtime, then use GetFMInstance
    auto func = GetFMInstance().GetRemoteFunction(func_name);
    msgpack::sbuffer result;
    result = func(args);

    return std::move(result);
  }
};

} // namespace rayclone
