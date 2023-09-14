#pragma once

#include <boost/uuid/uuid.hpp>
#include <unordered_map>
#include <future>
#include <msgpack.hpp>

#include "object_ref.h"

namespace rayclone {
class FutureStore {
public:
  static FutureStore &Instance() {
    static FutureStore fs;
    return fs;
  }

  template <typename ReturnType> void AddFuture(ObjectRef<ReturnType> ob_ref) {}

  std::future<msgpack::sbuffer> GetFuture(boost::uuids::uuid id){
    return std::move(future_map.at(id));
  }

private:
  std::unordered_map<boost::uuids::uuid, std::future<msgpack::sbuffer>, boost::hash<boost::uuids::uuid>> future_map;
};
} // namespace rayclone
