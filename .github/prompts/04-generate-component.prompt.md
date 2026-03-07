---
mode: agent
description: Add a single new component to the project incrementally.
---

# Phase 4: Add Component

You are an expert ESP-IDF embedded C++ developer. Add a single new component to the existing project.

## Instructions

Read before starting:
- `docs/skills/architecture-patterns.md`
- `docs/skills/coding-standards.md`
- `docs/skills/testing-strategy.md`

Ask the user for:
1. Component name (e.g., `SensorManager`)
2. Responsibility (one sentence)
3. Interface methods it must expose
4. Dependencies: which existing `*If` interfaces it depends on
5. Does it need a background task? (requires `TaskRunner`)
6. Does it communicate via queue? (requires `QueueIf<T>`)
7. Which ESP-IDF APIs will the concrete implementation use?

## Files to Create

1. `main/include/component_interfaces/<snake_name>_if.hpp`
2. `main/include/components/<snake_name>.hpp`
3. `main/src/components/<snake_name>.cpp`
4. `tests/host/<snake_name>_tests.cpp`

## Files to Update

5. `main/CMakeLists.txt` — add `"src/components/<snake_name>.cpp"` to SRCS
6. `tests/host/CMakeLists.txt` — add new `add_executable` + `add_test` block
7. `main/main.cpp` — add instantiation and wiring in composition root
8. `main/include/app.hpp` — add new `std::shared_ptr<XxxIf>` constructor parameter and member
9. `main/src/app.cpp` — add init call for the new component

## Quality Checklist

- [ ] No ESP-IDF headers in interface or component header
- [ ] Rule of Five applied to concrete class
- [ ] TAG defined in both header and source
- [ ] m_ prefix on all member variables
- [ ] At least 2 test cases: happy path + error/timeout path
- [ ] Mock class covers all interface methods
