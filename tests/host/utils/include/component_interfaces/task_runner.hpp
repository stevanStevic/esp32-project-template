#ifndef TESTS_HOST_UTILS_INCLUDE_COMPONENT_INTERFACES_TASK_RUNNER_HPP_
#define TESTS_HOST_UTILS_INCLUDE_COMPONENT_INTERFACES_TASK_RUNNER_HPP_

#include <functional>

// ─── TaskRunner ───────────────────────────────────────────────────────────────
//
// Abstract interface for executing a task function in a background context.
// Implementations:
//   - RtosTaskRunner    (production: FreeRTOS xTaskCreate)
//   - StdThreadRunner   (host tests: std::thread)
// ──────────────────────────────────────────────────────────────────────────────

class TaskRunner
{
public:
    virtual ~TaskRunner() = default;

    /// @brief Start the background task.
    /// @param task Function to run. Called once in the background context.
    virtual void start(std::function<void()> task) = 0;
};

#endif // TESTS_HOST_UTILS_INCLUDE_COMPONENT_INTERFACES_TASK_RUNNER_HPP_
