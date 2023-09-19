#pragma once

#include <src/common_types.h>
#include <src/serializer.h>

#include <memory>
#include <string>
#include <vector>

namespace rayclone {

class TaskSpec {
public:
  template <typename... Args>
  TaskSpec(std::string f, Args... args) : func_name(f) {
    (args_list.emplace_back(
         std::make_unique<ArgsBuffer>(Serializer::Serialize(args))),
     ...);
  };

  std::string func_name;
  std::vector<std::unique_ptr<ArgsBuffer>> args_list;
};
} // namespace rayclone
