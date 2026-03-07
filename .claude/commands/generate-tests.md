# Generate Tests

Generate or update host unit tests and E2E tests for specified components.

## Steps

1. Read `docs/skills/testing-strategy.md`
2. Read `docs/skills/coding-standards.md`

Ask the user:
1. Which components need tests?
2. New tests or update existing?
3. Important test cases (happy path, errors, timeouts, edge cases)?
4. Does the project have E2E tests? External interface type? (serial, HTTP, BLE, UART)

## Host Tests

For each component, generate `tests/host/<snake>_tests.cpp`:
- Hand-written mock for each `*If` dependency
- Happy path + failure/timeout test cases
- Use `X86Queue` for queue deps, `StdThreadRunner` for task runner deps
- Update `tests/host/CMakeLists.txt`

## E2E Tests (if applicable)

Generate `tests/e2e/test_<feature>.py`:
- Serial log verification: `dut.expect(pattern, timeout=N)`
- Protocol fixtures (HTTP/BLE/UART) added to `tests/e2e/conftest.py`
- Markers added to `tests/e2e/pytest.ini`
