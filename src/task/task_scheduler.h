#pragma once

#include <future>
#include <msgpack.hpp>

namespace rayclone {

class TaskScheduler{
public:
  virtual std::future<msgpack::sbuffer> Submit() = 0;

private:
};
}
