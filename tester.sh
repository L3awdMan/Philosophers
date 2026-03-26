#!/bin/bash
# ============================================================================
#  Philosophers Tester
# ============================================================================

PHILO="./philo"
PASS=0
FAIL=0
WARN=0

# --- Colors ---
RED="\033[1;31m"
GRN="\033[1;32m"
YLW="\033[1;33m"
CYN="\033[1;36m"
MAG="\033[1;35m"
BLD="\033[1m"
DIM="\033[2m"
RST="\033[0m"

ok() {
  ((PASS++))
  printf "  ${GRN}[PASS]${RST} %s\n" "$1"
}
ko() {
  ((FAIL++))
  printf "  ${RED}[FAIL]${RST} %s\n" "$1"
}
warn() {
  ((WARN++))
  printf "  ${YLW}[WARN]${RST} %s\n" "$1"
}
header() { printf "\n${CYN}━━━ %s ━━━${RST}\n" "$1"; }
desc() { printf "  ${DIM}%s${RST}\n" "$1"; }

print_results() {
  header "RESULTS"
  TOTAL=$((PASS + FAIL + WARN))
  printf "\n  ${GRN}PASS: $PASS${RST}  ${RED}FAIL: $FAIL${RST}  ${YLW}WARN: $WARN${RST}  TOTAL: $TOTAL\n"
  if [ "$FAIL" -eq 0 ]; then
    printf "\n  ${GRN}${BLD}VERDICT: ALL TESTS PASSED${RST}\n\n"
  else
    printf "\n  ${RED}${BLD}VERDICT: $FAIL TEST(S) FAILED${RST}\n\n"
  fi
}

# ============================================================================
#  TEST FUNCTIONS
# ============================================================================

run_compilation() {
  header "0. COMPILATION"
  desc "The project must compile with -Wall -Wextra -Werror and produce a binary."
  make re >/dev/null 2>&1
  if [ -f "$PHILO" ]; then
    ok "Binary compiled successfully"
  else
    ko "Compilation failed"
    return 1
  fi
}

run_globals() {
  header "1. GLOBAL VARIABLES"
  desc "No global variables should be used for shared resources among philosophers."
  desc "Only linker symbols (_edata, _end) are acceptable."
  GLOBALS=$(nm "$PHILO" 2>/dev/null | grep ' [BCDGS] ' | grep -v '__' | grep -v '_edata\|_end')
  if [ -z "$GLOBALS" ]; then
    ok "No global variables found"
  else
    ko "Global variables detected: $GLOBALS"
  fi
}

run_mandatory() {
  header "2. MANDATORY TESTS"
  desc "Tests from the official 42 evaluation sheet."

  printf "\n  ${BLD}Test 2.1:${RST} ./philo 1 800 200 200\n"
  desc "One philosopher, one fork. Cannot eat. Must die at ~800ms."
  OUTPUT=$(timeout 5 $PHILO 1 800 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
    if [ "$DEATH_TIME" -ge 800 ] && [ "$DEATH_TIME" -le 810 ]; then
      ok "Philosopher died at ${DEATH_TIME}ms (within 10ms of 800)"
    else
      warn "Philosopher died at ${DEATH_TIME}ms (expected ~800)"
    fi
    EAT_COUNT=$(echo "$OUTPUT" | grep -c "is eating")
    if [ "$EAT_COUNT" -eq 0 ]; then
      ok "Philosopher never ate"
    else
      ko "Philosopher ate $EAT_COUNT time(s) — should never eat with 1 fork"
    fi
  else
    ko "Philosopher did not die"
  fi

  printf "\n  ${BLD}Test 2.2:${RST} ./philo 5 800 200 200\n"
  desc "Five philosophers, nobody should die. Run for 10 seconds."
  OUTPUT=$(timeout 10 $PHILO 5 800 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died unexpectedly"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 10 ]; then
      ok "No death detected ($LINES lines of output)"
    else
      warn "No death but very little output ($LINES lines) — possible env issue"
    fi
  fi

  printf "\n  ${BLD}Test 2.3:${RST} ./philo 5 800 200 200 7\n"
  desc "Five philosophers, each must eat at least 7 times. Simulation must stop."
  OUTPUT=$(timeout 30 $PHILO 5 800 200 200 7 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 0 ]; then
      ok "Simulation completed without death ($LINES lines)"
    else
      ko "No output produced"
    fi
  fi

  printf "\n  ${BLD}Test 2.4:${RST} ./philo 4 410 200 200\n"
  desc "Four philosophers, tight timing. Nobody should die."
  OUTPUT=$(timeout 10 $PHILO 4 410 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 10 ]; then
      ok "No death detected ($LINES lines)"
    else
      warn "No death but little output ($LINES lines) — possible env issue"
    fi
  fi

  printf "\n  ${BLD}Test 2.5:${RST} ./philo 4 310 200 100\n"
  desc "Four philosophers, impossible timing. One must die."
  OUTPUT=$(timeout 5 $PHILO 4 310 200 100 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
    ok "Philosopher died at ${DEATH_TIME}ms"
    DEATH_COUNT=$(echo "$OUTPUT" | grep -c "died")
    if [ "$DEATH_COUNT" -eq 1 ]; then
      ok "Exactly one death message printed"
    else
      ko "Multiple death messages ($DEATH_COUNT)"
    fi
  else
    ko "No philosopher died — one should have"
  fi

  printf "\n  ${BLD}Test 2.6:${RST} ./philo 2 800 200 200\n"
  desc "Two philosophers. Death timing must be < 10ms precision."
  OUTPUT=$(timeout 10 $PHILO 2 800 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died unexpectedly"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 10 ]; then
      ok "No death detected ($LINES lines)"
    else
      warn "No death but little output ($LINES lines) — possible env issue"
    fi
  fi
}

run_timing() {
  header "3. DEATH TIMING PRECISION"
  desc "When a philosopher dies, it must be reported within 10ms of time_to_die."

  printf "\n  ${BLD}Test 3.1:${RST} ./philo 1 800 200 200\n"
  OUTPUT=$(timeout 5 $PHILO 1 800 200 200 2>&1)
  DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
  if [ -n "$DEATH_TIME" ]; then
    DELAY=$((DEATH_TIME - 800))
    if [ "$DELAY" -le 10 ]; then
      ok "Death delay: ${DELAY}ms (acceptable, <= 10ms)"
    else
      ko "Death delay: ${DELAY}ms (unacceptable, > 10ms)"
    fi
  else
    ko "No death detected"
  fi

  printf "\n  ${BLD}Test 3.2:${RST} ./philo 4 310 200 100\n"
  OUTPUT=$(timeout 5 $PHILO 4 310 200 100 2>&1)
  DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
  if [ -n "$DEATH_TIME" ]; then
    if [ "$DEATH_TIME" -le 320 ]; then
      ok "Death at ${DEATH_TIME}ms (within 10ms of 310)"
    else
      ko "Death at ${DEATH_TIME}ms (too late, expected ~310)"
    fi
  else
    ko "No death detected"
  fi
}

run_edge_cases() {
  header "4. EDGE CASES & ERROR HANDLING"
  desc "Invalid inputs must print an error and exit without crashing."

  printf "\n  ${BLD}Test 4.1:${RST} ./philo  ${DIM}(no arguments)${RST}\n"
  desc "Must print usage error."
  OUT=$($PHILO 2>&1)
  RC=$?
  [ "$RC" -ne 0 ] && [ -n "$OUT" ] && ok "Error: exit $RC" || ko "No error on missing args"

  printf "\n  ${BLD}Test 4.2:${RST} ./philo 5  ${DIM}(too few arguments)${RST}\n"
  desc "Must reject — needs at least 4 arguments."
  OUT=$($PHILO 5 2>&1)
  RC=$?
  [ "$RC" -ne 0 ] && [ -n "$OUT" ] && ok "Error: exit $RC" || ko "No error on too few args"

  printf "\n  ${BLD}Test 4.3:${RST} ./philo -1 800 200 200  ${DIM}(negative number)${RST}\n"
  desc "Must reject negative values."
  OUT=$($PHILO -1 800 200 200 2>&1)
  RC=$?
  [ "$RC" -ne 0 ] && [ -n "$OUT" ] && ok "Error: exit $RC" || ko "Accepted negative number"

  printf "\n  ${BLD}Test 4.4:${RST} ./philo abc 800 200 200  ${DIM}(non-numeric)${RST}\n"
  desc "Must reject non-digit characters."
  OUT=$($PHILO abc 800 200 200 2>&1)
  RC=$?
  [ "$RC" -ne 0 ] && [ -n "$OUT" ] && ok "Error: exit $RC" || ko "Accepted non-numeric input"

  printf "\n  ${BLD}Test 4.5:${RST} ./philo 99999999999 800 200 200  ${DIM}(integer overflow)${RST}\n"
  desc "Must detect values exceeding INT_MAX."
  OUT=$($PHILO 99999999999 800 200 200 2>&1)
  RC=$?
  [ "$RC" -ne 0 ] && [ -n "$OUT" ] && ok "Error: exit $RC" || ko "Accepted overflow value"

  printf "\n  ${BLD}Test 4.6:${RST} ./philo 0 800 200 200  ${DIM}(zero philosophers)${RST}\n"
  desc "Must reject — need at least 1 philosopher."
  OUT=$($PHILO 0 800 200 200 2>&1)
  RC=$?
  [ "$RC" -ne 0 ] && [ -n "$OUT" ] && ok "Error: exit $RC" || ko "Accepted 0 philosophers"

  printf "\n  ${BLD}Test 4.7:${RST} ./philo 5 800 200 200 0  ${DIM}(zero meals)${RST}\n"
  desc "Should stop immediately or handle gracefully (no crash)."
  OUT=$(timeout 3 $PHILO 5 800 200 200 0 2>&1)
  RC=$?
  if [ "$RC" -eq 0 ] || [ "$RC" -eq 124 ]; then
    ok "No crash (exit $RC)"
  else
    ko "Crashed or unexpected exit code ($RC)"
  fi
}

run_stress() {
  header "5. STRESS TESTS"
  desc "High philosopher counts and tight timings."

  printf "\n  ${BLD}Test 5.1:${RST} ./philo 200 800 200 200  ${DIM}(max philosophers)${RST}\n"
  desc "200 philosophers — must not crash or deadlock. No death expected."
  OUTPUT=$(timeout 15 $PHILO 200 800 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died with 200 philosophers"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 10 ]; then
      ok "No death, $LINES lines of output"
    else
      warn "No death but $LINES lines — possible env issue"
    fi
  fi

  printf "\n  ${BLD}Test 5.2:${RST} ./philo 4 410 200 200 10  ${DIM}(meal limit + tight timing)${RST}\n"
  desc "Must stop cleanly after each philosopher eats 10 times. No death."
  OUTPUT=$(timeout 30 $PHILO 4 410 200 200 10 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 0 ]; then
      ok "Completed without death ($LINES lines)"
    else
      warn "No output produced"
    fi
  fi

  printf "\n  ${BLD}Test 5.3:${RST} ./philo 3 610 200 200  ${DIM}(odd count, tight timing)${RST}\n"
  desc "Three philosophers with tight timing. No death expected."
  OUTPUT=$(timeout 10 $PHILO 3 610 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 10 ]; then
      ok "No death, $LINES lines"
    else
      warn "No death but $LINES lines — possible env issue"
    fi
  fi
}

run_helgrind() {
  header "6. DATA RACE CHECK (Helgrind)"
  desc "Any data race = instant FAIL on 42 evaluation."
  desc "Uses valgrind --tool=helgrind to detect thread errors."

  if ! command -v valgrind &>/dev/null; then
    warn "Valgrind not installed — skipping helgrind tests"
    return
  fi

  local tests=("5 800 200 200 3" "1 800 200 200" "4 310 200 100")
  local i=1
  for t in "${tests[@]}"; do
    printf "\n  ${BLD}Test 6.%d:${RST} valgrind --tool=helgrind ./philo %s\n" "$i" "$t"
    HOUT=$(timeout 15 valgrind --tool=helgrind $PHILO $t 2>&1)
    ERRORS=$(echo "$HOUT" | grep "ERROR SUMMARY" | awk '{print $4}')
    if [ "$ERRORS" = "0" ]; then
      ok "0 errors (helgrind clean)"
    else
      ko "$ERRORS error(s) detected — DATA RACE"
      echo "$HOUT" | grep -A2 "Possible data race" | head -20
    fi
    ((i++))
  done
}

run_leaks() {
  header "7. MEMORY LEAK CHECK (Valgrind)"
  desc "All heap memory must be freed before exit. Any leak = flag on evaluation."

  if ! command -v valgrind &>/dev/null; then
    warn "Valgrind not installed — skipping leak tests"
    return
  fi

  local tests=("5 800 200 200 3" "1 800 200 200" "4 310 200 100")
  local i=1
  for t in "${tests[@]}"; do
    printf "\n  ${BLD}Test 7.%d:${RST} valgrind --leak-check=full ./philo %s\n" "$i" "$t"
    VOUT=$(timeout 15 valgrind --leak-check=full --show-leak-kinds=all $PHILO $t 2>&1)
    LEAKS=$(echo "$VOUT" | grep "in use at exit" | awk '{print $6}')
    ERRORS=$(echo "$VOUT" | grep "ERROR SUMMARY" | awk '{print $4}')
    if [ "$LEAKS" = "0" ] && [ "$ERRORS" = "0" ]; then
      ok "0 bytes leaked, 0 errors"
    else
      ko "Leaks: ${LEAKS} bytes in use at exit, ${ERRORS} error(s)"
      echo "$VOUT" | grep -A5 "LEAK SUMMARY"
    fi
    ((i++))
  done
}

run_extra() {
  header "8. EXTRA TESTS"
  desc "Creative, extreme, and edge-case scenarios."

  # --- 8.1: Absurd time_to_sleep (500M ms) — must die ---
  printf "\n  ${BLD}Test 8.1:${RST} ./philo 8 800 1000 500000000\n"
  desc "time_to_sleep is 500M ms. Philosophers starve during sleep."
  OUTPUT=$(timeout 5 $PHILO 8 800 1000 500000000 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
    ok "Philosopher died at ${DEATH_TIME}ms (starved during absurd sleep)"
  else
    ko "No philosopher died — should starve during 500M ms sleep"
  fi

  # --- 8.2: Absurd time_to_sleep (2M ms) — must die ---
  printf "\n  ${BLD}Test 8.2:${RST} ./philo 5 800 200 2000000\n"
  desc "time_to_sleep is 2M ms. Philosophers will starve during sleep."
  OUTPUT=$(timeout 5 $PHILO 5 800 200 2000000 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
    ok "Philosopher died at ${DEATH_TIME}ms (starved during 2M ms sleep)"
  else
    ko "No philosopher died — should starve during 2M ms sleep"
  fi

  # --- 8.3: Absurd time_to_eat (2M ms) — should NOT die ---
  printf "\n  ${BLD}Test 8.3:${RST} ./philo 5 800 2000000 200\n"
  desc "time_to_eat is 2M ms. Eating resets last_meal — should NOT die."
  OUTPUT=$(timeout 5 $PHILO 5 800 2000000 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died — eating should reset last_meal_time"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 0 ]; then
      ok "No death ($LINES lines) — eating keeps philosophers alive"
    else
      warn "No output produced"
    fi
  fi

  # --- 8.4: Edge timing — time_to_die == time_to_eat + time_to_sleep ---
  printf "\n  ${BLD}Test 8.4:${RST} ./philo 4 400 200 200\n"
  desc "time_to_die == time_to_eat + time_to_sleep (exact edge). Death depends on scheduling."
  OUTPUT=$(timeout 10 $PHILO 4 400 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
    warn "Philosopher died at ${DEATH_TIME}ms (edge timing — scheduling dependent)"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    ok "No death ($LINES lines) — survived edge timing"
  fi

  # --- 8.5: time_to_eat > time_to_die — must die ---
  printf "\n  ${BLD}Test 8.5:${RST} ./philo 5 200 400 200\n"
  desc "time_to_eat (400) > time_to_die (200). Philosophers die while waiting for forks."
  OUTPUT=$(timeout 5 $PHILO 5 200 400 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
    ok "Philosopher died at ${DEATH_TIME}ms (cannot survive when eat > die)"
    DEATH_COUNT=$(echo "$OUTPUT" | grep -c "died")
    if [ "$DEATH_COUNT" -eq 1 ]; then
      ok "Exactly one death message printed"
    else
      ko "Multiple death messages ($DEATH_COUNT)"
    fi
  else
    ko "No philosopher died — should die when time_to_eat > time_to_die"
  fi

  # --- 8.6: Large philosopher count with tight timing ---
  printf "\n  ${BLD}Test 8.6:${RST} ./philo 150 410 200 200\n"
  desc "150 philosophers with tight timing. Must not crash, deadlock, or die."
  OUTPUT=$(timeout 15 $PHILO 150 410 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died with 150 philosophers (tight timing)"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 10 ]; then
      ok "No death, $LINES lines of output with 150 philos"
    else
      warn "No death but $LINES lines — possible env issue"
    fi
  fi

  # --- 8.7: Very large meal count ---
  printf "\n  ${BLD}Test 8.7:${RST} ./philo 4 800 200 200 100\n"
  desc "4 philosophers, each must eat 100 times. Simulation must stop cleanly."
  OUTPUT=$(timeout 120 $PHILO 4 800 200 200 100 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died before eating 100 times"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 100 ]; then
      ok "Completed without death ($LINES lines, 100 meals each)"
    else
      warn "Completed but only $LINES lines — possibly not all meals eaten"
    fi
  fi

  # --- 8.8: Single philosopher with generous timing ---
  printf "\n  ${BLD}Test 8.8:${RST} ./philo 1 5000 200 200\n"
  desc "One philosopher, generous time_to_die. Must still die (only 1 fork)."
  OUTPUT=$(timeout 10 $PHILO 1 5000 200 200 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
    if [ "$DEATH_TIME" -ge 5000 ] && [ "$DEATH_TIME" -le 5010 ]; then
      ok "Philosopher died at ${DEATH_TIME}ms (within 10ms of 5000)"
    else
      warn "Philosopher died at ${DEATH_TIME}ms (expected ~5000)"
    fi
  else
    ko "Philosopher did not die — must die with 1 fork"
  fi

  # --- 8.9: Rapid succession runs (resource cleanup) ---
  printf "\n  ${BLD}Test 8.9:${RST} 5x rapid ./philo 5 800 200 200 3\n"
  desc "Run 5 short simulations back-to-back to check for resource cleanup."
  CLEANUP_OK=true
  for RUN in 1 2 3 4 5; do
    OUTPUT=$(timeout 10 $PHILO 5 800 200 200 3 2>&1)
    RC=$?
    if echo "$OUTPUT" | grep -q "died"; then
      CLEANUP_OK=false
      ko "Run $RUN: A philosopher died"
      break
    fi
    if [ "$RC" -eq 124 ]; then
      CLEANUP_OK=false
      ko "Run $RUN: Timed out — possible deadlock or resource leak"
      break
    fi
  done
  if [ "$CLEANUP_OK" = true ]; then
    ok "All 5 rapid runs completed cleanly — no resource leaks"
  fi

  # --- 8.10: Output format validation ---
  printf "\n  ${BLD}Test 8.10:${RST} Output format check on ./philo 5 800 200 200 5\n"
  desc "Every line must match: [timestamp] [philo_id] [action]"
  OUTPUT=$(timeout 15 $PHILO 5 800 200 200 5 2>&1)
  BAD_LINES=$(echo "$OUTPUT" | grep -cvE '^[0-9]+ [0-9]+ (has taken a fork|is eating|is sleeping|is thinking|died)$')
  TOTAL_LINES=$(echo "$OUTPUT" | wc -l)
  if [ "$BAD_LINES" -eq 0 ]; then
    ok "All $TOTAL_LINES lines match expected format"
  else
    ko "$BAD_LINES out of $TOTAL_LINES lines have bad format"
    echo "$OUTPUT" | grep -vE '^[0-9]+ [0-9]+ (has taken a fork|is eating|is sleeping|is thinking|died)$' | head -5
  fi

  # --- 8.11: Two philosophers, tight meal count ---
  printf "\n  ${BLD}Test 8.11:${RST} ./philo 2 400 200 100 5\n"
  desc "Two philosophers, each must eat 5 times with tight timing. Must not die."
  OUTPUT=$(timeout 15 $PHILO 2 400 200 100 5 2>&1)
  if echo "$OUTPUT" | grep -q "died"; then
    ko "A philosopher died"
  else
    LINES=$(echo "$OUTPUT" | wc -l)
    if [ "$LINES" -gt 0 ]; then
      ok "Completed without death ($LINES lines)"
    else
      ko "No output produced"
    fi
  fi
}

# ============================================================================
#  MENU
# ============================================================================

show_menu() {
  clear
  printf "\n"
  printf "  ${MAG}${BLD}╔══════════════════════════════════════════════════════╗${RST}\n"
  printf "  ${MAG}${BLD}║                                                      ║${RST}\n"
  printf "  ${MAG}${BLD}║${RST}               ${BLD}PHILOSOPHERS EVO EVO TESTER${RST}            ${MAG}${BLD}║${RST}\n"
  printf "  ${MAG}${BLD}║                                                      ║${RST}\n"
  printf "  ${MAG}${BLD}╚══════════════════════════════════════════════════════╝${RST}\n"
  printf "\n"
  printf "  ${BLD}Select a test to run:${RST}\n\n"
  printf "  ${CYN}[0]${RST}  Compilation           ${DIM}— Build with -Wall -Wextra -Werror${RST}\n"
  printf "  ${CYN}[1]${RST}  Global Variables      ${DIM}— Check for forbidden globals${RST}\n"
  printf "  ${CYN}[2]${RST}  Mandatory Tests       ${DIM}— Official eval sheet tests${RST}\n"
  printf "  ${CYN}[3]${RST}  Death Timing          ${DIM}— Precision check (< 10ms)${RST}\n"
  printf "  ${CYN}[4]${RST}  Edge Cases            ${DIM}— Invalid inputs & error handling${RST}\n"
  printf "  ${CYN}[5]${RST}  Stress Tests          ${DIM}— 200 philos, tight timings${RST}\n"
  printf "  ${CYN}[6]${RST}  Helgrind              ${DIM}— Data race detection${RST}\n"
  printf "  ${CYN}[7]${RST}  Valgrind Leaks        ${DIM}— Memory leak detection${RST}\n"
  printf "  ${CYN}[8]${RST}  Extra Tests           ${DIM}— Absurd timings, edge cases, format${RST}\n"
  printf "\n"
  printf "  ${GRN}[a]${RST}  Run ALL tests\n"
  printf "  ${RED}[q]${RST}  Quit\n"
  printf "\n"
}

run_test_by_id() {
  case "$1" in
  0) run_compilation ;;
  1) run_globals ;;
  2) run_mandatory ;;
  3) run_timing ;;
  4) run_edge_cases ;;
  5) run_stress ;;
  6) run_helgrind ;;
  7) run_leaks ;;
  8) run_extra ;;
  *) printf "  ${RED}Invalid option${RST}\n" ;;
  esac
}

run_all() {
  run_compilation || return 1
  run_globals
  run_mandatory
  run_timing
  run_edge_cases
  run_stress
  run_helgrind
  run_leaks
  run_extra
  print_results
}

# ============================================================================
#  MAIN LOOP
# ============================================================================

# If --all flag is passed, run everything non-interactively
if [ "$1" = "--all" ]; then
  run_all
  exit $FAIL
fi

while true; do
  show_menu
  printf "  ${BLD}> ${RST}"
  read -r choice

  case "$choice" in
  [0-8])
    PASS=0
    FAIL=0
    WARN=0
    run_test_by_id "$choice"
    print_results
    printf "  ${DIM}Press Enter to continue...${RST}"
    read -r
    ;;
  a | A)
    PASS=0
    FAIL=0
    WARN=0
    run_all
    printf "  ${DIM}Press Enter to continue...${RST}"
    read -r
    ;;
  q | Q)
    printf "\n  ${DIM}Goodbye.${RST}\n\n"
    exit 0
    ;;
  *)
    printf "  ${RED}Invalid option. Try again.${RST}\n"
    sleep 1
    ;;
  esac
done
