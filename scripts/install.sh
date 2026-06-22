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

# Create the virtualenv if it does not already exist.
venv_dir="${PROJECT_ROOT}/.virtualenv"
python3 -m venv "${venv_dir}"

# Install pychaste first (it is a dependency of the project bindings).
"${venv_dir}/bin/pip" install "${pychaste_pkg}"

# Install the project Python bindings.
"${venv_dir}/bin/pip" install "${project_pkg}"

echo "Installed Python bindings to '${venv_dir}'."
echo "Activate with: source '${venv_dir}/bin/activate'"
