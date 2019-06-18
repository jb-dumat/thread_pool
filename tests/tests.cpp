#include "gtest/gtest.h"
#include "ThreadPool.hpp"

TEST(testPool, myPoolTest)
{
    async::threadpool::LocalInstance pool(std::thread::hardware_concurrency());

    EXPECT_EQ(pool.size(), std::thread::hardware_concurrency());
}