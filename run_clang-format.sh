#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT
set -euo pipefail

dirs=("src" "include" "examples" "tests")

for dir in "${dirs[@]}"; do
    if [ ! -d "$dir" ]; then
        echo "Error: directory '$dir' not found. Run this script from the project root."
        exit 1
    fi
done

if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found."
    exit 1
fi

find "${dirs[@]}" \
    -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \) \
    -exec clang-format -i {} +

echo "Formatted all source files with clang-format."