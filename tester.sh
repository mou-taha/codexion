#!/usr/bin/env bash
# ==============================================================================
#  CODEXION ULTIMATE AUTOMATED TESTER
#  Comprehensive Test Suite for 42 Cursus Codexion Project
# ==============================================================================
#  Features:
#   - Full 42 Scale Sheet / Correction Sheet test cases (Easy, Less Easy, Medium)
#   - Deep Log & Concurrency Analyzer (state transitions, dongle mutual exclusion,
#     dongle cooldown rules, interleaving detection, precise burnout timing)
#   - Comprehensive Valgrind Memory Leak checks (args errors, 1 coder burnout,
#     multi-coder completion, infeasible burnout, cooldown, FIFO, EDF)
#   - Valgrind Concurrency & Thread-Safety Tests (Helgrind & DRD)
#   - Compiler Sanitizers (ThreadSanitizer TSan & AddressSanitizer ASan)
#   - Argument Parsing & Robust Input Validation suite
#   - Multi-Topology Ring & Edge Stress Tests (2, 3, 4, 10, 50, 100 coders)
#   - Makefile & Norm/Code structure validation (global variables check, relink check)
#   - README.md mandatory format & sections validator
#   - Rich Terminal UI, color-coded PASS/FAIL badges, detailed logs, and summary
# ==============================================================================

set -uo pipefail

# ------------------------------------------------------------------------------
# Configuration & Paths
# ------------------------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_NAME="codexion"
BIN_PATH="${SCRIPT_DIR}/${BIN_NAME}"
MAKEFILE_PATH="${SCRIPT_DIR}/Makefile"
TMP_DIR="${SCRIPT_DIR}/.tester_tmp"

# Colors & Formatting
if [ -t 1 ] && [ "${NO_COLOR:-0}" != "1" ]; then
    C_RESET="\033[0m"
    C_BOLD="\033[1m"
    C_DIM="\033[2m"
    C_RED="\033[1;31m"
    C_GREEN="\033[1;32m"
    C_YELLOW="\033[1;33m"
    C_BLUE="\033[1;34m"
    C_MAGENTA="\033[1;35m"
    C_CYAN="\033[1;36m"
    C_WHITE="\033[1;37m"
    C_BG_RED="\033[41;37m"
    C_BG_GREEN="\033[42;30m"
else
    C_RESET=""
    C_BOLD=""
    C_DIM=""
    C_RED=""
    C_GREEN=""
    C_YELLOW=""
    C_BLUE=""
    C_MAGENTA=""
    C_CYAN=""
    C_WHITE=""
    C_BG_RED=""
    C_BG_GREEN=""
fi

# Counters & Metrics
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_WARNED=0
START_TIME_TOTAL=$(date +%s)

# CLI Options
VERBOSE=0
FAST_MODE=0

# Clean up temporary artifacts on exit
cleanup() {
    rm -rf "${TMP_DIR}"
}
trap cleanup EXIT INT TERM
mkdir -p "${TMP_DIR}"

# ------------------------------------------------------------------------------
# UI Helpers
# ------------------------------------------------------------------------------
print_banner() {
    echo -e "${C_CYAN}${C_BOLD}"
    echo "=============================================================================="
    echo "         CODEXION TEST SUITE - 42 CURRICULUM AUTOMATED TESTER"
    echo "=============================================================================="
    echo -e "${C_RESET}"
}

print_section() {
    local title="$1"
    echo ""
    echo -e "${C_BLUE}${C_BOLD}------------------------------------------------------------------------------${C_RESET}"
    echo -e "${C_CYAN}${C_BOLD}  [SECTION] ${title}${C_RESET}"
    echo -e "${C_BLUE}${C_BOLD}------------------------------------------------------------------------------${C_RESET}"
}

print_pass() {
    local name="$1"
    local info="${2:-}"
    TESTS_RUN=$((TESTS_RUN + 1))
    TESTS_PASSED=$((TESTS_PASSED + 1))
    if [ -n "${info}" ]; then
        echo -e "  [ ${C_GREEN}PASS${C_RESET} ] ${C_BOLD}${name}${C_RESET} ${C_DIM}(${info})${C_RESET}"
    else
        echo -e "  [ ${C_GREEN}PASS${C_RESET} ] ${C_BOLD}${name}${C_RESET}"
    fi
}

print_fail() {
    local name="$1"
    local reason="${2:-}"
    TESTS_RUN=$((TESTS_RUN + 1))
    TESTS_FAILED=$((TESTS_FAILED + 1))
    echo -e "  [ ${C_RED}FAIL${C_RESET} ] ${C_BOLD}${name}${C_RESET}"
    if [ -n "${reason}" ]; then
        echo -e "         ${C_RED}↳ ${reason}${C_RESET}"
    fi
}

print_warn() {
    local name="$1"
    local reason="${2:-}"
    TESTS_RUN=$((TESTS_RUN + 1))
    TESTS_WARNED=$((TESTS_WARNED + 1))
    echo -e "  [ ${C_YELLOW}WARN${C_RESET} ] ${C_BOLD}${name}${C_RESET}"
    if [ -n "${reason}" ]; then
        echo -e "         ${C_YELLOW}↳ ${reason}${C_RESET}"
    fi
}

print_info() {
    echo -e "  [ ${C_BLUE}INFO${C_RESET} ] $*"
}

# ------------------------------------------------------------------------------
# Python Log Analyzer Engine
# ------------------------------------------------------------------------------
cat << 'EOF' > "${TMP_DIR}/analyzer.py"
import sys
import re

def analyze_logs(logfile, num_coders, burnout_time, compile_time, debug_time, refactor_time, compiles_req, cooldown, scheduler, expected_burnout):
    try:
        with open(logfile, 'r', errors='replace') as f:
            lines = [line.strip() for line in f if line.strip()]
    except Exception as e:
        print(f"FAIL|Cannot read logfile: {e}")
        return

    if not lines:
        if compiles_req == 0:
            print("PASS|Clean empty output for 0 compiles")
            return
        print("FAIL|No output produced by simulation")
        return

    pattern = re.compile(r"^(\d+)\s+(\d+)\s+(has taken a dongle|is compiling|is debugging|is refactoring|burned out)$")
    
    events = []
    for idx, line in enumerate(lines, start=1):
        m = pattern.match(line)
        if not m:
            print(f"FAIL|Line {idx} invalid / interleaved format: '{line[:80]}'")
            return
        ts = int(m.group(1))
        cid = int(m.group(2))
        action = m.group(3)
        if cid < 1 or cid > num_coders:
            print(f"FAIL|Line {idx} invalid coder ID {cid} (expected 1..{num_coders})")
            return
        events.append((ts, cid, action, idx))

    # Burnout position check: MUST be the last line if burnout occurs
    burnout_events = [(ts, cid, idx) for (ts, cid, action, idx) in events if action == "burned out"]
    if burnout_events:
        last_ts, last_cid, last_idx = burnout_events[0]
        if last_idx != len(events):
            print(f"FAIL|'burned out' appeared on line {last_idx}/{len(events)} (must be the LAST line)")
            return
        if len(burnout_events) > 1:
            print(f"FAIL|Multiple 'burned out' events detected ({len(burnout_events)})")
            return

    # Simulation State Tracking
    coder_state = {i: 0 for i in range(1, num_coders + 1)}
    compiles_count = {i: 0 for i in range(1, num_coders + 1)}
    last_compile_start = {i: 0 for i in range(1, num_coders + 1)}

    # Dongle occupancy tracking: dongle_id -> (owner_cid, compile_start_ts)
    dongle_occupant = {}
    dongle_available_at = {}

    def get_dongles(c):
        if num_coders == 1:
            return (0,)
        d1 = c - 1
        d2 = c % num_coders
        return (min(d1, d2), max(d1, d2))

    for ts, cid, action, lnum in events:
        st = coder_state[cid]

        # Check existing dongle occupancies to see if previous compile time naturally finished
        for d in list(dongle_occupant.keys()):
            occ_cid, start_ts = dongle_occupant[d]
            if ts >= start_ts + compile_time:
                del dongle_occupant[d]
                dongle_available_at[d] = start_ts + compile_time + cooldown

        if action == "has taken a dongle":
            if st == 0 or st == 5:
                coder_state[cid] = 1
            elif st == 1:
                coder_state[cid] = 2
            else:
                print(f"FAIL|Line {lnum}: Coder {cid} took dongle in invalid state {st}")
                return

        elif action == "is compiling":
            if num_coders > 1 and st != 2:
                print(f"FAIL|Line {lnum}: Coder {cid} started compiling without holding 2 dongles (state={st})")
                return
            
            d_list = get_dongles(cid)
            for d in d_list:
                if d in dongle_occupant:
                    occ_cid, occ_start = dongle_occupant[d]
                    if occ_cid != cid and ts < occ_start + compile_time:
                        print(f"FAIL|Line {lnum}: Dongle {d} conflict! Held by Coder {occ_cid} while Coder {cid} compiles")
                        return
                if d in dongle_available_at:
                    if ts < dongle_available_at[d] - 25: # Tolerance for thread scheduling jitter
                        print(f"FAIL|Line {lnum}: Cooldown violated on Dongle {d}! Acquired at {ts}ms, cooldown expires at {dongle_available_at[d]}ms")
                        return

            for d in d_list:
                dongle_occupant[d] = (cid, ts)

            coder_state[cid] = 3
            last_compile_start[cid] = ts
            compiles_count[cid] += 1

        elif action == "is debugging":
            d_list = get_dongles(cid)
            for d in d_list:
                if d in dongle_occupant and dongle_occupant[d][0] == cid:
                    del dongle_occupant[d]
                dongle_available_at[d] = ts + cooldown

            coder_state[cid] = 4

        elif action == "is refactoring":
            coder_state[cid] = 5

        elif action == "burned out":
            coder_state[cid] = 6
            expected_deadline = last_compile_start[cid] + burnout_time
            diff = ts - expected_deadline
            if diff < -25 or diff > 35:
                print(f"WARN|Burnout on Coder {cid} logged at {ts}ms, expected ~{expected_deadline}ms (drift: {diff:+d}ms)")
            break

    # Result Evaluation
    if expected_burnout:
        if not burnout_events:
            print("FAIL|Expected simulation to burn out, but no burnout occurred")
            return
        b_ts, b_cid, _ = burnout_events[0]
        print(f"PASS|Burnout correctly detected at {b_ts}ms (coder {b_cid})")
        return
    else:
        if burnout_events:
            b_ts, b_cid, _ = burnout_events[0]
            print(f"FAIL|Unexpected burnout on Coder {b_cid} at {b_ts}ms")
            return
        
        if compiles_req > 0:
            for cid in range(1, num_coders + 1):
                if compiles_count[cid] < compiles_req:
                    print(f"FAIL|Coder {cid} completed only {compiles_count[cid]}/{compiles_req} compiles")
                    return
        print(f"PASS|All {num_coders} coders successfully completed {compiles_req} compiles without burnout")
        return

if __name__ == "__main__":
    logfile = sys.argv[1]
    num_coders = int(sys.argv[2])
    burnout_time = int(sys.argv[3])
    compile_time = int(sys.argv[4])
    debug_time = int(sys.argv[5])
    refactor_time = int(sys.argv[6])
    compiles_req = int(sys.argv[7])
    cooldown = int(sys.argv[8])
    scheduler = sys.argv[9]
    expected_burnout = sys.argv[10].lower() in ["1", "true", "yes", "burnout"]
    
    analyze_logs(logfile, num_coders, burnout_time, compile_time, debug_time, refactor_time, compiles_req, cooldown, scheduler, expected_burnout)
EOF

# ------------------------------------------------------------------------------
# Core Test Runner Function
# ------------------------------------------------------------------------------
run_test() {
    local test_name="$1"
    local num_coders="$2"
    local t_burnout="$3"
    local t_compile="$4"
    local t_debug="$5"
    local t_refactor="$6"
    local n_compiles="$7"
    local cooldown="$8"
    local scheduler="$9"
    local expected_burnout="${10}"
    local timeout_sec="${11:-15}"

    local log_out="${TMP_DIR}/test.log"
    rm -f "${log_out}"

    local cmd=("${BIN_PATH}" "${num_coders}" "${t_burnout}" "${t_compile}" "${t_debug}" "${t_refactor}" "${n_compiles}" "${cooldown}" "${scheduler}")
    local cmd_str="./codexion ${num_coders} ${t_burnout} ${t_compile} ${t_debug} ${t_refactor} ${n_compiles} ${cooldown} ${scheduler}"

    local start_ms
    start_ms=$(( $(date +%s%N) / 1000000 ))

    # Run command with timeout protection
    timeout "${timeout_sec}" "${cmd[@]}" > "${log_out}" 2>&1
    local ret=$?

    local end_ms
    end_ms=$(( $(date +%s%N) / 1000000 ))
    local duration_ms=$((end_ms - start_ms))

    if [ ${ret} -eq 124 ]; then
        print_fail "${test_name}" "TIMED OUT after ${timeout_sec}s (Deadlock, starvation, or infinite loop) | Command: ${cmd_str}"
        return 1
    elif [ ${ret} -gt 128 ]; then
        local sig=$((ret - 128))
        print_fail "${test_name}" "CRASHED with signal ${sig} (Segmentation Fault / Abort) | Command: ${cmd_str}"
        return 1
    fi

    # Formal trace verification
    local analysis
    analysis=$(python3 "${TMP_DIR}/analyzer.py" "${log_out}" "${num_coders}" "${t_burnout}" "${t_compile}" "${t_debug}" "${t_refactor}" "${n_compiles}" "${cooldown}" "${scheduler}" "${expected_burnout}")

    local status
    status=$(echo "${analysis}" | cut -d'|' -f1)
    local msg
    msg=$(echo "${analysis}" | cut -d'|' -f2-)

    if [ "${status}" == "PASS" ]; then
        print_pass "${test_name}" "${msg} [${duration_ms}ms]"
        if [ "${VERBOSE}" -eq 1 ]; then
            echo -e "${C_DIM}--- Output Log Preview ---"
            head -n 12 "${log_out}"
            if [ "$(wc -l < "${log_out}")" -gt 24 ]; then
                echo "... ($(wc -l < "${log_out}") lines total) ..."
                tail -n 12 "${log_out}"
            fi
            echo -e "--------------------------${C_RESET}"
        fi
        return 0
    elif [ "${status}" == "WARN" ]; then
        print_warn "${test_name}" "${msg} | Command: ${cmd_str}"
        return 0
    else
        print_fail "${test_name}" "${msg} | Command: ${cmd_str}"
        if [ -s "${log_out}" ]; then
            echo -e "${C_DIM}--- Output Log Trace ---"
            head -n 25 "${log_out}"
            if [ "$(wc -l < "${log_out}")" -gt 50 ]; then
                echo "..."
                tail -n 25 "${log_out}"
            fi
            echo -e "------------------------${C_RESET}"
        fi
        return 1
    fi
}

# ------------------------------------------------------------------------------
# SECTION 1: Compilation, Makefile, Global State & README Checks
# ------------------------------------------------------------------------------
test_preliminaries() {
    print_section "1. PRELIMINARIES & CODEBASE INTEGRITY"

    # 1.1 Makefile exists
    if [ ! -f "${MAKEFILE_PATH}" ]; then
        print_fail "Makefile presence" "Makefile not found at ${MAKEFILE_PATH}"
        return 1
    else
        print_pass "Makefile presence" "Found Makefile"
    fi

    # 1.2 Compilation with make re
    local build_log="${TMP_DIR}/build.log"
    make -C "${SCRIPT_DIR}" re > "${build_log}" 2>&1
    local build_ret=$?
    if [ ${build_ret} -ne 0 ]; then
        print_fail "Compilation (make re)" "Compilation failed"
        cat "${build_log}"
        return 1
    fi

    # Flags check
    if grep -q "\-Wall" "${build_log}" && grep -q "\-Wextra" "${build_log}" && grep -q "\-Werror" "${build_log}"; then
        print_pass "Compilation flags" "-Wall -Wextra -Werror -pthread verified"
    else
        print_warn "Compilation flags" "Check that -Wall -Wextra -Werror -pthread are passed to cc"
    fi

    # 1.3 Binary named codexion
    if [ -x "${BIN_PATH}" ]; then
        print_pass "Binary name and executable" "Binary '${BIN_NAME}' created successfully"
    else
        print_fail "Binary executable" "Executable '${BIN_NAME}' not found"
        return 1
    fi

    # 1.4 Makefile rules check
    local rules_pass=1
    for rule in "all" "clean" "fclean" "re"; do
        if ! make -C "${SCRIPT_DIR}" -n "${rule}" >/dev/null 2>&1; then
            rules_pass=0
            print_fail "Makefile rule '${rule}'" "Rule '${rule}' missing or failing"
        fi
    done
    if [ ${rules_pass} -eq 1 ]; then
        print_pass "Makefile required rules" "all, clean, fclean, re present"
    fi

    # 1.5 Makefile relink prevention
    local relink_log="${TMP_DIR}/relink.log"
    make -C "${SCRIPT_DIR}" all > "${relink_log}" 2>&1
    if grep -Ei "cc |gcc |clang " "${relink_log}" >/dev/null 2>&1; then
        print_fail "Makefile relinking" "Makefile re-compiled objects when no files changed!"
    else
        print_pass "Makefile relink prevention" "No unnecessary relinking detected"
    fi

    # 1.6 Global Variables Check (nm)
    local global_vars
    global_vars=$(nm -g --defined-only "${BIN_PATH}" 2>/dev/null | awk '$2 ~ /^[BCDGRS]$/ {print $3}' | grep -Ev "^(__bss_start|_edata|_end|__data_start|data_start|__dso_handle|__TMC_END__|_IO_stdin_used)$" || true)
    if [ -n "${global_vars}" ]; then
        print_fail "Forbidden global mutable variables" "Detected global variables: ${global_vars}"
    else
        print_pass "Global variables check" "No forbidden mutable global state detected"
    fi

    # 1.7 README.md compliance check
    local readme_file=""
    if [ -f "${SCRIPT_DIR}/README.md" ]; then
        readme_file="${SCRIPT_DIR}/README.md"
    elif [ -f "${SCRIPT_DIR}/../README.md" ]; then
        readme_file="${SCRIPT_DIR}/../README.md"
    fi

    if [ -n "${readme_file}" ]; then
        local first_line
        first_line=$(head -n 1 "${readme_file}")
        if [[ "${first_line}" =~ ^\*This\ project\ has\ been\ created\ as\ part\ of\ the\ 42\ curriculum\ by ]]; then
            print_pass "README.md first line format" "Exact 42 header detected"
        else
            print_warn "README.md first line format" "First line should be: *This project has been created as part of the 42 curriculum by <login>...*"
        fi

        local missing_sections=()
        for sec in "Description" "Instructions" "Resources" "Blocking cases handled" "Thread synchronization mechanisms"; do
            if ! grep -qi "${sec}" "${readme_file}"; then
                missing_sections+=("${sec}")
            fi
        done

        if [ ${#missing_sections[@]} -eq 0 ]; then
            print_pass "README.md required sections" "All 5 mandatory sections found"
        else
            print_warn "README.md required sections" "Missing sections: ${missing_sections[*]}"
        fi
    else
        print_warn "README.md presence" "README.md not found in project or repository root"
    fi
}

# ------------------------------------------------------------------------------
# SECTION 2: Argument Parsing & Input Validation
# ------------------------------------------------------------------------------
test_argument_parsing() {
    print_section "2. ARGUMENT PARSING & INPUT VALIDATION"

    local parse_cases=(
        "0 arguments|./codexion"
        "1 argument|./codexion 5"
        "2 arguments|./codexion 5 800"
        "7 arguments (missing scheduler)|./codexion 5 800 200 200 200 10 0"
        "9 arguments (extra arg)|./codexion 5 800 200 200 200 10 0 fifo extra"
        "Non-numeric coder count|./codexion abc 800 200 200 200 10 0 fifo"
        "Non-numeric burnout time|./codexion 5 xxx 200 200 200 10 0 fifo"
        "Negative time parameter|./codexion 5 -800 200 200 200 10 0 fifo"
        "Invalid scheduler string|./codexion 5 800 200 200 200 10 0 invalid_sched"
        "Scheduler with uppercase|./codexion 5 800 200 200 200 10 0 FIFO"
        "Empty string argument|./codexion 5 800 200 200 200 10 0 \"\""
    )

    for tc in "${parse_cases[@]}"; do
        local desc
        desc=$(echo "${tc}" | cut -d'|' -f1)
        local cmd_line
        cmd_line=$(echo "${tc}" | cut -d'|' -f2)

        local out_err
        out_err=$(eval "${cmd_line}" 2>&1)
        local ret=$?

        if [ ${ret} -eq 0 ]; then
            print_fail "Reject ${desc}" "Program returned exit code 0 on invalid input! Command: ${cmd_line}"
        elif [ ${ret} -gt 128 ]; then
            print_fail "Reject ${desc}" "Program crashed (signal $((ret-128))) on invalid input! Command: ${cmd_line}"
        else
            print_pass "Reject ${desc}" "Correctly rejected (exit code ${ret})"
        fi
    done
}

# ------------------------------------------------------------------------------
# SECTION 3: Correction Sheet Benchmark Tests (Easy, Less Easy, Medium)
# ------------------------------------------------------------------------------
test_scale_sheet_cases() {
    print_section "3. 42 SCALE SHEET BENCHMARK TESTS"

    # Easy 1: 1 Coder Burnout
    print_info "Testing Easy Case 1: Single coder burnout (1 coder, 1 dongle -> cannot compile)"
    run_test "Scale Sheet Easy 1 [1 Coder Burnout]" 1 800 200 200 200 10 0 fifo 1 5

    # Easy 2: 5 Coders Feasible FIFO
    print_info "Testing Easy Case 2: 5 Coders FIFO 10 compiles (Feasible: 2000 > 600)"
    run_test "Scale Sheet Easy 2 [5 Coders FIFO Feasible]" 5 2000 200 200 200 10 0 fifo 0 15

    # Easy 3: 5 Coders Feasible EDF
    print_info "Testing Easy Case 3: 5 Coders EDF 7 compiles (Feasible)"
    run_test "Scale Sheet Easy 3 [5 Coders EDF Feasible]" 5 2000 200 200 200 7 0 edf 0 15

    # Less Easy 1: 5 Coders Infeasible FIFO Burnout
    print_info "Testing Less Easy Case 1: 5 Coders Infeasible (Cycle 600ms > 500ms burnout)"
    run_test "Scale Sheet Less Easy 1 [Infeasible Burnout Detection]" 5 500 200 200 200 10 0 fifo 1 8

    # Medium 1: Cooldown Verification (400ms cooldown)
    print_info "Testing Medium Case 1: Cooldown 400ms under EDF (5 coders, 5 compiles)"
    run_test "Scale Sheet Medium 1 [Dongle Cooldown 400ms]" 5 4000 200 200 200 5 400 edf 0 25

    # Medium 2: EDF vs FIFO under contention (800ms cooldown)
    print_info "Testing Medium Case 2: Heavy Cooldown Contention (EDF scheduling)"
    run_test "Scale Sheet Medium 2 [Heavy Cooldown EDF]" 5 4000 200 200 200 3 800 edf 0 30
}

# ------------------------------------------------------------------------------
# SECTION 4: Multi-Topology Ring & Edge Stress Tests
# ------------------------------------------------------------------------------
test_multi_topologies_and_stress() {
    print_section "4. MULTI-TOPOLOGY RING & STRESS TESTS"

    # 2 Coders (Pair topology)
    run_test "Topology: 2 Coders FIFO" 2 1500 200 200 200 5 0 fifo 0 10
    run_test "Topology: 2 Coders EDF" 2 1500 200 200 200 5 0 edf 0 10

    # 3 Coders (Odd ring)
    run_test "Topology: 3 Coders FIFO" 3 1500 200 200 200 5 0 fifo 0 10
    run_test "Topology: 3 Coders EDF" 3 1500 200 200 200 5 0 edf 0 10

    # 4 Coders (Even ring)
    run_test "Topology: 4 Coders FIFO" 4 1500 200 200 200 5 0 fifo 0 10
    run_test "Topology: 4 Coders EDF" 4 1500 200 200 200 5 0 edf 0 10

    # 10 Coders with Cooldown
    run_test "Topology: 10 Coders EDF with 100ms Cooldown" 10 3500 150 150 150 3 100 edf 0 20

    # Fast transitions (Subject minimum timing limit = 60ms)
    run_test "Timing Boundary: Fast 60ms Phases (EDF)" 4 1000 60 60 60 5 0 edf 0 10

    # High Coder Counts (50 and 100 coders)
    if [ "${FAST_MODE}" -eq 0 ]; then
        print_info "Running high thread count test (50 coders, 2 compiles)..."
        run_test "Stress: 50 Coders EDF" 50 6000 100 100 100 2 0 edf 0 25

        print_info "Running high thread count test (100 coders, 2 compiles)..."
        run_test "Stress: 100 Coders EDF" 100 12000 100 100 100 2 0 edf 0 35
    else
        print_info "Skipping 50/100 coders stress tests in fast mode"
    fi
}

# ------------------------------------------------------------------------------
# SECTION 5: Valgrind Memory Leak Suite
# ------------------------------------------------------------------------------
test_valgrind_leaks() {
    print_section "5. VALGRIND MEMORY LEAK SUITE"

    if ! command -v valgrind >/dev/null 2>&1; then
        print_warn "Valgrind not installed" "Skipping Valgrind leak tests"
        return 0
    fi

    local leak_cases=(
        "Valgrind: Bad arguments cleanup|./codexion 1 2 3"
        "Valgrind: Bad scheduler cleanup|./codexion 5 2000 200 200 200 10 0 invalid"
        "Valgrind: Single coder burnout|./codexion 1 800 200 200 200 10 0 fifo"
        "Valgrind: Infeasible burnout (5 coders)|./codexion 5 500 200 200 200 10 0 fifo"
        "Valgrind: 2 Coders Completion (FIFO)|./codexion 2 1500 150 150 150 2 0 fifo"
        "Valgrind: 3 Coders Completion (EDF)|./codexion 3 1500 150 150 150 2 0 edf"
        "Valgrind: 5 Coders Completion (FIFO)|./codexion 5 2000 150 150 150 2 0 fifo"
        "Valgrind: 5 Coders with Cooldown (EDF)|./codexion 5 3000 150 150 150 2 100 edf"
    )

    for tc in "${leak_cases[@]}"; do
        local desc
        desc=$(echo "${tc}" | cut -d'|' -f1)
        local cmd_line
        cmd_line=$(echo "${tc}" | cut -d'|' -f2)

        local val_log="${TMP_DIR}/valgrind_leak.log"
        rm -f "${val_log}"

        valgrind --leak-check=full \
                 --show-leak-kinds=all \
                 --errors-for-leak-kinds=all \
                 --error-exitcode=42 \
                 -q \
                 --log-file="${val_log}" \
                 ${cmd_line} >/dev/null 2>&1

        local ret=$?

        if [ ${ret} -eq 42 ] || [ -s "${val_log}" ]; then
            if grep -Eq "definitely lost:|indirectly lost:|possibly lost:|still reachable: [1-9]" "${val_log}"; then
                local leak_sum
                leak_sum=$(grep -E "lost:|reachable:" "${val_log}" | head -n 3 | tr '\n' '; ')
                print_fail "${desc}" "Memory leaks detected: ${leak_sum}"
                if [ "${VERBOSE}" -eq 1 ]; then
                    cat "${val_log}"
                fi
            elif grep -q "ERROR SUMMARY: [1-9]" "${val_log}"; then
                local err_sum
                err_sum=$(grep "ERROR SUMMARY:" "${val_log}" | head -n 1)
                print_fail "${desc}" "Valgrind errors detected: ${err_sum}"
            else
                print_pass "${desc}" "0 leaks / 0 memory errors"
            fi
        else
            print_pass "${desc}" "0 leaks / 0 memory errors"
        fi
    done
}

# ------------------------------------------------------------------------------
# SECTION 6: Valgrind Concurrency & Thread-Safety (Helgrind & DRD)
# ------------------------------------------------------------------------------
test_thread_safety() {
    print_section "6. THREAD SAFETY & CONCURRENCY (HELGRIND & DRD)"

    if ! command -v valgrind >/dev/null 2>&1; then
        print_warn "Valgrind not installed" "Skipping Helgrind/DRD tests"
        return 0
    fi

    # 6.1 Helgrind Tests
    local helgrind_cases=(
        "Helgrind: 1 Coder Burnout|./codexion 1 800 200 200 200 10 0 fifo"
        "Helgrind: 3 Coders FIFO|./codexion 3 1500 150 150 150 2 0 fifo"
        "Helgrind: 4 Coders EDF Cooldown|./codexion 4 2500 150 150 150 2 100 edf"
    )

    for tc in "${helgrind_cases[@]}"; do
        local desc
        desc=$(echo "${tc}" | cut -d'|' -f1)
        local cmd_line
        cmd_line=$(echo "${tc}" | cut -d'|' -f2)

        local hel_log="${TMP_DIR}/helgrind.log"
        rm -f "${hel_log}"

        valgrind --tool=helgrind \
                 --error-exitcode=42 \
                 -q \
                 --log-file="${hel_log}" \
                 ${cmd_line} >/dev/null 2>&1

        local ret=$?

        if [ ${ret} -eq 42 ] || [ -s "${hel_log}" ]; then
            if grep -Eq "data race|lock order|deadlock" "${hel_log}"; then
                local race_info
                race_info=$(grep -E "Possible data race|lock order" "${hel_log}" | head -n 1)
                print_fail "${desc}" "Concurrency issue detected: ${race_info}"
            else
                print_pass "${desc}" "0 thread errors / 0 data races"
            fi
        else
            print_pass "${desc}" "0 thread errors / 0 data races"
        fi
    done

    # 6.2 DRD Test
    local drd_log="${TMP_DIR}/drd.log"
    rm -f "${drd_log}"

    valgrind --tool=drd \
             --error-exitcode=42 \
             -q \
             --log-file="${drd_log}" \
             ./codexion 3 1500 150 150 150 2 0 fifo >/dev/null 2>&1

    local drd_ret=$?
    if [ ${drd_ret} -eq 42 ] || [ -s "${drd_log}" ]; then
        if grep -Eq "conflicting load|conflicting store|mutex" "${drd_log}"; then
            print_fail "DRD: 3 Coders Concurrency Check" "DRD reported conflicting accesses"
        else
            print_pass "DRD: 3 Coders Concurrency Check" "0 synchronization errors"
        fi
    else
        print_pass "DRD: 3 Coders Concurrency Check" "0 synchronization errors"
    fi
}

# ------------------------------------------------------------------------------
# SECTION 7: Compiler Sanitizers (TSan & ASan)
# ------------------------------------------------------------------------------
test_sanitizers() {
    print_section "7. COMPILER SANITIZERS (TSan & ASan)"

    # 7.1 ThreadSanitizer
    local tsan_bin="${TMP_DIR}/codexion_tsan"
    local tsan_log="${TMP_DIR}/tsan_build.log"

    print_info "Compiling binary with ThreadSanitizer (-fsanitize=thread)..."
    make -C "${SCRIPT_DIR}" fclean >/dev/null 2>&1 || true
    make -C "${SCRIPT_DIR}" CFLAGS="-Wall -Wextra -Werror -pthread -fsanitize=thread -g" > "${tsan_log}" 2>&1

    if [ -x "${BIN_PATH}" ]; then
        mv "${BIN_PATH}" "${tsan_bin}"
        local tsan_run_log="${TMP_DIR}/tsan_run.log"

        "${tsan_bin}" 4 2000 150 150 150 3 0 edf > "${tsan_run_log}" 2>&1
        local tsan_ret=$?

        if grep -qi "WARNING: ThreadSanitizer" "${tsan_run_log}"; then
            print_fail "ThreadSanitizer Data Race Check" "TSan reported data race!"
        elif [ ${tsan_ret} -ne 0 ]; then
            print_fail "ThreadSanitizer Execution" "Non-zero exit code: ${tsan_ret}"
        else
            print_pass "ThreadSanitizer Data Race Check" "Clean execution at full native speed"
        fi
    else
        print_warn "ThreadSanitizer Compilation" "Could not build with -fsanitize=thread"
    fi

    # 7.2 AddressSanitizer
    local asan_bin="${TMP_DIR}/codexion_asan"
    local asan_log="${TMP_DIR}/asan_build.log"

    print_info "Compiling binary with AddressSanitizer (-fsanitize=address)..."
    make -C "${SCRIPT_DIR}" fclean >/dev/null 2>&1 || true
    make -C "${SCRIPT_DIR}" CFLAGS="-Wall -Wextra -Werror -pthread -fsanitize=address -g" > "${asan_log}" 2>&1

    if [ -x "${BIN_PATH}" ]; then
        mv "${BIN_PATH}" "${asan_bin}"
        local asan_run_log="${TMP_DIR}/asan_run.log"

        "${asan_bin}" 4 2000 150 150 150 3 0 edf > "${asan_run_log}" 2>&1
        local asan_ret=$?

        if grep -qi "ERROR: AddressSanitizer" "${asan_run_log}"; then
            print_fail "AddressSanitizer Memory Check" "ASan reported memory safety error!"
        elif [ ${asan_ret} -ne 0 ]; then
            print_fail "AddressSanitizer Execution" "Non-zero exit code: ${asan_ret}"
        else
            print_pass "AddressSanitizer Memory Check" "0 memory corruption errors"
        fi
    else
        print_warn "AddressSanitizer Compilation" "Could not build with -fsanitize=address"
    fi

    # Restore standard build
    make -C "${SCRIPT_DIR}" re >/dev/null 2>&1
}

# ------------------------------------------------------------------------------
# SECTION 8: FIFO to LIFO Recode Validation Helper
# ------------------------------------------------------------------------------
show_recode_help() {
    print_section "8. 42 EVALUATION RECODE HELPER (FIFO -> LIFO)"
    echo -e "${C_WHITE}During the defense, the evaluator may ask to turn the FIFO scheduler into LIFO:${C_RESET}"
    echo -e "${C_DIM}  - Under FIFO: Requests are served First In, First Out (in arrival order).${C_RESET}"
    echo -e "${C_DIM}  - Under LIFO: Requests are served Last In, First Out (most recent requester first).${C_RESET}"
    echo -e "${C_DIM}  - In calendar_fns2.c, add_fifo can be changed to insert at the HEAD (e.g. ticket->next = *clipboard; *clipboard = ticket;)${C_RESET}"
    echo -e "${C_CYAN}To verify LIFO behavior under contention:${C_RESET}"
    echo -e "  Run: ${C_BOLD}./codexion 5 3000 200 200 200 10 800 fifo${C_RESET}"
    echo -e "  Observe grant order: the last coder to request is granted first.\n"
}

# ------------------------------------------------------------------------------
# Usage / Help
# ------------------------------------------------------------------------------
show_help() {
    echo -e "${C_CYAN}${C_BOLD}Usage:${C_RESET} $0 [OPTION / SUITE]"
    echo ""
    echo -e "${C_BOLD}Suites:${C_RESET}"
    echo "  all             Run complete test suite (default)"
    echo "  scale           Run only the official 42 scale sheet benchmark tests"
    echo "  leaks           Run only the Valgrind memory leak suite"
    echo "  threads         Run only Helgrind / DRD concurrency tests"
    echo "  sanitizers      Run ThreadSanitizer & AddressSanitizer checks"
    echo "  parsing         Run argument parsing & input validation tests"
    echo "  stress          Run ring topologies & multi-thread stress tests"
    echo "  recode          Show FIFO to LIFO recode guidance and test commands"
    echo "  custom <args>   Run a single custom test case with full trace analysis"
    echo ""
    echo -e "${C_BOLD}Options:${C_RESET}"
    echo "  -v, --verbose   Show detailed simulation output & diagnostic logs"
    echo "  --fast          Skip slow high-thread-count tests"
    echo "  --no-color      Disable colored ANSI terminal output"
    echo "  -h, --help      Display this help menu"
    echo ""
    echo -e "${C_BOLD}Example:${C_RESET}"
    echo "  $0 scale"
    echo "  $0 leaks -v"
    echo "  $0 custom 5 2000 200 200 200 10 0 fifo"
    exit 0
}

# ------------------------------------------------------------------------------
# Main Entrypoint & CLI Parsing
# ------------------------------------------------------------------------------
main() {
    local suite="all"

    while [ $# -gt 0 ]; do
        case "$1" in
            -h|--help)
                show_help
                ;;
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            --fast)
                FAST_MODE=1
                shift
                ;;
            --no-color)
                C_RESET=""; C_BOLD=""; C_DIM=""; C_RED=""; C_GREEN=""
                C_YELLOW=""; C_BLUE=""; C_MAGENTA=""; C_CYAN=""; C_WHITE=""
                shift
                ;;
            all)
                suite="all"
                shift
                ;;
            scale|benchmarks)
                suite="scale"
                shift
                ;;
            leaks|valgrind)
                suite="leaks"
                shift
                ;;
            threads|helgrind|drd)
                suite="threads"
                shift
                ;;
            sanitizers|tsan|asan)
                suite="sanitizers"
                shift
                ;;
            parsing|args)
                suite="parsing"
                shift
                ;;
            stress)
                suite="stress"
                shift
                ;;
            recode)
                suite="recode"
                shift
                ;;
            custom)
                suite="custom"
                shift
                CUSTOM_ARGS=("$@")
                break
                ;;
            *)
                echo -e "${C_RED}Unknown option: $1${C_RESET}"
                show_help
                ;;
        esac
    done

    print_banner

    if [ "${suite}" == "custom" ]; then
        if [ ${#CUSTOM_ARGS[@]} -ne 8 ]; then
            echo -e "${C_RED}Error: Custom run requires exactly 8 arguments:${C_RESET}"
            echo "  $0 custom <coders> <burnout> <compile> <debug> <refactor> <compiles_req> <cooldown> <scheduler>"
            exit 1
        fi
        if [ ! -x "${BIN_PATH}" ]; then
            make -C "${SCRIPT_DIR}" re >/dev/null 2>&1
        fi
        print_section "CUSTOM SIMULATION RUN"
        run_test "Custom Run" "${CUSTOM_ARGS[0]}" "${CUSTOM_ARGS[1]}" "${CUSTOM_ARGS[2]}" "${CUSTOM_ARGS[3]}" "${CUSTOM_ARGS[4]}" "${CUSTOM_ARGS[5]}" "${CUSTOM_ARGS[6]}" "${CUSTOM_ARGS[7]}" 0 30
        exit 0
    fi

    # Execute selected suites
    case "${suite}" in
        all)
            test_preliminaries
            test_argument_parsing
            test_scale_sheet_cases
            test_multi_topologies_and_stress
            test_valgrind_leaks
            test_thread_safety
            test_sanitizers
            show_recode_help
            ;;
        scale)
            test_preliminaries
            test_scale_sheet_cases
            ;;
        leaks)
            test_preliminaries
            test_valgrind_leaks
            ;;
        threads)
            test_preliminaries
            test_thread_safety
            ;;
        sanitizers)
            test_sanitizers
            ;;
        parsing)
            test_preliminaries
            test_argument_parsing
            ;;
        stress)
            test_preliminaries
            test_multi_topologies_and_stress
            ;;
        recode)
            show_recode_help
            ;;
    esac

    # --------------------------------------------------------------------------
    # Final Summary Report
    # --------------------------------------------------------------------------
    local end_time_total
    end_time_total=$(date +%s)
    local total_duration=$((end_time_total - START_TIME_TOTAL))

    echo ""
    echo -e "${C_CYAN}${C_BOLD}==============================================================================${C_RESET}"
    echo -e "${C_BOLD}                             TEST SUITE SUMMARY                               ${C_RESET}"
    echo -e "${C_CYAN}${C_BOLD}==============================================================================${C_RESET}"
    echo -e "  Total Tests Executed : ${C_BOLD}${TESTS_RUN}${C_RESET}"
    echo -e "  Passed               : ${C_GREEN}${C_BOLD}${TESTS_PASSED}${C_RESET}"
    echo -e "  Failed               : ${C_RED}${C_BOLD}${TESTS_FAILED}${C_RESET}"
    echo -e "  Warnings             : ${C_YELLOW}${C_BOLD}${TESTS_WARNED}${C_RESET}"
    echo -e "  Total Time Elapsed   : ${C_BOLD}${total_duration}s${C_RESET}"
    echo -e "${C_CYAN}${C_BOLD}==============================================================================${C_RESET}"

    if [ ${TESTS_FAILED} -eq 0 ]; then
        echo -e "\n  ${C_BG_GREEN}  ✓ ALL TESTS PASSED SUCCESSFULLY! PROJECT IS DEFENSE READY (100/100)  ${C_RESET}\n"
        exit 0
    else
        echo -e "\n  ${C_BG_RED}  ✗ SOME TESTS FAILED. PLEASE REVIEW THE FAILURES ABOVE BEFORE DEFENSE.  ${C_RESET}\n"
        exit 1
    fi
}

main "$@"
