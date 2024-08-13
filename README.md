# ThreadSafeQueue Library

## Overview

The `ThreadSafeQueue` library is a C++ template-based implementation of a thread-safe queue. This library provides an efficient and easy-to-use way to handle queues in a multithreaded environment, ensuring safe access and modification of the queue by multiple threads. It is designed to be versatile and robust, supporting blocking and non-blocking operations, with optional timeout for retrieval operations.

## Features

- **Thread Safety:** Ensures safe access and modification of the queue in multithreaded environments using mutexes and condition variables.
- **Blocking Operations:** Supports blocking `pop()` operations that will wait until an element is available.
- **Non-Blocking Operations:** Provides a non-blocking `try_pop()` operation to retrieve elements without waiting.
- **Timed Operations:** Allows for a timed `pop()` operation that waits for a specified duration to retrieve an element before timing out.
- **Graceful Shutdown:** Includes a `shutdown()` method to gracefully terminate blocking operations, ensuring clean thread exits.

## How It Works

The `ThreadSafeQueue` class manages a standard queue (`std::queue`) and uses a mutex (`std::mutex`) to protect shared access to the queue. The `std::condition_variable` is used to handle blocking operations, allowing threads to wait until an item is available in the queue. The library also supports shutdown functionality to allow threads to exit gracefully from blocking operations.

### Example Usage

```cpp
#include "ThreadSafeQueue.h"
#include <iostream>
#include <thread>
#include <string>

ThreadSafeQueue<std::string> queue;

void producer() {
    queue.push("Hello");
    queue.push("World");
    queue.shutdown(true); // Signal that we are done producing
}

void consumer() {
    std::string item;
    while (queue.pop(item)) {
        std::cout << item << std::endl;
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    return 0;
}
