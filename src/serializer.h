#pragma once

#include "msgpack.hpp"

namespace rayclone {
class Serializer {
public:
  template <typename T> static msgpack::sbuffer Serialize(const T &arg) {
    msgpack::sbuffer buf;
    msgpack::pack(buf, arg);
    return buf;
  }

  template <typename T> static T Deserialize(const msgpack::sbuffer &buf) {
    T res;
    msgpack::unpack(buf.data(), buf.size()).get().convert(res);
    return res;
  }
};
} // namespace rayclone
