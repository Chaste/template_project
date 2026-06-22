#!/usr/bin/env bash
set -euo pipefail

# Abort if number of arguments is incorrect.
if [[ $# -gt 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import common variables and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/common.sh"

# Check that cmake is available and the Chaste source exists.
require_command cmake
require_source

# Ensure this project is registered under Chaste/projects/.
"${common_dir}/register.sh"

# Create the build directory
mkdir -p "${CHASTE_BUILD_DIR}"

# Configure.
cd "${CHASTE_BUILD_DIR}"
cmake "${CHASTE_SOURCE_DIR}"
