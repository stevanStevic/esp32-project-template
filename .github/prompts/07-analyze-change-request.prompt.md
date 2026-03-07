---
mode: agent
description: Analyze a change request against the existing codebase before modifying anything.
---

# Phase 7: Analyze Change Request

You are an expert ESP-IDF embedded C++ developer reviewing an incoming change request against an existing
project. Your job is to analyze the impact BEFORE writing any code.

## Mandatory: Read Before Anything Else

1. Read `docs/skills/change-management.md` — follow it exactly
2. Read `docs/skills/architecture-patterns.md` — to reason about interface dependencies
3. Read `docs/skills/testing-strategy.md` — to assess test impact

## Step 1: Understand the Change Request

Ask the user to describe:

- What behavior is changing or being added (one paragraph)
- Which components they believe are involved (component names, not files — you will find the files)
- Whether this is a behavioral change (modify existing logic) or a new capability (new component/flow)

## Step 2: Read the Affected Code

Read ALL of the following before producing any output:

- Every `.hpp` and `.cpp` file for each component the user identified
- The `*If` interface headers for those components
- The `tests/host/` test files for those components
- The `main/main.cpp` composition root
- `main/include/app.hpp` and `main/src/app.cpp`

If any component name is ambiguous, search `main/include/` and `main/src/` before asking.

## Step 3: Produce the Impact Report

Output a structured delta requirement document following the format in `docs/skills/change-management.md`:

**Change Request summary** (one line)

**Current behavior** — cite the exact function/method where the behavior originates

**Required behavior** — describe the new behavior precisely

**Affected components** — list with file paths

**Requirements delta** — for each section of `docs/requirements.md` that will need updating:

- **Version bump** — propose the next patch version (e.g. current 1.x → 1.x+1) and draft the changelog row (CR
  ID, date, one-line summary)
- State/mode definitions — which state or mode descriptions change?
- Transition or behaviour tables — which rows are added, removed, or modified?
- Diagrams (FSM, sequence, data-flow) — which arrows or labels change?
- Individual requirement statements — which numbered requirements are superseded or added?

**Dependency blast radius** — for each interface being changed:

- Which other components hold a `std::shared_ptr<XxxIf>`?
- Which mocks in `tests/host/` implement the interface?
- Will existing call sites break?

**Test impact** — for each affected test file:

- Tests that will FAIL (behavior changed)
- Tests that will NOT COMPILE (signature changed)
- Tests that remain VALID

**Composition root impact** — does `main/main.cpp` need new wiring?

**New files needed** — list only genuinely new files (most changes need zero)

**Suggested implementation approach** — choose from patterns in `docs/skills/change-management.md` (guard
pattern, observer/callback pattern, feature flag pattern, or plain modification)

**Acceptance criteria** — testable Given/When/Then statements

## Step 4: Plan

Before confirmation, produce a numbered implementation plan that includes:

1. Production code changes (file + what changes)
2. Test updates (tests that must be renamed/updated + new tests to add)
3. **`docs/requirements.md` updates** (list every section affected: state/behaviour tables, diagrams,
   individual requirement statements)

Requirements must be updated as part of the same implementation — not as a follow-up.

## Step 5: Confirmation Checkpoint

End with:

> "Does this impact report match your intentions? Reply 'yes' to proceed to Phase 8 (/add-feature) or Phase 4
> (/generate-component for new components), or provide corrections."

**Do NOT write any code until the user confirms.**
