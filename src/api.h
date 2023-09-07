#pragma once

namespace rayclone {

void Init() {}

template <typename Func> Task(Func f) {
  rayclone::TaskCaller<Func> tc(f);
  return tc;
}

template <typename ReturnType>
Get(const rayclone::ObjectRef<ReturnType> &ob_ref) {}
} // namespace rayclone
