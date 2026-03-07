# Scaffold Project

Generate the complete project scaffold from the approved Phase 2 architecture.

## Steps

1. Read ALL docs/skills/*.md files
2. Ask user to confirm/paste the approved architecture

## Files to Generate

For each interface:
- `main/include/component_interfaces/<name>_if.hpp`

For each component:
- `main/include/components/<name>.hpp`
- `main/src/components/<name>.cpp`
- `tests/host/<name>_tests.cpp`

Always update:
- `main/main.cpp` — full composition root wiring
- `main/include/app.hpp` — constructor params for all top-level components
- `main/src/app.cpp` — init() and run() implementations
- `main/CMakeLists.txt` — SRCS list
- `tests/host/CMakeLists.txt` — test targets
- `main/include/component_interfaces/commands.hpp` — if queued messaging needed

## Quality Checklist

Before writing files:
- [ ] No ESP-IDF headers in `component_interfaces/` or `components/` headers
- [ ] Rule of Five on every concrete class
- [ ] TAG on every class
- [ ] m_ prefix on all members
- [ ] All interface methods pure virtual
- [ ] Include guards on all headers
- [ ] At least 2 test cases per component
- [ ] CMakeLists files updated with all new files
