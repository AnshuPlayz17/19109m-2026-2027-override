#!/usr/bin/env bash
#
# Type-checks the robot code without a VEX brain or an ARM toolchain.
#
# A shallow clone of the PROS kernel supplies the API headers, and a host g++
# then parses our sources against them. This catches the mistakes that actually
# bite -- a renamed method, a wrong constructor argument -- long before an
# upload.
#
# It does NOT produce a binary. Use 'pros make' for that.
#
# Usage:  ./scripts/syntax-check.sh
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
headers="$(mktemp -d)"
trap 'rm -rf "$headers"' EXIT

echo "==> Fetching PROS headers"
git clone --quiet --depth 1 https://github.com/purduesigbots/pros.git "$headers/pros"

echo "==> Type-checking"
status=0
while IFS= read -r source; do
    printf '    %s ... ' "${source#"$repo_root"/}"
    if g++ -fsyntax-only -std=gnu++20 \
        -I "$repo_root/include" \
        -I "$headers/pros/include" \
        -Wno-deprecated-declarations \
        "$source" 2>"$headers/err"; then
        echo "ok"
    else
        echo "FAILED"
        cat "$headers/err" >&2
        status=1
    fi
done < <(find "$repo_root/src" -name '*.cpp' | sort)

exit $status
