#pragma once

#include <boost/callable_traits.hpp>
#include <functional>
#include <iostream>
#include <msgpack.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "common_types.h"
#include "serializer.h"

namespace rayclone {

/*
 * Type erased wrapper around all user defined functions
 */
template <typename Function> class Applicator {
public:
  static msgpack::sbuffer Apply(Function f, const ArgsBufferList &args_list) {
    using ReturnType = boost::callable_traits::return_type_t<Function>;
    using ArgListType = boost::callable_traits::args_t<Function>;

    // unpack args
    ArgListType arg_tuple;
    DeserializeArgs(
        arg_tuple, args_list,
        std::make_index_sequence<std::tuple_size<ArgListType>::value>{});

    // call f with unpacked args
    ReturnType res = Call<ReturnType>(
        f, std::make_index_sequence<std::tuple_size<ArgListType>::value>{},
        arg_tuple);

    std::cout << res << "\n";

    // pack result
    return Serializer::Serialize<ReturnType>(res);
  }

private:
  template <typename ArgType>
  static ArgType ParseArg(const msgpack::sbuffer &arg) {
    return Serializer::Deserialize<ArgType>(arg);
  }

  template <typename ArgListType, size_t... I>
  static void DeserializeArgs(ArgListType &arg_tuple,
                              const ArgsBufferList &args_list,
                              std::index_sequence<I...>) {
    // fold expression to parse each arg in the arg_list
    ((std::get<I>(arg_tuple) =
          ParseArg<std::tuple_element_t<I, ArgListType>>(args_list.at(I))),
     ...);
  }

  template <typename ReturnType, size_t... I, typename... Args>
  static ReturnType Call(Function f, std::index_sequence<I...>,
                         std::tuple<Args...> args_tuple) {
    return f((std::get<I>(args_tuple))...);
  }
};

class FunctionManager {
public:
  static FunctionManager &Instance() {
    static FunctionManager fm;
    return fm;
  }

  template <typename Function>
  int RegisterRemoteFunction(std::string func_name, Function f) {
    std::string addr = GetAddress(f);
    function_ptr_map.emplace(addr, func_name);
    remote_function_map.emplace(func_name,
                                std::bind(Applicator<Function>::Apply,
                                          std::move(f), std::placeholders::_1));
    return 0;
  }

  std::unordered_map<std::string, RemoteFunction> GetRemoteFunctions() {
    return remote_function_map;
  }

  RemoteFunction &GetRemoteFunction(std::string func_name) {
    return remote_function_map[func_name];
  }

  template <typename Function>
  RemoteFunction &FuncPtrToRemoteFunction(Function f) {
    std::string func_name = function_ptr_map[GetAddress(f)];
    return remote_function_map[func_name];
  }

  template <typename Function>
  std::string FuncPtrToFuncName(Function f){
    return function_ptr_map[GetAddress(f)];
  }

private:
  std::unordered_map<std::string, RemoteFunction> remote_function_map;
  std::unordered_map<std::string, std::string> function_ptr_map;

  template <class Dest, class Source> Dest BitCast(const Source &source) {
    static_assert(
        sizeof(Dest) == sizeof(Source),
        "BitCast requires source and destination to be the same size");

    Dest dest;
    memcpy(&dest, &source, sizeof(dest));
    return dest;
  }

  // converting function to string
  template <typename F> std::string GetAddress(F f) {
    auto arr = BitCast<std::array<char, sizeof(F)>>(f);
    return std::string(arr.data(), arr.size());
  }
};

#define CONCAT(s1, s2) s1##s2
#define CONCAT2(s1, s2) CONCAT(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCAT2(str, __LINE__)

#define REGISTER_REMOTE_FUNCTION(func)                                         \
  inline auto ANONYMOUS_VARIABLE(var) =                                        \
      rayclone::FunctionManager::Instance().RegisterRemoteFunction(#func,      \
                                                                   func);
} // namespace rayclone
