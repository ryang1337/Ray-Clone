#pragma once

#include <boost/uuid/uuid.hpp>
#include <future>
#include <msgpack.hpp>
#include <unordered_map>
#include <utility>

#include "object_ref.h"

namespace rayclone {
class FutureStore {
public:
  static FutureStore &Instance() {
    static FutureStore fs;
    return fs;
  }

  template <typename ReturnType>
  bool AddFuture(const ObjectRef<ReturnType> &ob_ref,
                 std::future<msgpack::sbuffer> &&fut) {
    return future_map
        .emplace(ob_ref.GetTag(),
                 std::forward<std::future<msgpack::sbuffer>>(fut))
        .second;
  }

  template <typename ReturnType>
  std::future<msgpack::sbuffer> GetFuture(const ObjectRef<ReturnType> &ob_ref) {
    return std::move(future_map.at(ob_ref.GetTag()));
  }

private:
  std::unordered_map<boost::uuids::uuid, std::future<msgpack::sbuffer>,
                     boost::hash<boost::uuids::uuid>>
      future_map;
};
} // namespace rayclone
