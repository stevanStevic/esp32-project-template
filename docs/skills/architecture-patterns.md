# Architecture Patterns

This document defines the mandatory architectural patterns for all ESP-IDF C++ projects generated from this
template. Agents generating code MUST follow all patterns described here.

---

## Core Principle: Interface/Implementation Separation

**Business logic never depends on ESP-IDF directly.** All ESP-IDF API calls are encapsulated in wrapper
classes that implement pure abstract interfaces. Business-logic components depend only on those interfaces.

```
component_interfaces/     ← Pure abstract interfaces (no ESP-IDF includes)
    foo_if.hpp            ← class FooIf { virtual void doThing() = 0; };
    logger.hpp            ← free-function declarations only

components/               ← Concrete ESP-IDF wrapper implementations
    foo.hpp               ← class Foo : public FooIf { ... };

src/components/
    foo.cpp               ← #include <esp_xxx.h> lives HERE only
    logger.cpp            ← #include <esp_log.h> lives HERE only
```

This separation means every business-logic class can be compiled, linked, and tested on a host machine (x86)
using Unity without any ESP-IDF dependency.

---

## Interface Definition Rules

All interfaces live in `main/include/component_interfaces/` and follow these rules:

```cpp
#ifndef MAIN_INCLUDE_COMPONENT_INTERFACES_FOO_IF_HPP_
#define MAIN_INCLUDE_COMPONENT_INTERFACES_FOO_IF_HPP_

class FooIf
{
public:
    virtual ~FooIf() = default;              // Always virtual destructor

    virtual void doThing() = 0;              // All methods pure virtual
    virtual int  getValue() const = 0;       // Const methods declared const
};

#endif
```

Rules:
- Filename: `snake_case_if.hpp`
- Class name: `PascalCaseIf`  (always `If` suffix)
- No ESP-IDF includes, no FreeRTOS includes, no platform-specific types
- Use only standard C++ types in method signatures (`std::string_view`, `uint8_t`, `std::span`, etc.)

---

## Concrete Implementation Rules

All implementations live in `main/include/components/` (header) and `main/src/components/` (source):

```cpp
// foo.hpp
#ifndef MAIN_INCLUDE_COMPONENTS_FOO_HPP_
#define MAIN_INCLUDE_COMPONENTS_FOO_HPP_

#include "component_interfaces/foo_if.hpp"

class Foo : public FooIf
{
public:
    explicit Foo(/* ESP-IDF config params */);

    Foo(const Foo &) = delete;               // Rule of Five: always delete
    Foo &operator=(const Foo &) = delete;
    Foo(Foo &&) = delete;
    Foo &operator=(Foo &&) = delete;

    ~Foo() override = default;

    void doThing() override;
    int  getValue() const override;

private:
    static constexpr std::string_view TAG = "Foo";
    // ESP-IDF handle types go here (e.g. spi_device_handle_t m_handle;)
};

#endif
```

```cpp
// foo.cpp
#include "components/foo.hpp"
#include "component_interfaces/logger.hpp"
#include <esp_xxx.h>   // ← ESP-IDF includes ONLY in .cpp files

void Foo::doThing()
{
    logger::info(TAG, "doThing called");
    // ESP-IDF API calls here
}
```

---

## Dependency Injection Pattern

All dependencies are passed into constructors as `std::shared_ptr`. Components never instantiate their own
dependencies.

```cpp
// ✅ Correct: depends on interface, injected via constructor
class MyManager
{
public:
    explicit MyManager(
        std::shared_ptr<FooIf> foo,
        std::shared_ptr<BarIf> bar)
        : m_foo(std::move(foo))
        , m_bar(std::move(bar))
    {
    }

private:
    std::shared_ptr<FooIf> m_foo;
    std::shared_ptr<BarIf> m_bar;
};

// ❌ Wrong: instantiates concrete class, untestable
class MyManager
{
    Foo m_foo;  // Direct concrete dependency — never do this
};
```

The composition root (`main/main.cpp`) is the **only** place that instantiates concrete classes.

---

## Composition Root Pattern

`main/main.cpp` wires the entire application:

```cpp
extern "C" void app_main()
{
    // 1. Create ESP-IDF wrapper components (concrete implementations)
    auto foo = std::make_shared<Foo>(/* config */);
    auto bar = std::make_shared<Bar>(/* config */);

    // 2. Create inter-task queues
    auto cmdQueue = std::make_shared<RtosQueue<MyCommand>>(10);

    // 3. Create business-logic components (injected deps)
    auto manager = std::make_shared<MyManager>(foo, bar, cmdQueue);

    // 4. Create and run App
    auto app = std::make_unique<App>(manager);
    app->init();
    app->run();
}
```

---

## Inter-Task Communication: Queue Pattern

When multiple FreeRTOS tasks need to communicate, use typed queues behind the `QueueIf<T>` interface:

```cpp
// component_interfaces/queue.hpp
template<typename T>
class QueueIf
{
public:
    virtual ~QueueIf() = default;
    virtual bool send(T item, uint32_t timeoutMs) = 0;
    virtual bool receive(T &item, uint32_t timeoutMs) = 0;
};
```

Two implementations:
- `RtosQueue<T>` — wraps FreeRTOS `xQueueCreate` / `xQueueSend` / `xQueueReceive` (production)
- `x86_queue.hpp` — wraps `std::queue` + `std::mutex` + `std::condition_variable` (host tests)

This allows producer-consumer patterns to be fully tested on host without any FreeRTOS dependency.

---

## Command/Response Pattern (for complex inter-task messaging)

When a subsystem needs to accept multiple distinct message types, use `std::variant`:

```cpp
// component_interfaces/commands.hpp
#include <variant>

struct PingCommand { };
struct GetInfoCommand { };
struct FlashCommand { std::string fileName; uint32_t address; };

using RequestCommand = std::variant<PingCommand, GetInfoCommand, FlashCommand>;

struct AckResponse { bool success; };
struct InfoResponse { std::string version; };

using ResponseCommand = std::variant<AckResponse, InfoResponse>;
```

Dispatch with `std::visit`:

```cpp
std::visit([this](auto &&cmd) { handleCommand(std::forward<decltype(cmd)>(cmd)); }, command);
```

---

## Strategy Pattern

When multiple implementations of the same behaviour need to be swapped at runtime, implement one interface and
select the implementation at the composition root:

```cpp
class UploadTargetIf
{
public:
    virtual ~UploadTargetIf() = default;
    virtual void upload(std::span<const uint8_t> data) = 0;
};

// Different implementations:
class FlashUploadTarget  : public UploadTargetIf { ... };
class UartUploadTarget   : public UploadTargetIf { ... };
```

---

## App Class

`App` is the application orchestrator — it owns `std::shared_ptr` references to all top-level
business-logic components and sequences their initialisation. It does NOT instantiate anything itself.

```cpp
class App
{
public:
    explicit App(
        std::shared_ptr<ManagerAIf> managerA,
        std::shared_ptr<ManagerBIf> managerB);

    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;
    App &operator=(App &&) = delete;
    ~App() = default;

    void init();
    void run();   // Blocks until shutdown signal

private:
    std::shared_ptr<ManagerAIf> m_managerA;
    std::shared_ptr<ManagerBIf> m_managerB;
};
```

---

## Rule of Five

All classes explicitly delete copy and move operations unless copying/moving is intentionally supported:

```cpp
MyClass(const MyClass &) = delete;
MyClass &operator=(const MyClass &) = delete;
MyClass(MyClass &&) = delete;
MyClass &operator=(MyClass &&) = delete;
```

Most ESP-IDF wrapper classes hold OS handles (queues, semaphores, tasks) that must not be copied or moved.

---

## RAII and Ownership

- `std::shared_ptr` for component lifetimes shared across multiple consumers
- `std::unique_ptr` for exclusive ownership (e.g. `App` itself in `main.cpp`)
- `std::unique_ptr` for queue message ownership when passing heap-allocated objects through queues
- Never use raw `new`/`delete` in business logic code
