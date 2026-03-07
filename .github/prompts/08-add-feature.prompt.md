---
mode: agent
description: Implement an approved change request or incremental feature with minimal footprint.
---

# Phase 8: Add Feature / Implement Change Request

You are an expert ESP-IDF embedded C++ developer implementing an APPROVED change request in an existing
project. You have a confirmed impact report from Phase 7.

## Mandatory Reading

1. Read `docs/skills/change-management.md` — follow the minimal-change principle exactly
2. Read `docs/skills/coding-standards.md` — all conventions apply
3. Read `docs/skills/testing-strategy.md` — for test update patterns

## Step 1: Confirm Scope

Ask the user to paste the approved impact report from Phase 7 if not already in context.

Confirm:
- Exactly which files will be modified (not created)
- Exactly which files will be created (if any)
- Which test cases will be updated vs which will stay identical

Do not begin implementation until this is confirmed.

## Step 2: Implement

Apply changes in this order:

1. **Interface changes first** (if any) — `component_interfaces/*_if.hpp`
   - Prefer adding methods over changing existing signatures
   - If a signature changes, note every call site that must be updated

2. **Implementation changes** — `src/components/*.cpp` and `components/*.hpp`
   - Apply the pattern identified in the impact report (guard, callback, flag, or plain)
   - Keep changes local — do not refactor surrounding code

3. **Composition root** — `main/main.cpp`, `app.hpp`, `app.cpp` (only if wiring changed)

4. **Test updates** — `tests/host/`
   - Update stale test cases: change expectation, add a comment `// Updated: <reason>`
   - Fix non-compiling tests (signature changes)
   - Add new test cases for every acceptance criterion
   - Do NOT delete test cases — if behavior is gone, mark with `TEST_IGNORE_MESSAGE("removed: <reason>")`

5. **Build system** — `main/CMakeLists.txt`, `tests/host/CMakeLists.txt` (only if new files added)

## Step 3: Quality Checklist

Before outputting files, verify:
- [ ] Minimal-change principle respected (no unrelated edits)
- [ ] Every acceptance criterion has a corresponding test case
- [ ] No existing passing test was silently broken
- [ ] Every modified interface still compiles at all call sites
- [ ] Every mock in tests/host/ that implements a changed interface is updated
- [ ] m_ prefix, TAG, Rule of Five preserved in any modified class
- [ ] No ESP-IDF includes introduced into interface or business-logic headers
- [ ] clang-format applied to all modified files

## Step 4: Summary

After generating all file changes, produce a summary table:

| File | Change type | Reason |
|---|---|---|
| `main/src/components/foo.cpp` | Modified | Added charging guard in shutdown() |
| `tests/host/foo_tests.cpp` | Modified | Updated 1 test, added 2 new test cases |
| (no new files) | — | — |
