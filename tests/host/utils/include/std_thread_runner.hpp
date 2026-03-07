#ifndef TESTS_HOST_UTILS_INCLUDE_STD_THREAD_RUNNER_HPP_
#define TESTS_HOST_UTILS_INCLUDE_STD_THREAD_RUNNER_HPP_

#include "component_interfaces/task_runner.hpp"

#include <functional>
#include <thread>

// ─── StdThreadRunner ──────────────────────────────────────────────────────────
//
// Host-test replacement for RtosTaskRunner. Implements TaskRunner using
// std::thread instead of FreeRTOS xTaskCreate.
//
// Useful when testing components that spawn background tasks — the task runs
// as a std::thread on host, exercising the same code path without FreeRTOS.
// ──────────────────────────────────────────────────────────────────────────────

class StdThreadRunner : public TaskRunner
{
public:
    StdThreadRunner() = default;

    StdThreadRunner(const StdThreadRunner &) = delete;
    StdThreadRunner &operator=(const StdThreadRunner &) = delete;
    StdThreadRunner(StdThreadRunner &&) = delete;
    StdThreadRunner &operator=(StdThreadRunner &&) = delete;

    ~StdThreadRunner() override
    {
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void start(std::function<void()> task) override
    {
        m_thread = std::thread(std::move(task));
    }

private:
    std::thread m_thread;
};

#endif // TESTS_HOST_UTILS_INCLUDE_STD_THREAD_RUNNER_HPP_
