#!/usr/bin/env bash
set -euo pipefail

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import common variables and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/common.sh"

# Abort if Python bindings are not set up for this project.
if [[ ! -f "${PROJECT_ROOT}/dynamic/config.yaml" ]]; then
	echo "Error: Python bindings are not set up for this project." >&2
	echo "Run setup_project.py with Python bindings enabled." >&2
	exit 1
fi

require_command python3
require_configured

# Check that the project's Python bindings have been compiled.
project_pkg="${CHASTE_BUILD_DIR}/projects/${PROJECT_NAME}/dynamic/package"
if [[ ! -d "${project_pkg}" ]]; then
	echo "Error: Python bindings package not found at '${project_pkg}'." >&2
	echo "Run compile.sh first." >&2
	exit 1
fi

# Check that pychaste has been compiled.
pychaste_pkg="${CHASTE_BUILD_DIR}/pychaste/package"
if [[ ! -d "${pychaste_pkg}" ]]; then
	echo "Error: pychaste package not found at '${pychaste_pkg}'." >&2
	echo "Run compile.sh first." >&2
	exit 1
fi

# Create the virtualenv (with --system-site-packages) if it does not already exist, so it
# can see PyChaste's native runtime dependencies (petsc4py, mpi4py, vtk) from the system
# Python. These are not pip-installable here.
"${common_dir}/create_venv.sh"

# Install pychaste first (it is a dependency of the project bindings).
"${VENV_DIR}/bin/pip" install "${pychaste_pkg}"

# Install the project Python bindings.
"${VENV_DIR}/bin/pip" install "${project_pkg}"

echo "Installed Python bindings to '${VENV_DIR}'."
echo "Activate with: source '${VENV_DIR}/bin/activate'"
