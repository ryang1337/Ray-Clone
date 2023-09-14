#include <gtest/gtest.h>
#include <msgpack.hpp>
#include <string>
#include <src/register_function.h>
#include <src/serializer.h>

int Add1(int x){
  return x + 1;
}

std::string AddIntReturnString(int x, int y){
  return std::to_string(x + y);
}
REGISTER_REMOTE_FUNCTION(AddIntReturnString);
REGISTER_REMOTE_FUNCTION(Add1);

TEST(RegisterFunctionTest, RemoteFunctionSizeTest){
  EXPECT_EQ(rayclone::FunctionManager::Instance().GetRemoteFunctions().size(), 2);
}

TEST(RegisterFunctionTest, FunctionNameTest){
  EXPECT_EQ(rayclone::FunctionManager::Instance().FuncPtrToFuncName(AddIntReturnString), "AddIntReturnString");
  EXPECT_EQ(rayclone::FunctionManager::Instance().FuncPtrToFuncName(Add1), "Add1");
}

TEST(RegisterFunctionTest, FunctionExecutionTest){
  rayclone::ArgsBufferList args1;
  args1.push_back(rayclone::Serializer::Serialize(3));
  auto add1Res = rayclone::FunctionManager::Instance().FuncPtrToRemoteFunction(Add1)(args1);
  EXPECT_EQ(rayclone::Serializer::Deserialize<int>(add1Res), 4);

  rayclone::ArgsBufferList args2;
  args2.push_back(rayclone::Serializer::Serialize(5));
  args2.push_back(rayclone::Serializer::Serialize(6));
  auto addIntReturnStringRes = rayclone::FunctionManager::Instance().FuncPtrToRemoteFunction(AddIntReturnString)(args2);
  EXPECT_EQ(rayclone::Serializer::Deserialize<std::string>(addIntReturnStringRes), "11");
}

TEST(ApplicatorTest, ApplyTest){
  rayclone::ArgsBufferList args2;
  args2.push_back(rayclone::Serializer::Serialize(5));
  args2.push_back(rayclone::Serializer::Serialize(6));
  auto applyRes = rayclone::Applicator<std::string(int, int)>::Apply(AddIntReturnString, args2);
  EXPECT_EQ(rayclone::Serializer::Deserialize<std::string>(applyRes), "11");
}
