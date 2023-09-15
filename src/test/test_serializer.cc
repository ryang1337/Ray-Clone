#include <gtest/gtest.h>
#include <src/serializer.h>

TEST(SerializerTest, SerializeIntTest) {
  msgpack::sbuffer buf = rayclone::Serializer::Serialize(5);
  int val = 5;
  msgpack::sbuffer expected;
  msgpack::pack(expected, val);
  EXPECT_EQ(*buf.data(), *expected.data());
}

TEST(SerializerTest, SerializeStringTest) {
  msgpack::sbuffer buf = rayclone::Serializer::Serialize("test");
  std::string val = "test";
  msgpack::sbuffer expected;
  msgpack::pack(expected, val);
  EXPECT_EQ(*buf.data(), *expected.data());
}

TEST(SerializerTest, DeserializeIntTest) {
  msgpack::sbuffer packed;
  msgpack::pack(packed, 5);
  int unpacked = rayclone::Serializer::Deserialize<int>(packed);
  EXPECT_EQ(unpacked, 5);
}

TEST(SerializerTest, DeserializeStringTest) {
  msgpack::sbuffer packed;
  msgpack::pack(packed, "test");
  std::string unpacked = rayclone::Serializer::Deserialize<std::string>(packed);
  EXPECT_EQ(unpacked, "test");
}
