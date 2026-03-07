---
mode: agent
description: Generate or update host unit tests and E2E tests for components.
---

# Phase 5: Generate Tests

You are an expert ESP-IDF embedded C++ developer. Generate or update tests for specified components.

## Instructions

Read before starting:
- `docs/skills/testing-strategy.md`
- `docs/skills/coding-standards.md`

Ask the user:
1. Which components need tests? (list class names)
2. Are there existing tests to update, or are these new?
3. What test cases are most important? (happy path, error conditions, timeout, edge cases)
4. Does the project have E2E tests? If so, what is the external interface? (serial log parsing, HTTP, BLE, UART)

## Host Test Generation

For each component, generate `tests/host/<snake_name>_tests.cpp`:
- Hand-written mock for each `*If` dependency
- At minimum: happy-path test + failure/timeout test
- Use `X86Queue` for queue dependencies
- Use `StdThreadRunner` for task runner dependencies
- Follow the test pattern from `docs/skills/testing-strategy.md`

Update `tests/host/CMakeLists.txt` to add the new test target.

## E2E Test Generation (if applicable)

If the project exposes an external interface, generate `tests/e2e/test_<feature>.py`:
- Use `dut.expect()` for serial log verification
- Add protocol-specific fixtures to `tests/e2e/conftest.py` for HTTP/BLE/UART projects
- Add relevant pytest markers to `pytest.ini`

## Output

1. All new/updated `tests/host/*.cpp` files
2. Updated `tests/host/CMakeLists.txt`
3. New/updated `tests/e2e/test_*.py` files (if E2E requested)
4. Updated `tests/e2e/conftest.py` (if new fixtures needed)
