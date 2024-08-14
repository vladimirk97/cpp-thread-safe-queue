#ifndef THREAD_SAFE_QUEUE_H_
#define THREAD_SAFE_QUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

namespace TSQ
{
template<typename QueueType>
class ThreadSafeQueue 
{
  private:
    std::queue<QueueType> m_queue;
    std::mutex m_lock;
    std::condition_variable m_cv;
    std::atomic<bool> m_shutting_down;

  public:
    ThreadSafeQueue(): m_shutting_down(false) {};
    ~ThreadSafeQueue() {};
 
    /** 
        \brief Check if the queue is empty.
        \returns Returns true if queue is empty.
    **/
    bool empty(void);
    
    /** 
        \brief Get the size of the queue.
        \returns Returns the number of elements in the queue.
    **/
    std::size_t size(void);
    
    /** 
        \brief Push to the back of the queue. NOTE: The value pushed is moved (std::move) 
        onto the queue.
        \param[in] value Value reference to push to the queue.
    **/
    void push(QueueType& value);

    /** 
        \brief Blocking call to retrieve the front element of the queue. If it is used in the main
        thread, it must have an element present in the queue or it will block indefinately. In order
        for it to function as a blocking queue, it should be called from a separate thread. To exit
        the function, call the shutdown() command.
        \param[in] value Reference to the retrieved value.
        \returns true if element is successfully retrieved, false shutdown() was called.
    **/
    bool pop(QueueType& value);  
    
    /** 
        \brief Blocking call to retrieve the front element of the queue with a timeout.
        \param[in] value Reference to the retrieved value
        \param[in] time Duration to wait for retrieving the front element.
        \returns true if element is successfully retrieved, false if a timeout has occured.
    **/
    template<typename _Rep, typename _Period>
    bool pop(QueueType& value, const std::chrono::duration<_Rep, _Period>& time);

    /** 
        \brief Non-Blocking call to retrieve the front element of the queue.
        \param[in] value Reference to the retrieved value
        \returns true if element is successfully retrieved, false if the queue is empty.
    **/
    bool try_pop(QueueType& value);

    /** 
        \brief Initialize shutdown of the queue. Need to call this function to exit the
        blocking pop(&) function when deconstructing. Otherwise, it will block indefinately.
    **/
    void shutdown(const bool state);
};

template<typename QueueType>
bool ThreadSafeQueue<QueueType>::empty(void) 
{
    std::lock_guard<std::mutex> ulock(m_lock);
    return m_queue.empty();
}

template<typename QueueType>
std::size_t ThreadSafeQueue<QueueType>::size(void) 
{
    std::lock_guard<std::mutex> ulock(m_lock);
    return m_queue.size();
}

template<typename QueueType>
void ThreadSafeQueue<QueueType>::push(QueueType& value) 
{
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_queue.push(std::move(value));
    }
    m_cv.notify_one();
}

template<typename QueueType>
bool ThreadSafeQueue<QueueType>::pop(QueueType& value) 
{
    std::unique_lock<std::mutex> ulock(m_lock);
    m_cv.wait(ulock, [this](){ return (!m_queue.empty() || m_shutting_down.load()); });
    
    if(m_shutting_down.load() && m_queue.empty())
    {
        return false;
    }

    value = std::move(m_queue.front());
    m_queue.pop();
    return true;
}
    
template<typename QueueType>
template<typename _Rep, typename _Period>
bool ThreadSafeQueue<QueueType>::pop(QueueType& value, const std::chrono::duration<_Rep, _Period>& time)
{
    std::unique_lock<std::mutex> ulock(m_lock);
    if(!m_cv.wait_for(ulock, time, [this](){ return (!m_queue.empty() || m_shutting_down.load()); }))
    {
        // Timeout expired
        return false;
    }

    value = std::move(m_queue.front());
    m_queue.pop();
    return true;
}

template<typename QueueType>
bool ThreadSafeQueue<QueueType>::try_pop(QueueType& value)
{
    std::unique_lock<std::mutex> ulock(m_lock);
    if(m_queue.empty())
    {
        return false;
    }

    value = std::move(m_queue.front());
    m_queue.pop();
    return true;
}

template<typename QueueType>
void ThreadSafeQueue<QueueType>::shutdown(const bool state) 
{
    m_shutting_down.store(state);
    m_cv.notify_all();
}

} // namespace TSQ
#endif /* THREAD_SAFE_QUEUE_H_ */