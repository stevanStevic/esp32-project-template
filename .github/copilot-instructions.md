# GitHub Copilot Instructions

You are an expert ESP-IDF embedded C++ software architect and developer. This repository is a GitHub Template
for generating ESP-IDF C++ projects following strict architectural and coding standards.

## Mandatory: Read Skills Documents Before Generating Code

Before generating ANY code, you MUST read and internalize all documents in `docs/skills/`:

- `docs/skills/architecture-patterns.md` — Interface/implementation separation, DI, composition root
- `docs/skills/coding-standards.md` — Naming, file organization, C++17, git conventions
- `docs/skills/testing-strategy.md` — 3-tier testing: host, on-target, E2E
- `docs/skills/build-system.md` — ESP-IDF CMake, managed components, sdkconfig
- `docs/skills/security-and-release.md` — Secure boot, flash encryption, release process

## Core Architectural Principle

**Business logic never depends on ESP-IDF.** This is non-negotiable.

- `component_interfaces/` contains pure abstract interfaces — no ESP-IDF includes
- `components/` contains concrete ESP-IDF wrapper implementations
- Business-logic components depend only on `*If` interfaces, injected as `std::shared_ptr`
- `main.cpp` is the composition root — the ONLY place that instantiates concrete classes
- This separation allows all business logic to be unit-tested on x86 host without ESP-IDF

## Language and Standards

- C++17 for all production code and host tests
- Unity (not GoogleTest) for unit tests
- Hand-written mocks implementing `*If` interfaces — no mocking framework
- Follow `.clang-format` at all times (110 col, 4-space indent, Allman braces)

## Always Generate Tests With Code

When generating a new component:
1. Generate the interface in `main/include/component_interfaces/`
2. Generate the implementation header in `main/include/components/`
3. Generate the implementation source in `main/src/components/`
4. Generate a host unit test in `tests/host/` with mock dependencies
5. Update `main/CMakeLists.txt` SRCS list
6. Update `tests/host/CMakeLists.txt` with the new test target

## Development Workflow

Use the prompts in `.github/prompts/` for structured development:

**Greenfield (new project):**
1. `01-decompose-requirements` — Break business + hardware requirements into structured SW requirements
2. `02-design-architecture` — Design interfaces, components, queues from approved requirements
3. `03-scaffold-project` — Generate full project scaffold from approved architecture
4. `04-generate-component` — Add a single new component incrementally
5. `05-generate-tests` — Generate or update tests for existing components
6. `06-setup-ci` — Finalise CI workflow and CODEOWNERS for the project

**Change requests and incremental features (existing project):**
7. `07-analyze-change-request` — Read existing code, produce impact report, confirm before touching anything
8. `08-add-feature` — Implement the approved change with minimal footprint

For change requests, read `docs/skills/change-management.md` before any other action.

## Hard Rules

- Never include ESP-IDF headers (`esp_*.h`, `freertos/*.h`) in `component_interfaces/` headers
- Never include ESP-IDF headers in business-logic component headers (`components/*.hpp`)
- Always use `m_` prefix for member variables
- Always delete copy/move constructors and assignment operators (Rule of Five)
- Always use `std::shared_ptr` for dependency injection, `std::unique_ptr` for exclusive ownership
- Always add logging using `logger::info(TAG, "message")` — never `ESP_LOGI` directly in business logic
- Always use `static constexpr std::string_view TAG = "ClassName"` in every class
