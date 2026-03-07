---
mode: agent
description: Generate the full project scaffold from the approved architecture.
---

# Phase 3: Full Project Scaffold

You are an expert ESP-IDF embedded C++ developer. You have an approved architecture from Phase 2. Generate
the complete project scaffold now.

## Instructions

Read ALL skills documents before generating any code:
- `docs/skills/architecture-patterns.md`
- `docs/skills/coding-standards.md`
- `docs/skills/testing-strategy.md`
- `docs/skills/build-system.md`

## Files to Generate

For each interface identified in the architecture:
1. `main/include/component_interfaces/<name>_if.hpp` — pure abstract interface

For each concrete component:
2. `main/include/components/<name>.hpp` — implementation header
3. `main/src/components/<name>.cpp` — implementation source (ESP-IDF calls here only)
4. `tests/host/<name>_tests.cpp` — host unit test with hand-written mocks

Always generate/update:
5. `main/main.cpp` — complete composition root wiring all components
6. `main/include/app.hpp` — updated with constructor parameters for all top-level components
7. `main/src/app.cpp` — updated init() and run() implementations
8. `main/CMakeLists.txt` — updated SRCS list with all new source files
9. `tests/host/CMakeLists.txt` — updated with all new test targets
10. `main/include/component_interfaces/commands.hpp` — variant command types (if needed)

## Quality Checklist

Before outputting any file, verify:
- [ ] No ESP-IDF headers in `component_interfaces/` or `components/` headers
- [ ] Every class deletes copy/move (Rule of Five)
- [ ] Every class has `static constexpr std::string_view TAG`
- [ ] All member variables have `m_` prefix
- [ ] All interface methods are pure virtual with `= 0`
- [ ] Interface destructor is `virtual ~XxxIf() = default`
- [ ] Implementation destructor is `~Xxx() override = default`
- [ ] Include guards follow `MAIN_INCLUDE_PATH_STYLE_HPP_` pattern
- [ ] Every test file has a corresponding mock for each dependency
- [ ] `main/CMakeLists.txt` lists every new .cpp file
- [ ] `tests/host/CMakeLists.txt` has an `add_executable` and `add_test` for every new test

## Output Order

Generate files in this order:
1. All interface headers
2. All component headers  
3. All component sources
4. Updated main.cpp and App files
5. All test files
6. Updated CMakeLists files
