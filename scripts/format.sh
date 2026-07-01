#!/bin/bash
# Format all C++ sources with clang-format using scripts/.clang-format.
set -e

command -v clang-format >/dev/null 2>&1 || { echo "clang-format is not installed. Aborting."; exit 1; }

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
STYLE="file:$SCRIPT_DIR/.clang-format"

for d in "$SCRIPT_DIR"/../src "$SCRIPT_DIR"/../include; do
	find "$d" -type f \( -iname "*.cpp" -o -iname "*.hpp" \) -print \
		-exec clang-format --style="$STYLE" -i {} \;
done

echo "Formatting complete."
