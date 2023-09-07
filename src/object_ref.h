#pragma once

#include <boost::uuid/uuid.hpp>
#include <boost::uuid/uuid_generators.hpp>

namespace rayclone {
template <typename ReturnType> class ObjectRef {
public:
  ObjectRef() : tag(boost::uuids::random_generator()) {}

  bool operator==(ObjectRef const &rhs) const { return tag == rhs.tag; }

private:
  boost::uuids::uuid tag;
};
} // namespace rayclone
