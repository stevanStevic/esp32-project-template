# Coding Standards

All generated C++ code MUST follow these standards exactly. Agents generating code must reference this
document before producing any output.

---

## Language Standard

- **C++17** for all production code and host tests
- C is only permitted in ESP-IDF component glue code that explicitly requires it (e.g., `extern "C"` hooks)

---

## Naming Conventions

| Entity | Convention | Example |
|---|---|---|
| Namespaces | `snake_case` | `component_interfaces`, `logger` |
| Classes | `PascalCase` | `SpiManager`, `HealthManager` |
| Interfaces | `PascalCase` + `If` suffix | `SpiHandlerIf`, `QueueIf<T>` |
| Methods | `camelCase` | `processRequest()`, `getValue()` |
| Member variables | `m_` prefix + `camelCase` | `m_spiHandler`, `m_requestQueue` |
| Static constexpr TAG | `TAG` | `static constexpr std::string_view TAG = "MyClass"` |
| Constants | `SCREAMING_SNAKE_CASE` | `MAX_RETRY_COUNT`, `DEFAULT_TIMEOUT_MS` |
| Enum classes | `PascalCase` name, `PascalCase` values | `enum class State { Idle, Running, Done };` |
| Files | `snake_case` | `spi_manager.hpp`, `health_manager.cpp` |
| Test files | `<component>_tests.cpp` | `health_manager_tests.cpp` |
| Include guards | `PATH_STYLE_HPP_` | `#ifndef MAIN_INCLUDE_COMPONENTS_SPI_MANAGER_HPP_` |
| Type aliases | `PascalCase` | `using TaskRunnerPtr = std::shared_ptr<TaskRunner>;` |
| Free functions | `snake_case` (in namespace) | `logger::info(tag, msg)` |

---

## File Organization

```
main/
├── main.cpp                              # Composition root — wires all components
├── include/
│   ├── app.hpp                           # Application orchestrator declaration
│   ├── component_interfaces/             # Pure abstract interfaces and shared types
│   │   ├── foo_if.hpp                    # One interface per file
│   │   ├── queue.hpp                     # QueueIf<T> template interface
│   │   ├── commands.hpp                  # std::variant command/response types
│   │   └── logger.hpp                   # Free-function logging declarations
│   └── components/                       # Concrete implementation headers
│       └── foo.hpp                       # One class per file, mirrors interface name
└── src/
    ├── app.cpp                           # App implementation
    └── components/                       # Implementation source files
        ├── foo.cpp                       # One per header in components/
        └── logger.cpp                   # Only file allowed to include <esp_log.h>
```

Rules:
- **One class per file** — no exceptions
- **Interface files** go in `component_interfaces/`, named `snake_case_if.hpp`
- **Implementation headers** go in `components/`, named `snake_case.hpp`
- **Implementation sources** go in `src/components/`, named `snake_case.cpp`
- Header and source file names mirror each other exactly

---

## Include Guards

Always use include guards (not `#pragma once`):

```cpp
#ifndef MAIN_INCLUDE_COMPONENTS_MY_COMPONENT_HPP_
#define MAIN_INCLUDE_COMPONENTS_MY_COMPONENT_HPP_

// ... content ...

#endif // MAIN_INCLUDE_COMPONENTS_MY_COMPONENT_HPP_
```

The guard name reflects the full path from the repo root, with `/` replaced by `_` and all uppercase.

---

## Include Ordering (enforced by .clang-format)

1. C system headers (`<stdio.h>`, `<string.h>`)
2. C++ standard library headers (`<memory>`, `<variant>`, `<string_view>`)
3. Other library headers with angle brackets (`<esp_log.h>`, `<freertos/FreeRTOS.h>`)
4. Library headers with quotes from subdirectories (`"unity/unity.h"`)
5. Project headers with quotes (`"component_interfaces/logger.hpp"`)

Blank line between each group.

---

## Formatting

Enforced by `.clang-format` at the root of the repository:

- **Column limit**: 110 characters
- **Indent**: 4 spaces (no tabs)
- **Brace style**: Allman (opening brace on new line for classes, functions, control flow, namespaces)
- **Pointer alignment**: Right (`int *ptr`, not `int* ptr`)
- **Short lambdas**: always on single line
- **Short functions**: never on single line
- **Constructor initializers**: break before colon

Run clang-format before committing: `clang-format -i main/src/**/*.cpp main/include/**/*.hpp`

---

## Class Structure Order

```cpp
class MyClass : public MyIf
{
public:
    // 1. Constructor(s)
    explicit MyClass(std::shared_ptr<DepIf> dep);

    // 2. Rule of Five
    MyClass(const MyClass &) = delete;
    MyClass &operator=(const MyClass &) = delete;
    MyClass(MyClass &&) = delete;
    MyClass &operator=(MyClass &&) = delete;
    ~MyClass() override = default;

    // 3. Public interface methods (override keyword required)
    void doThing() override;
    int  getValue() const override;

private:
    // 4. TAG first
    static constexpr std::string_view TAG = "MyClass";
    // 5. Constants
    static constexpr uint32_t TIMEOUT_MS = 3000;
    // 6. Member variables (m_ prefix)
    std::shared_ptr<DepIf> m_dep;
};
```

---

## Git Conventions

### Conventional Commits

Format: `<type>(<scope>): <subject>`

| Type | When to use |
|---|---|
| `feat` | New feature or capability |
| `fix` | Bug fix |
| `refactor` | Code change with no functional change |
| `test` | Adding or updating tests |
| `docs` | Documentation only |
| `build` | Build system or dependency changes |
| `ci` | CI/CD changes |
| `chore` | Housekeeping (gitignore, formatting, etc.) |

Examples:
```
feat(spi): add DMA transfer support
fix(health): handle NXP timeout correctly
test(gpio): add host unit tests for interrupt handler
build(cmake): add new component to SRCS list
```

### Branch Naming

Format: `<type>/<short-description>` (lowercase, hyphens, max 40 chars)

Examples: `feature/spi-dma-support`, `bugfix/health-timeout`, `release/v1.2.0`

Types: `feature/`, `bugfix/`, `release/`, `hotfix/`

### PR Process

- 1 approval required before merge
- Self-review the diff before requesting review
- Address all review comments before merge
- One logical change per PR
