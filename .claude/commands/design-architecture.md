# Design Architecture

Design the complete component architecture from approved Phase 1 requirements.

## Steps

1. Read `docs/skills/architecture-patterns.md` — mandatory
2. Read `docs/skills/coding-standards.md` — mandatory
3. Ask user to confirm/paste the approved requirements document

## Output

**Interface List** — for each `*If` interface: filename, methods, users, implementor

**Component List** — for each concrete class: filename, responsibility, constructor args, ESP-IDF APIs

**Queue Topology** (if applicable) — producer, consumer, type, capacity

**Command/Response Types** (if applicable) — `std::variant` type aliases

**Mermaid Class Diagram** — interfaces, classes, relationships

**Composition Root Wiring** — pseudocode for main.cpp

**Requirement Traceability** — table: requirement → implementing component

## Confirmation

End with:
> "Does this architecture match your intentions? Reply 'yes' to proceed to /scaffold-project, or provide corrections."

Do NOT generate any code files until the user confirms.
