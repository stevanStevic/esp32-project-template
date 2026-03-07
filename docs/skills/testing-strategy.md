# Testing Strategy

All projects follow a 3-tier testing strategy. Agents generating code MUST generate corresponding tests for
every new component. This document describes each tier, when to use it, and the implementation patterns.

---

## The 3-Tier Model

```
Tier 1: Host Tests       Fast, no hardware, no ESP-IDF — run on every commit
Tier 2: On-Target Tests  Require ESP32 hardware — run before releases
Tier 3: E2E Tests        Require full hardware setup — run before releases
```

Each tier increases confidence but also cost (time, hardware). The goal is to:
- Catch logic bugs cheaply in Tier 1
- Catch hardware-integration bugs in Tier 2
- Catch system-level behaviour bugs in Tier 3

---

## Tier 1: Host Unit Tests

**Location**: `tests/host/`
**Framework**: Unity (C unit test framework bundled with ESP-IDF)
**Language**: C++17
**Hardware required**: None
**No ESP-IDF dependency**: All business-logic components must be compilable without ESP-IDF

### How They Work

Because business-logic components depend only on abstract interfaces (see `architecture-patterns.md`), they
can be tested by replacing all interface dependencies with hand-written mock classes.

Platform-specific replacements in `tests/host/utils/`:
- `x86_queue.hpp` — `QueueIf<T>` backed by `std::queue` + `std::mutex`
- `std_thread_runner.hpp` — `TaskRunner` backed by `std::thread`
- `logger_mock.cpp` — `logger::*` free functions using `printf`

### Mock Pattern

```cpp
// Hand-written mock — no mocking framework needed
class MockFoo : public FooIf
{
public:
    bool doThingCalled = false;
    int  returnValue   = 0;

    void doThing() override { doThingCalled = true; }
    int  getValue() const override { return returnValue; }
};
```

Mocks:
- Use boolean flags (`wasCalled`, `initCalled`) to verify method was called
- Use captured fields to verify arguments
- Return configurable values for methods that return data
- Pre-populate queues to simulate upstream component responses

### Test File Pattern

```cpp
// my_component_tests.cpp
#include "unity.h"
#include "components/my_component.hpp"
#include "x86_queue.hpp"

// Mock dependencies
class MockDep : public DepIf
{
public:
    bool called = false;
    void doThing() override { called = true; }
};

void test_my_component_calls_dep(void)
{
    // Arrange
    auto mockDep = std::make_shared<MockDep>();
    MyComponent component(mockDep);

    // Act
    component.process();

    // Assert
    TEST_ASSERT_TRUE(mockDep->called);
}

void test_my_component_handles_timeout(void)
{
    // Arrange: empty queue simulates timeout
    auto queue = std::make_shared<X86Queue<MyResponse>>(5);
    auto mockDep = std::make_shared<MockDep>();
    MyComponent component(mockDep, queue);

    // Act
    auto result = component.processWithResponse();

    // Assert
    TEST_ASSERT_EQUAL(ResultCode::Timeout, result.code);
}

extern "C" void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_my_component_calls_dep);
    RUN_TEST(test_my_component_handles_timeout);
    UNITY_END();
}
```

### Adding a New Test Target

In `tests/host/CMakeLists.txt`, add a new executable:

```cmake
add_executable(my_component_tests
    my_component_tests.cpp
    ${PROJECT_ROOT}/main/src/components/my_component.cpp
    utils/src/logger_mock.cpp)

target_include_directories(my_component_tests PRIVATE
    ${PROJECT_ROOT}/main/include
    utils/include
    ${UNITY_DIR})

target_link_libraries(my_component_tests unity_lib)
add_test(NAME my_component_tests COMMAND my_component_tests)
```

### Running Host Tests

```bash
./tests/host/run_tests.sh
```

---

## Tier 2: On-Target Tests

**Location**: `tests/esp32/`
**Framework**: Unity + ESP-IDF test runner
**Hardware required**: ESP32-S3 connected via USB

For testing behaviour that cannot be replicated on host:
- Real FreeRTOS scheduling and timing
- Real SPI/I2C/UART peripheral behaviour
- NVS, OTA, partition API behaviour
- PSRAM allocations

The test project at `tests/esp32/` is a separate ESP-IDF project with `idf_build_set_property(MINIMAL_BUILD
ON)` and a custom partition table that fits test binaries.

### Running On-Target Tests

```bash
./tests/esp32/run_tests.sh
```

---

## Tier 3: E2E / Integration Tests

**Location**: `tests/e2e/`
**Framework**: pytest + pytest-embedded
**Hardware required**: Full hardware setup with the device flashed

E2E tests exercise the firmware through its **real external interfaces** — whatever those are for the
specific project. The verification strategy is not fixed:

| Project type | E2E verification approach |
|---|---|
| HTTP REST API device | Send HTTP requests, parse JSON responses |
| BLE device | Use `bleak` library to connect, send commands, read characteristics |
| UART command device | Send/receive over serial (`dut.write_line()`, `dut.expect()`) |
| Autonomous device | Parse serial log output (`dut.expect(pattern)`) |
| GPIO-triggered device | Monitor GPIO states, trigger inputs |

The base `conftest.py` provides the device-under-test via pytest-embedded serial. Each project adds its own
protocol-specific fixtures on top.

### Base conftest.py Pattern

```python
# tests/e2e/conftest.py
import pytest

@pytest.fixture(scope="session")
def build_firmware(request):
    """Build firmware before running E2E tests."""
    import subprocess
    subprocess.run(["idf.py", "build"], check=True)

@pytest.fixture
def dut(request):
    """Device under test via pytest-embedded serial connection."""
    # pytest-embedded provides the `dut` fixture automatically when configured
    # in pytest.ini. This fixture exists to document the dependency.
    return request.getfixturevalue("dut")
```

### E2E Test Pattern

```python
# test_my_feature.py
import pytest

@pytest.mark.esp32s3
def test_boot_message(dut):
    """Verify device logs expected boot message."""
    dut.expect("Application running", timeout=10)

@pytest.mark.esp32s3
def test_sensor_reading(dut):
    """Verify device logs sensor readings periodically."""
    dut.expect(r"Temperature: \d+\.\d+ C", timeout=30)

@pytest.mark.esp32s3
def test_uart_command_response(dut):
    """Verify device responds to UART command."""
    dut.write_line("GET_STATUS")
    dut.expect("STATUS:OK", timeout=5)
```

### Adding Project-Specific Fixtures

If your project has HTTP, BLE, or another protocol, add fixture files alongside `conftest.py`:

```python
# fixtures/http_client.py — for HTTP REST API projects
import pytest, requests

@pytest.fixture
def http_client(esp32_ip):
    return requests.Session()
```

### Running E2E Tests

```bash
cd tests/e2e && pytest --target esp32s3 -m esp32s3
```

---

## Running All Tests

```bash
./scripts/run_all_tests.sh --all     # Host + on-target + E2E
./scripts/run_all_tests.sh --host    # Host only (no hardware needed)
./scripts/run_all_tests.sh --esp32   # On-target only
./scripts/run_all_tests.sh --e2e     # E2E only
```
