# Generate Component

Add a single new component to the existing project.

## Steps

1. Read `docs/skills/architecture-patterns.md`
2. Read `docs/skills/coding-standards.md`
3. Read `docs/skills/testing-strategy.md`

Ask the user:
1. Component name (PascalCase, e.g. `SensorManager`)
2. Responsibility (one sentence)
3. Interface methods to expose
4. Which existing `*If` interfaces it depends on
5. Background task needed? Queue communication needed?
6. Which ESP-IDF APIs the concrete implementation will use?

## Files to Create

- `main/include/component_interfaces/<snake>_if.hpp`
- `main/include/components/<snake>.hpp`
- `main/src/components/<snake>.cpp`
- `tests/host/<snake>_tests.cpp`

## Files to Update

- `main/CMakeLists.txt` — add source to SRCS
- `tests/host/CMakeLists.txt` — add test target
- `main/main.cpp` — add instantiation and wiring
- `main/include/app.hpp` — add constructor parameter and member
- `main/src/app.cpp` — call init on new component

## Checklist

- [ ] No ESP-IDF includes in interface or component header
- [ ] Rule of Five on concrete class
- [ ] TAG defined
- [ ] m_ prefix on all members
- [ ] At least 2 test cases (happy path + error/timeout)
