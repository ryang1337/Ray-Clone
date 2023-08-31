#include <msgpack.hpp>
#include <vector>

namespace rayclone {

using ArgsBuffer = msgpack::sbuffer;
using ArgsBufferList = std::vector<ArgsBuffer>;

using RemoteFunction = std::function<msgpack::sbuffer(ArgsBufferList &)>;

} // namespace rayclone
