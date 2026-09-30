#!/usr/bin/env bash
set -euo pipefail

# Install this project's compiled Python bindings into the project virtualenv.
#
# Usage: install.sh
#
# Run after configure.sh and compile.sh.

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import the project environment and helpers.
_here="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
source "${_here}/scripts/env.sh"

# Abort if Python bindings are not set up for this project.
if [[ ! -f "${PROJECT_ROOT}/bindings/config.yaml" ]]; then
	echo "Error: Python bindings are not set up for this project." >&2
	echo "Run setup_project.py with Python bindings enabled." >&2
	exit 1
fi

require_command python3
require_configured

# Check that the project's Python bindings have been compiled.
project_pkg="${CHASTE_BUILD_DIR}/projects/${PROJECT_NAME}/bindings/package"
if [[ ! -d "${project_pkg}" ]]; then
	echo "Error: Python bindings package not found at '${project_pkg}'." >&2
	echo "Run compile.sh first." >&2
	exit 1
fi

# Check that PyChaste has been compiled.
pychaste_pkg="${CHASTE_BUILD_DIR}/pychaste/package"
if [[ ! -d "${pychaste_pkg}" ]]; then
	echo "Error: PyChaste package not found at '${pychaste_pkg}'." >&2
	echo "Run compile.sh first." >&2
	exit 1
fi

# Create the project virtualenv if it does not already exist.
# --system-site-packages lets it see native packages provided by the system
# Python (petsc4py and vtk), which are not pip-installable here. The
# virtualenv is shared with SBML if enabled.
if [[ ! -d "${VENV_DIR}" ]]; then
	python3 -m venv --system-site-packages "${VENV_DIR}"
fi

# Warn if PyChaste's runtime dependencies are not visible from the virtualenv.
missing=""
for module in petsc4py vtk; do
	"${VENV_DIR}/bin/python" -c "import ${module}" >/dev/null 2>&1 || missing="${missing} ${module}"
done
if [[ -n "${missing}" ]]; then
	echo "Warning: PyChaste runtime dependencies not found:${missing}." >&2
	echo "These are provided by the system Python (e.g. in the chaste/base image) and are" >&2
	echo "needed to import the bindings. Install them on your system before using them." >&2
fi

# Install PyChaste first (it is a dependency of the project bindings).
"${VENV_DIR}/bin/pip" install "${pychaste_pkg}"

# Install the project Python bindings.
"${VENV_DIR}/bin/pip" install "${project_pkg}"

echo "Installed Python bindings to '${VENV_DIR}'."
echo "Activate with: source '${VENV_DIR}/bin/activate'"
