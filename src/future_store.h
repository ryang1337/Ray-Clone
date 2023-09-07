#pragma once

namespace rayclone {
class FutureStore {
public:
  static FutureStore &Instance() {
    static FutureStore fs;
    return fs;
  }

  template <typename ReturnType> void AddFuture(ObjectRef<ReturnType> ob_ref) {}

private:
};
} // namespace rayclone
