#!/bin/bash
# Run all test tiers or a selected subset.
#
# Usage:
#   ./scripts/run_all_tests.sh --all      # All tiers
#   ./scripts/run_all_tests.sh --host     # Host unit tests only (no hardware)
#   ./scripts/run_all_tests.sh --esp32    # On-target tests only
#   ./scripts/run_all_tests.sh --e2e      # E2E tests only

set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

RUN_HOST=false
RUN_ESP32=false
RUN_E2E=false

PASS=0
FAIL=0

# ── Parse arguments ───────────────────────────────────────────────────────────
if [[ $# -eq 0 ]]; then
    echo "Usage: $0 [--all | --host | --esp32 | --e2e]"
    exit 1
fi

while [[ $# -gt 0 ]]; do
    case $1 in
        --all)   RUN_HOST=true; RUN_ESP32=true; RUN_E2E=true; shift ;;
        --host)  RUN_HOST=true;  shift ;;
        --esp32) RUN_ESP32=true; shift ;;
        --e2e)   RUN_E2E=true;   shift ;;
        *)
            echo "Unknown argument: $1"
            exit 1
            ;;
    esac
done

# ── Helper ────────────────────────────────────────────────────────────────────
run_suite() {
    local name="$1"
    local cmd="$2"

    echo ""
    echo "════════════════════════════════════════"
    echo "  Running: ${name}"
    echo "════════════════════════════════════════"

    if eval "${cmd}"; then
        echo "✅ ${name} PASSED"
        PASS=$((PASS + 1))
    else
        echo "❌ ${name} FAILED"
        FAIL=$((FAIL + 1))
    fi
}

# ── Run selected tiers ────────────────────────────────────────────────────────
if [[ "${RUN_HOST}" == true ]]; then
    run_suite "Host Unit Tests" "${ROOT_DIR}/tests/host/run_tests.sh"
fi

if [[ "${RUN_ESP32}" == true ]]; then
    run_suite "ESP32 On-Target Tests" "${ROOT_DIR}/tests/esp32/run_tests.sh"
fi

if [[ "${RUN_E2E}" == true ]]; then
    run_suite "E2E Tests" "cd ${ROOT_DIR}/tests/e2e && pytest --target esp32s3"
fi

# ── Summary ───────────────────────────────────────────────────────────────────
echo ""
echo "════════════════════════════════════════"
echo "  Test Summary"
echo "════════════════════════════════════════"
echo "  Passed: ${PASS}"
echo "  Failed: ${FAIL}"
echo "════════════════════════════════════════"

if [[ "${FAIL}" -gt 0 ]]; then
    exit 1
fi

exit 0
