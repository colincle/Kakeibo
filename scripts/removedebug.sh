#!/bin/bash

# Get absolute path to script directory, then move to project root (assumes script is in scripts/)
cd "$(dirname "$0")/.." || exit 1

# Find all .cpp and .h files and clean lines with //debug
find . -type f \( -name "*.cpp" -o -name "*.h" \) | while IFS= read -r file; do
	if [[ -s "$file" ]]; then
		if grep -q "//debug" "$file"; then
			echo "Processing: $file"
			grep "//debug" "$file" | sed 's/^/Removed: /'
			tmp_file=$(mktemp)
			sed '/\/\/debug/d' "$file" > "$tmp_file" && mv "$tmp_file" "$file"
		fi
	fi
done
