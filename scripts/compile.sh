#!/usr/bin/env bash
set -euo pipefail

# Build this project's library, apps and tests.
#
# Usage: compile.sh
#
# Run after configure.sh. Builds with NCORES parallel jobs.

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import the project environment and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/env.sh"

# Check that cmake is available.
require_command cmake

# Check that the build directory has been configured.
require_configured

# Build.
echo "NCORES=${NCORES}"
cd "${CHASTE_BUILD_DIR}"
cmake --build . --target "project_${PROJECT_NAME}" --parallel "${NCORES}"
