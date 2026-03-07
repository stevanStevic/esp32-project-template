#ifndef TESTS_HOST_UTILS_INCLUDE_COMPONENT_INTERFACES_QUEUE_HPP_
#define TESTS_HOST_UTILS_INCLUDE_COMPONENT_INTERFACES_QUEUE_HPP_

#include <cstdint>

// ─── QueueIf<T> ───────────────────────────────────────────────────────────────
//
// Abstract interface for a thread-safe FIFO queue. Used for inter-task
// communication. Implementations:
//   - RtosQueue<T>   (production: FreeRTOS xQueueCreate/Send/Receive)
//   - X86Queue<T>    (host tests: std::queue + std::mutex)
// ──────────────────────────────────────────────────────────────────────────────

template<typename T>
class QueueIf
{
public:
    virtual ~QueueIf() = default;

    /// @brief Send an item to the queue.
    /// @param item     Item to send (by value, moved in).
    /// @param timeoutMs Maximum time to wait if queue is full.
    /// @return true if sent successfully within timeoutMs.
    virtual bool send(T item, uint32_t timeoutMs) = 0;

    /// @brief Receive an item from the queue.
    /// @param item     Output: populated with received item on success.
    /// @param timeoutMs Maximum time to wait if queue is empty.
    /// @return true if an item was received within timeoutMs.
    virtual bool receive(T &item, uint32_t timeoutMs) = 0;
};

#endif // TESTS_HOST_UTILS_INCLUDE_COMPONENT_INTERFACES_QUEUE_HPP_
