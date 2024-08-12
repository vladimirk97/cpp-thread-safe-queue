#include <gtest/gtest.h>
#include "thread_safe_queue.hpp"

TEST(ThreadSafeQueue, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.

    ThreadSafeQueue<int>* tsq = new ThreadSafeQueue<int>();  

    tsq->push(10);
    int value = 0;
    tsq->pop(value);

    EXPECT_EQ(10, value);

    delete tsq;
}


TEST(ExampleTests, BasicAssertionsFail) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
}