#!/usr/bin/env bash
set -euo pipefail

# Configure the Chaste build for this project.
#
# Usage: configure.sh
#
# Symlinks the project into ${CHASTE_SOURCE_DIR}/projects, where Chaste looks for
# user projects, then runs cmake in ${CHASTE_BUILD_DIR}. With Python bindings
# enabled the cppwg wrappers are generated here too, so re-run this after editing
# bindings/config.yaml.

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import the project environment and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/env.sh"

# Check that cmake is available and the Chaste source exists.
require_command cmake
require_source

# Ensure this project is registered under Chaste/projects/.
register_project

# Create the build directory
mkdir -p "${CHASTE_BUILD_DIR}"

# Configure
cd "${CHASTE_BUILD_DIR}"
cmake "${CHASTE_SOURCE_DIR}" \
    -DChaste_ENABLE_PYCHASTE="${Chaste_ENABLE_PYCHASTE}" \
    -DChaste_UPDATE_PROVENANCE="${Chaste_UPDATE_PROVENANCE}"
