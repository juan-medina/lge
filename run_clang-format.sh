#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT
set -euo pipefail

# Ensure we are run from project root ---
if [ ! -d "src" ]; then
    echo "Error: run this script from the project root (where src/ exists)."
    exit 1
fi

# run clang-format in-place for all source files
find "src" -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \) -exec clang-format -i {} \;

echo "Formatted all source files in src/ with clang-format."
