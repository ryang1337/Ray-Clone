#pragma once

#include "future_store.h"
#include "ray_config.h"
#include "task/task.h"

#include <msgpack.hpp>

namespace rayclone {

void Init() { RayRuntime::Instance().Init(); }

void Init(RayConfig &config) { RayRuntime::Instance().Init(config); }

template <typename Func> TaskCaller<Func> Task(Func f) {
  TaskCaller<Func> tc(f);
  return tc;
}

template <typename ReturnType>
ReturnType Get(const rayclone::ObjectRef<ReturnType> &ob_ref) {
  // blocking call to get result of future
  msgpack::sbuffer buf = FutureStore::Instance().GetFuture(ob_ref).get();
  ReturnType res;
  msgpack::unpack(buf.data(), buf.size()).get().convert(res);
  return res;
}
} // namespace rayclone
