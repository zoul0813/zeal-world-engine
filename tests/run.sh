#!/bin/sh
# Host tests; optional ZGDK_PATH compiles its real tilemap implementation.
set -eu
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
test_dir=$(mktemp -d "${TMPDIR:-/tmp}/world-tests.XXXXXX")
trap 'rm -rf "$test_dir"' EXIT HUP INT TERM
set --
if [ -n "${ZGDK_PATH:-}" ]; then
    set -- -DTEST_USE_REAL_ZGDK "$ZGDK_PATH/src/tilemap.c"
fi
for suite in world minimap; do
    "${CC:-cc}" -std=c11 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
        -I"$project_dir/tests/sdk" -I"$project_dir/tests" -I"$project_dir/libsrc" \
        "$project_dir/tests/$suite.c" "$project_dir/tests/support.c" \
        "$project_dir/libsrc/world/world.c" "$project_dir/libsrc/world/transitions.c" "$project_dir/libsrc/world/slide.c" \
        "$project_dir/libsrc/minimap/state.c" "$project_dir/libsrc/minimap/render.c" "$project_dir/libsrc/minimap/minimap.c" \
        "$@" -o "$test_dir/$suite"
    "$test_dir/$suite"
done
