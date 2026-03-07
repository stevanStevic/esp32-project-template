#ifndef TESTS_HOST_UTILS_INCLUDE_X86_QUEUE_HPP_
#define TESTS_HOST_UTILS_INCLUDE_X86_QUEUE_HPP_

#include "component_interfaces/queue.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

// ─── X86Queue ─────────────────────────────────────────────────────────────────
//
// Host-test replacement for RtosQueue<T>. Implements QueueIf<T> using
// std::queue + std::mutex + std::condition_variable instead of FreeRTOS.
//
// Allows all producer-consumer patterns to be tested on host without any
// FreeRTOS dependency.
// ──────────────────────────────────────────────────────────────────────────────

template<typename T>
class X86Queue : public QueueIf<T>
{
public:
    explicit X86Queue(size_t /* maxSize */) { }

    X86Queue(const X86Queue &) = delete;
    X86Queue &operator=(const X86Queue &) = delete;
    X86Queue(X86Queue &&) = delete;
    X86Queue &operator=(X86Queue &&) = delete;
    ~X86Queue() override = default;

    bool send(T item, uint32_t timeoutMs) override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(std::move(item));
        m_cv.notify_one();
        return true;
    }

    bool receive(T &item, uint32_t timeoutMs) override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        bool ready = m_cv.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this]
                                   { return !m_queue.empty(); });
        if (!ready)
        {
            return false;
        }
        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

private:
    std::queue<T>           m_queue;
    mutable std::mutex      m_mutex;
    std::condition_variable m_cv;
};

#endif // TESTS_HOST_UTILS_INCLUDE_X86_QUEUE_HPP_
