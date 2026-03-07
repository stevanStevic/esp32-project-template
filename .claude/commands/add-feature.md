# Add Feature / Implement Change Request

Implement an approved change request from /analyze-change-request with minimal footprint.

## Steps

1. Read `docs/skills/change-management.md` — minimal-change principle is mandatory
2. Read `docs/skills/coding-standards.md`
3. Read `docs/skills/testing-strategy.md`
4. Ask user to paste the approved impact report if not in context

## Implementation Order

1. **Interface changes** (if any) — prefer adding methods over changing signatures
2. **Implementation changes** — apply the agreed pattern (guard/callback/flag/plain)
3. **Composition root** — only if new wiring is needed
4. **Test updates**:
   - Update stale cases: change expectation, add comment `// Updated: <reason>`
   - Fix non-compiling cases
   - Add new cases for every acceptance criterion
   - Never delete test cases — use `TEST_IGNORE_MESSAGE("removed: <reason>")` instead
5. **Build system** — only if new files were added

## Quality Checklist

- [ ] Minimal-change principle: no unrelated edits
- [ ] Every acceptance criterion has a test case
- [ ] No existing passing test silently broken
- [ ] All call sites of changed interfaces still compile
- [ ] All mocks for changed interfaces updated
- [ ] m_ prefix, TAG, Rule of Five preserved
- [ ] No ESP-IDF includes in interfaces or business-logic headers
- [ ] clang-format applied to all modified files

## Summary

After all changes, produce a table:

| File | Change type | Reason |
|---|---|---|
| `main/src/components/foo.cpp` | Modified | Added charging guard in shutdown() |
| `tests/host/foo_tests.cpp` | Modified | Updated 1 test, added 2 new cases |
