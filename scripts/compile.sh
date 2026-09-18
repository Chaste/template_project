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

# Check that cmake is available.
require_command cmake

# Check that the build directory has been configured.
require_configured

# Build.
cd "${CHASTE_BUILD_DIR}"
cmake --build . --target "project_${PROJECT_NAME}" --parallel "${NCORES}"
