# Change Management

This document defines how agents MUST handle change requests and incremental feature additions to an existing
project. Follow these rules any time you are modifying code that already exists — do NOT apply the greenfield
scaffolding approach (Phases 1–3) to an existing codebase.

---

## The Minimal-Change Principle

Every change request must be implemented with the smallest possible footprint:

- Prefer adding behavior over changing behavior
- Prefer adding a new method to an interface over changing an existing method signature
- Prefer adding a new component over modifying an existing one that has dependents
- Never rename, move, or restructure existing files unless the change request explicitly requires it
- Never "clean up" or refactor code not touched by the change request

Rationale: every line changed is a regression risk. Unnrelated edits make code review harder and obscure the
intent of the change.

---

## Step 0: Read Before Writing

Before making ANY modification, you MUST:

1. Read the affected source files (`.cpp` and `.hpp`) in full
2. Read the interface header(s) for any interface you are about to change
3. Read the existing test file(s) for every component you will modify
4. Identify the composition root wiring in `main/main.cpp` — check if new injection is needed

Do not infer file contents from filenames. Read them.

---

## Impact Analysis (Required Before Any Code Change)

Produce an impact report with these sections before writing any code:

### Components affected
List each component (class name + file) whose source **or** interface you intend to modify.

### Dependency blast radius
For each modified interface (`*If`):
- Who calls it? (list all components with a `std::shared_ptr<XxxIf>` member)
- Will the existing call sites still compile if you change the interface?
- Will any mock in `tests/host/` need updating?

### Test impact
For each test file in `tests/host/`:
- Which test cases will **fail** after the change (expected behavior changes)?
- Which test cases will **not compile** after the change (signature changes)?
- Which test cases are **still valid** and must continue to pass?

### Composition root impact
- Does `main/main.cpp` need new instantiation or wiring?
- Does `App`'s constructor or `init()`/`run()` need updating?

### New files needed
List only if genuinely new — a new interface, a new component. Most behavioral changes require zero new files.

---

## When to Change an Interface vs. Add to It vs. Leave It Alone

| Situation | Action |
|---|---|
| New behavior, new callers | Add a new method to the interface |
| Existing behavior needs a parameter added | Add overload; deprecate old signature if needed |
| Behavior change that all callers must adopt | Change method, update all call sites and mocks |
| Change isolated to one implementation | Modify the `.cpp` only — leave interface unchanged |
| New component needed to coordinate behavior | Add component via Phase 4 pattern; inject via constructor |

**Never remove a method from an interface** without first verifying no call sites exist.

---

## Delta Requirement Format

When capturing a change request, produce a focused delta document — not a full FR/NFR decomposition:

```
Change Request: <one-line summary>
Trigger: <what the client/user asked for>

Current behavior:
  <describe exactly what currently happens, citing the relevant component>

Required behavior:
  <describe exactly what must happen instead, or in addition>

Affected components (initial assessment):
  - <ComponentName> in main/src/components/<file>.cpp
  - <InterfaceName>If in main/include/component_interfaces/<file>_if.hpp (if interface changes)

Out of scope:
  <explicitly list things that are NOT changing, to prevent scope creep>

Acceptance criteria:
  - Given <precondition>, when <action>, then <expected outcome>
  - (repeat for each testable scenario)
```

---

## Regression Checklist (Before Submitting Changes)

- [ ] All existing tests that were valid still pass (no unintended behavior changes)
- [ ] Any test whose expected behavior changed has been updated with the new expectation and a comment explaining why
- [ ] Any test that no longer compiles due to signature changes has been updated
- [ ] New test cases cover every acceptance criterion from the delta requirement
- [ ] No unrelated files were modified
- [ ] `main/CMakeLists.txt` SRCS list is updated if any new `.cpp` files were added
- [ ] `tests/host/CMakeLists.txt` is updated if any new test executables were added
- [ ] `main/main.cpp` wiring is updated if new components or dependencies were introduced
- [ ] Clang-format has been applied to all modified files

---

## Common Patterns for Behavioral Changes

### Guard Pattern (block behavior in a certain state)

Do not modify the protected component. Instead, have the decision-maker read a new or existing state query:

```cpp
// In PowerManager::shutdown():
if (m_charger->getState() == ChargingState::Charging)
{
    logger::info(TAG, "Shutdown blocked: device is charging.");
    return ShutdownResult::Blocked;
}
// ... existing shutdown logic
```

The `ChargerManagerIf::getState()` method may already exist. Check before adding anything.

### Observer/Callback Pattern (react to state transitions)

If component A needs to react when component B changes state, add a callback registration to B's interface:

```cpp
class ChargerManagerIf
{
public:
    // New method — existing callers are unaffected
    virtual void registerStateChangeCallback(std::function<void(ChargingState)> cb) = 0;
};
```

The existing `getState()` call sites are untouched. Only the new consumer registers a callback.

### Feature Flag Pattern (compile-time or runtime gating)

For changes that need to be toggleable, use Kconfig (compile-time) or NVS (runtime) rather than hardcoding:

```kconfig
config CHARGING_KEEPS_DEVICE_ACTIVE
    bool "Keep device active while charging"
    default y
```

```cpp
#if CONFIG_CHARGING_KEEPS_DEVICE_ACTIVE
    if (m_charger->getState() == ChargingState::Charging) { return ShutdownResult::Blocked; }
#endif
```
