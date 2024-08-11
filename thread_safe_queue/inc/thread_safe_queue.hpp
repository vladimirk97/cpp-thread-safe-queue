#ifndef THREAD_SAFE_QUEUE_H_
#define THREAD_SAFE_QUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#define DEFAULT_MAX_QUEUE_SIZE  10

template<typename QueueType>
class ThreadSafeQueue 
{
  private:
    std::queue<QueueType> m_queue;
    std::mutex m_lock;
    std::condition_variable m_cv;
    std::atomic<bool> m_shutting_down;
    const std::size_t m_max_queue_size;

  public:
    ThreadSafeQueue() = delete;
    ThreadSafeQueue(std::size_t max_queue_size = DEFAULT_MAX_QUEUE_SIZE);
    ~ThreadSafeQueue() {};

    QueueType& front(void) const;  
    
    QueueType& back(void) const;
    
    bool empty(void) const;
    
    std::size_t size(void) const;
    
    void push(const QueueType& value);
    
    QueueType pop(void);  

    void pop(QueueType& value);  

    void shutdown(void) const;
};

template<typename QueueType>
ThreadSafeQueue<QueueType>::ThreadSafeQueue(std::size_t max_queue_size)
: m_max_queue_size(max_queue_size), m_shutting_down(false) 
{
}

template<typename QueueType>
QueueType& ThreadSafeQueue<QueueType>::front(void) const 
{
    std::lock_guard<std::mutex> ulock(m_lock);
    return m_queue.front();
}

template<typename QueueType>
QueueType& ThreadSafeQueue<QueueType>::back(void) const 
{
    std::lock_guard<std::mutex> ulock(m_lock);
    return m_queue.back();
}

template<typename QueueType>
bool ThreadSafeQueue<QueueType>::empty(void) const 
{
    std::lock_guard<std::mutex> ulock(m_lock);
    return m_queue.empty();
}

template<typename QueueType>
std::size_t ThreadSafeQueue<QueueType>::size(void) const 
{
    std::lock_guard<std::mutex> ulock(m_lock);
    return m_queue.size();
}

template<typename QueueType>
void ThreadSafeQueue<QueueType>::push(const QueueType& value) 
{
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_queue.push(value);
    }
    m_cv.notify_one();
}

template<typename QueueType>
QueueType ThreadSafeQueue<QueueType>::pop(void) 
{
    std::lock_guard<std::mutex> ulock(m_lock);
    m_cv.wait(ulock, [this](){ return (!m_queue.empty() || m_shutting_down.load()); });

    QueueType value = m_queue.front();
    m_queue.pop();
    return value;
}

template<typename QueueType>
void ThreadSafeQueue<QueueType>::pop(QueueType& value) 
{
    std::lock_guard<std::mutex> ulock(m_lock);
    m_cv.wait(ulock, [this](){ return (!m_queue.empty() || m_shutting_down.load()); });

    value = std::move(m_queue.front());
    m_queue.pop();
}

template<typename QueueType>
void ThreadSafeQueue<QueueType>::shutdown(void) const 
{
    m_shutting_down.store(true);
}

#endif /* THREAD_SAFE_QUEUE_H_ */