# Analyze Change Request

Analyze an incoming change request against the existing codebase before writing any code.

## Steps

1. Read `docs/skills/change-management.md` — mandatory
2. Read `docs/skills/architecture-patterns.md`
3. Read `docs/skills/testing-strategy.md`

## Gather Information

Ask the user:

- What behavior is changing or being added?
- Which components are involved (names)?
- Is this a behavioral change or a new capability?

## Read Existing Code

Before producing output, read ALL of:

- `.hpp` and `.cpp` files for every identified component
- Their `*If` interface headers
- Their test files in `tests/host/`
- `main/main.cpp`, `main/include/app.hpp`, `main/src/app.cpp`

## Produce Impact Report

Follow the delta requirement format from `docs/skills/change-management.md`:

**Change Request** — one-line summary
**Current behavior** — cite the exact method
**Required behavior** — precise description
**Affected components** — list with file paths
**Dependency blast radius** — which components/mocks depend on changing interfaces
**Test impact** — FAIL / NOT COMPILE / VALID categorization per
test file
**Composition root impact** — new wiring needed?
**New files needed** — be conservative (most changes = zero new files)
**Suggested pattern** — guard / observer / feature flag / plain modification
**Acceptance criteria** — Given/When/Then per scenario

## Confirmation

End with:

> "Does this impact report match your intentions? Reply 'yes' to proceed to /add-feature, or provide
> corrections."

Do NOT write any code until confirmed.
