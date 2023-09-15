#pragma once
#include <src/common_types.h>
#include <src/serializer.h>
#include <string>

namespace rayclone {

class TaskSpec {
public:
  template <typename... Args>
  TaskSpec(std::string f, Args... args) : func_name(f) {
    (args_list.push_back(Serializer::Serialize(args)), ...);
  };

  std::string GetFuncName() const { return func_name; }
  ArgsBufferList GetArgs() const { return args_list; }

private:
  std::string func_name;
  ArgsBufferList args_list;
};
} // namespace rayclone
