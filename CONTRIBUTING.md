# Contributing

## Development Workflow

This project uses [Git Flow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow)
with [Conventional Commits](https://www.conventionalcommits.org/).

### Branch Naming

Format: `<type>/<short-description>` (lowercase, hyphens only, max 40 characters)

| Prefix | Purpose |
|---|---|
| `feature/` | New feature or capability |
| `bugfix/` | Bug fix |
| `release/` | Release preparation |
| `hotfix/` | Emergency fix on production |

Examples:
```
feature/spi-dma-support
bugfix/health-check-timeout
release/v1.2.0
hotfix/nvs-corruption
```

### Commit Messages

Format: `<type>(<scope>): <subject>`

| Type | When to use |
|---|---|
| `feat` | New feature |
| `fix` | Bug fix |
| `refactor` | Code change with no functional change |
| `test` | Adding or updating tests |
| `docs` | Documentation only |
| `build` | Build system or dependency changes |
| `ci` | CI/CD changes |
| `chore` | Housekeeping (gitignore, formatting) |

The `scope` is the component or area affected:
```
feat(spi): add DMA transfer support
fix(health): handle NXP timeout correctly
test(gpio): add host unit tests for interrupt handler
build(cmake): add sensor_manager to SRCS list
ci(release): add signing key restore step
```

Rules:
- Subject line: imperative mood, lowercase, no period, max 72 chars
- One logical change per commit
- No "WIP" commits in PRs — squash before requesting review

### Pull Request Process

1. Branch from `develop` (for features/bugfixes) or `main` (for hotfixes)
2. Keep PRs focused — one logical change per PR
3. Self-review your diff before requesting review
4. At least **1 approval** required before merge
5. Respond to all review comments before merging
6. Squash commits or rebase onto latest `develop` before merge

### Code Standards

See `docs/skills/coding-standards.md` for:
- C++17 naming conventions
- File organization rules
- Clang-format settings
- Include guard format

### Adding a New Component

Follow the 4-file pattern (see `docs/skills/architecture-patterns.md`):
1. `main/include/component_interfaces/<name>_if.hpp` — interface
2. `main/include/components/<name>.hpp` — implementation header
3. `main/src/components/<name>.cpp` — implementation (ESP-IDF calls here only)
4. `tests/host/<name>_tests.cpp` — host unit test

Update:
- `main/CMakeLists.txt` — add source to SRCS
- `tests/host/CMakeLists.txt` — add test target

Or use the agent command: `/generate-component` (Claude Code) or the prompt `04-generate-component` (Copilot).

### Running Tests Before Opening a PR

```bash
# Host tests (no hardware needed — run these always)
./scripts/run_all_tests.sh --host

# All tests (requires hardware)
./scripts/run_all_tests.sh --all
```

### Formatting

Before committing, run clang-format:

```bash
find main/ -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

The devcontainer formats on save automatically.
