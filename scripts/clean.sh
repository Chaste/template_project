#!/usr/bin/env bash
set -euo pipefail

usage() {
	echo "Usage: $(basename "$0")" >&2
}

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	usage
	exit 1
fi

# Import common variables and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/common.sh"

# Clean the build directory.
safe_remove_dir "${CHASTE_BUILD_DIR}"
mkdir -p "${CHASTE_BUILD_DIR}"

echo "Cleaned build in '${CHASTE_BUILD_DIR}'."
