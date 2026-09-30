#!/usr/bin/env bash
set -euo pipefail

# Run this project's tests.
#
# Usage: test.sh
#
# Run after compile.sh. Runs the tests labelled for this project with NCORES
# parallel cores. Simulation output is written to ${CHASTE_TEST_OUTPUT}.

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import the project environment and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/env.sh"

# Check that ctest is available
require_command ctest

# Check that the build directory has been configured
require_configured

# Make sure the test output directory exists
mkdir -p "${CHASTE_TEST_OUTPUT}"
echo "CHASTE_TEST_OUTPUT=${CHASTE_TEST_OUTPUT}"

# Run tests
echo "NCORES=${NCORES}"
cd "${CHASTE_BUILD_DIR}"
ctest -j"${NCORES}" -V -L "project_${PROJECT_NAME}$"
