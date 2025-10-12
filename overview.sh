#!/usr/bin/env bash
# ===============================================
# Project Code Statistics Overview (Colorized)
# ===============================================

set -e

# Directories to scan
DIRS=("origo" "game")

# ANSI color codes
BOLD="\033[1m"
RESET="\033[0m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
CYAN="\033[36m"
WHITE="\033[97m"

# Helper: count matching lines
count_lines() {
    local pattern="$1"
    find "${DIRS[@]}" -type f \( -name "*.cpp" -o -name "*.h" \) \
        -exec grep -E "$pattern" {} + | wc -l
}

# Count files
CPP_FILES=$(find "${DIRS[@]}" -type f -name "*.cpp" | wc -l)
H_FILES=$(find "${DIRS[@]}" -type f -name "*.h" | wc -l)
TOTAL_FILES=$((CPP_FILES + H_FILES))

# Count total lines
CPP_LINES=$(find "${DIRS[@]}" -type f -name "*.cpp" -exec cat {} + | wc -l)
H_LINES=$(find "${DIRS[@]}" -type f -name "*.h" -exec cat {} + | wc -l)
TOTAL_LINES=$((CPP_LINES + H_LINES))

# Count classes and structs
CLASS_COUNT=$(count_lines '^[[:space:]]*class[[:space:]]+')
STRUCT_COUNT=$(count_lines '^[[:space:]]*struct[[:space:]]+')

# Header
echo -e "${BOLD}${CYAN}==============================================="
echo -e "        C++ PROJECT CODE ANALYSIS REPORT"
echo -e "===============================================${RESET}"
echo -e "${BOLD}${WHITE} Scanned directories:${RESET} ${YELLOW}${DIRS[*]}${RESET}\n"

# Table header
printf "${BOLD} %-20s %10s${RESET}\n" "Metric" "Count"
printf "${BLUE} %-20s %10s${RESET}\n" "--------------------" "----------"

# Data rows
printf "${WHITE} %-20s ${YELLOW}%10d${RESET}\n" "Total files" "$TOTAL_FILES"
printf "${WHITE} %-20s ${YELLOW}%10d${RESET}\n" "Total lines" "$TOTAL_LINES"
printf "${WHITE} %-20s ${GREEN}%10d${RESET}\n" ".cpp files" "$CPP_FILES"
printf "${WHITE} %-20s ${GREEN}%10d${RESET}\n" "Lines in .cpp" "$CPP_LINES"
printf "${WHITE} %-20s ${GREEN}%10d${RESET}\n" ".h files" "$H_FILES"
printf "${WHITE} %-20s ${GREEN}%10d${RESET}\n" "Lines in .h" "$H_LINES"
printf "${WHITE} %-20s ${CYAN}%10d${RESET}\n" "Classes" "$CLASS_COUNT"
printf "${WHITE} %-20s ${CYAN}%10d${RESET}\n" "Structs" "$STRUCT_COUNT"

# Footer
echo -e "${BOLD}${CYAN}===============================================${RESET}"
