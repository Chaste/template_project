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

# Check that ctest is available
require_command ctest

# Check that the build directory has been configured
require_configured

# Make sure the test output directory exists
mkdir -p "${CHASTE_TEST_OUTPUT}"
echo "CHASTE_TEST_OUTPUT=${CHASTE_TEST_OUTPUT}"

# Run tests
cd "${CHASTE_BUILD_DIR}"
ctest -j"${NCORES}" -V -L "project_${PROJECT_NAME}$"
