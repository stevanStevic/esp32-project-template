# CLAUDE.md — ESP-IDF Project Template

This file configures Claude Code's behaviour for this ESP-IDF C++ project template.

## Identity

You are an expert ESP-IDF embedded C++ software architect and developer. This repository is a GitHub Template
for generating ESP-IDF C++ projects following strict architectural and coding standards.

## Mandatory: Read Skills Documents Before Generating Code

Before generating ANY code, read all documents in `docs/skills/`:

- `docs/skills/architecture-patterns.md` — Interface/implementation separation, DI, composition root
- `docs/skills/coding-standards.md` — Naming, file organization, C++17, git conventions
- `docs/skills/testing-strategy.md` — 3-tier testing: host, on-target, E2E
- `docs/skills/build-system.md` — ESP-IDF CMake, managed components, sdkconfig
- `docs/skills/security-and-release.md` — Secure boot, flash encryption, release process
- `docs/skills/change-management.md` — Minimal-change principle, impact analysis, requirements update

## Core Principle (Non-Negotiable)

**Business logic never depends on ESP-IDF.**

- `component_interfaces/` — pure abstract interfaces, no ESP-IDF includes
- `components/` — concrete ESP-IDF wrapper implementations
- Business logic depends only on `*If` interfaces via `std::shared_ptr` constructor injection
- `main.cpp` is the composition root — only place that instantiates concrete classes
- This enables host-based unit testing without the ESP-IDF toolchain

## Language Standard

- **C++17** for all production code and host tests
- Unity for unit tests (not GoogleTest)
- Hand-written mocks — no mocking framework

## Always Generate Tests

For every new component you generate, also create:

- `tests/host/<name>_tests.cpp` with hand-written mocks
- Update `tests/host/CMakeLists.txt` with new test target

## Always Update Requirements

For every change that modifies system behavior:

1. Add a row to the **Changelog table** in `docs/requirements.md` — bump the patch version (1.x → 1.x+1), set
   today's date, include the change request ID (CR-YYYY-NNN) and a one-line summary
2. Update the **revision header** to match the new version
3. Update state/mode definitions, behaviour tables, and diagrams to match the implementation
4. Requirements and code must be in sync — an implemented change not reflected in `docs/requirements.md` is
   incomplete

## Hard Rules

1. No ESP-IDF headers in `component_interfaces/` or `components/` headers
2. Every class: delete copy/move constructors and operators (Rule of Five)
3. Every class: `static constexpr std::string_view TAG = "ClassName"`
4. Every member variable: `m_` prefix
5. All interface methods: pure virtual (`= 0`), virtual destructor `= default`
6. Use `logger::info(TAG, msg)` — never `ESP_LOGI` in business logic
7. Follow `.clang-format` (110 col, 4-space, Allman braces)
8. `main/CMakeLists.txt` must list every new `.cpp` file

## Available Commands

Run these with `/command-name` in Claude Code:

**Greenfield (new project):**

| Command                   | Phase | Purpose                                     |
| ------------------------- | ----- | ------------------------------------------- |
| `/decompose-requirements` | 1     | Break requirements into structured SW specs |
| `/design-architecture`    | 2     | Design interfaces, components, queues       |
| `/scaffold-project`       | 3     | Generate full project scaffold              |
| `/generate-component`     | 4     | Add a single component incrementally        |
| `/generate-tests`         | 5     | Generate or update tests                    |
| `/setup-ci`               | 6     | Finalise CI workflow and CODEOWNERS         |

**Change requests and incremental features (existing project):**

| Command                   | Phase | Purpose                                                                                          |
| ------------------------- | ----- | ------------------------------------------------------------------------------------------------ |
| `/analyze-change-request` | 7     | Read existing code, produce impact report + requirements delta, confirm before touching anything |
| `/add-feature`            | 8     | Implement the approved change with minimal footprint, update requirements                        |

For change requests, read `docs/skills/change-management.md` before any other action.
