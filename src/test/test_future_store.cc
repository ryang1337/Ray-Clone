#include <chrono>
#include <gtest/gtest.h>

#include <src/future_store.h>
#include <src/serializer.h>

TEST(FutureStoreTest, AddFutureTest) {
  rayclone::ObjectRef<int> ob_ref;
  std::future<msgpack::sbuffer> fut;
  bool status =
      rayclone::FutureStore::Instance().AddFuture(ob_ref, std::move(fut));
  EXPECT_EQ(status, true);
}

TEST(FutureStoreTest, SetPromiseValueTest) {
  // add future to store
  rayclone::ObjectRef<int> ob_ref;
  std::promise<msgpack::sbuffer> promise;
  std::future<msgpack::sbuffer> fut_before = promise.get_future();
  bool status = rayclone::FutureStore::Instance().AddFuture(
      ob_ref, std::move(fut_before));
  EXPECT_EQ(status, true);

  // set promise value to a msgpack::sbuffer
  msgpack::sbuffer buf;
  msgpack::pack(buf, 5);
  promise.set_value(std::move(buf));

  // check the future store has the right value
  std::future<msgpack::sbuffer> fut_after =
      rayclone::FutureStore::Instance().GetFuture(ob_ref);
  std::future_status fut_status =
      fut_after.wait_for(std::chrono::milliseconds(1000));

  ASSERT_EQ(fut_status, std::future_status::ready);

  msgpack::sbuffer buf2 = fut_after.get();
  EXPECT_EQ(rayclone::Serializer::Deserialize<int>(buf2), 5);
}
