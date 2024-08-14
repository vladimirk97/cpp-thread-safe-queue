#include <gtest/gtest.h>
#include "thread_safe_queue.hpp"
#include <thread>

TEST(ThreadSafeQueue, EmptyTest) 
{
    // Create queue
    TSQ::ThreadSafeQueue<int> tsq{};
    // Queue should be empty
    EXPECT_EQ(tsq.empty(), true);
    // Add element to queue
    int send_val = 1;
    tsq.push(send_val);
    EXPECT_EQ(tsq.empty(), false);
}

TEST(ThreadSafeQueue, SizeTest) 
{
    TSQ::ThreadSafeQueue<int> tsq{};
    // Queue should be empty, size 0
    EXPECT_EQ(tsq.size(), 0);
    // Add element to queue
    for(int i = 0; i < 3; i++)
        tsq.push(i);

    EXPECT_EQ(tsq.size(), 3);
}

TEST(ThreadSafeQueue, TryPopEmpty) 
{
    // The Blocking call  
    TSQ::ThreadSafeQueue<int> tsq{};
    int return_val = 0;
    EXPECT_EQ(tsq.try_pop(return_val), false);
    EXPECT_EQ(return_val, 0);
}

TEST(ThreadSafeQueue, TryPopHasElement) 
{
    TSQ::ThreadSafeQueue<int> tsq{};
    int return_val = 0;
    int send_val = 1;
    tsq.push(send_val);
    EXPECT_EQ(tsq.try_pop(return_val), true);
    EXPECT_EQ(return_val, 1);
}

TEST(ThreadSafeQueue, TryPopMultiple) 
{
    TSQ::ThreadSafeQueue<int> tsq{};
    int return_val = 0;

    for(int i = 1; i < 3; i++)
        tsq.push(i);
    
    for(int i = 1; i < 3; i++)
    {
        EXPECT_EQ(tsq.try_pop(return_val), true);
        EXPECT_EQ(return_val, i);
    }
}

TEST(ThreadSafeQueue, PopTimeoutEmpty) 
{
    TSQ::ThreadSafeQueue<int> tsq{};
    int return_val = 0;
    auto start_time = std::chrono::system_clock::now().time_since_epoch();
    EXPECT_EQ(tsq.pop(return_val, std::chrono::seconds(1)), false);
    EXPECT_EQ(return_val, 0);
    auto end_time = std::chrono::system_clock::now().time_since_epoch();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    // Durations >= timeout (queue was not read)
    EXPECT_LE(1000, duration);
}

TEST(ThreadSafeQueue, PopTimeoutHasElement) 
{
    TSQ::ThreadSafeQueue<int> tsq{};
    int return_val = 0;
    int send_val = 1;
    tsq.push(send_val);

    auto start_time = std::chrono::system_clock::now().time_since_epoch();
    EXPECT_EQ(tsq.pop(return_val, std::chrono::seconds(1)), true);
    EXPECT_EQ(return_val, 1);
    auto end_time = std::chrono::system_clock::now().time_since_epoch();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    // Durations < timeout
    EXPECT_LE(duration, 1000);
}

TEST(ThreadSafeQueue, PopBlockingEmptyShutdown) 
{
    auto tsq = std::make_shared<TSQ::ThreadSafeQueue<int>>();
    auto value = std::make_shared<int>(0);
    auto status = std::make_shared<bool>(false);

    std::thread t1([this](std::shared_ptr<TSQ::ThreadSafeQueue<int>> queue, std::shared_ptr<int> val, std::shared_ptr<bool> stat){ 
        *stat = queue->pop(*val);
    }, tsq, value, status);
    
    // Wait for thread t1 to block
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    tsq->shutdown(true);
    // Wait for thread t1 to finish
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(false, *status);
    EXPECT_EQ(0, *value);
    t1.join();
}

TEST(ThreadSafeQueue, PopBlockingHasElements) 
{
    TSQ::ThreadSafeQueue<int> tsq{};
    int return_val = 0;
    int send_val = 1;
    tsq.push(send_val);
    EXPECT_EQ(tsq.pop(return_val), true);
    EXPECT_EQ(return_val, 1);
}

TEST(ThreadSafeQueue, PopBlockingElementAddedAfterBlocking) 
{
    auto tsq = std::make_shared<TSQ::ThreadSafeQueue<int>>();
    auto value = std::make_shared<int>(0);
    auto status = std::make_shared<bool>(false);
    
    std::thread t1([this](std::shared_ptr<TSQ::ThreadSafeQueue<int>> queue, std::shared_ptr<int> val, std::shared_ptr<bool> stat){ 
        *stat = queue->pop(*val);
    }, tsq, value, status);

    // Wait for thread t1 to block
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int send_val = 1;
    tsq->push(send_val);
    // Wait to value to pop
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(true, *status);
    EXPECT_EQ(1, *value);
    t1.join();
}

void producer(TSQ::ThreadSafeQueue<int>& queue) 
{
    for(int i = 1; i < 3; i++)
        queue.push(i);

    queue.shutdown(true);
}

void consumer(TSQ::ThreadSafeQueue<int>& queue) 
{
    int item = 0;
    bool status = false;
    for(int i = 1; i < 3; i++)
    {
        status = queue.pop(item);
        EXPECT_EQ(i, item);
        EXPECT_EQ(true, status);
    }

    EXPECT_EQ(false, queue.pop(item));
}

TEST(ThreadSafeQueue, PopBlockingElementAddedAndShutdownBeforePop) 
{
    TSQ::ThreadSafeQueue<int> tsq;
    std::thread t1(producer, std::ref(tsq));
    // Wait until thread t1 finish all operations
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t2(consumer, std::ref(tsq));

    t1.join();
    t2.join();

    EXPECT_EQ(true, true);
}