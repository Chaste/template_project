#!/usr/bin/env bash
set -euo pipefail

# Install chaste-sbml and the Chaste SBML C++ base classes.
#
# Usage: install.sh
#
# This script is run by setup_project.py to install the SBML dependencies.
# Re-run it to refresh chaste-sbml or the base classes.

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import the project environment and helpers.
_here="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
source "${_here}/scripts/env.sh"

# The chaste-sbml release to install.
# Override with: CHASTE_SBML_VERSION=develop sbml/install.sh
CHASTE_SBML_VERSION="${CHASTE_SBML_VERSION:-0.0.1}"

require_command python3

# The SBML generator formats its output with clang-format; warn if absent.
if ! command -v clang-format >/dev/null 2>&1; then
	echo "Warning: clang-format is not on PATH; chaste-sbml needs it to format generated code." >&2
fi

# Create the project virtualenv if it does not already exist.
# --system-site-packages lets it see native packages provided by the system
# Python (petsc4py, mpi4py and vtk), which are not pip-installable here. The
# virtualenv is shared with Python bindings if enabled.
if [[ ! -d "${VENV_DIR}" ]]; then
	python3 -m venv --system-site-packages "${VENV_DIR}"
fi

# Install chaste-sbml from GitHub.
"${VENV_DIR}/bin/pip" install --upgrade pip
"${VENV_DIR}/bin/pip" install "git+https://github.com/Chaste/chaste-sbml@${CHASTE_SBML_VERSION}"

# Copy the C++ base classes the generated code depends on into the project's src/, so they
# always match the installed version of chaste-sbml.
"${VENV_DIR}/bin/chaste-sbml" --copy-base-classes --output-dir "${PROJECT_ROOT}/src"

echo ""
echo "Installed chaste-sbml into '${VENV_DIR}'."
echo "Copied the SBML base classes into '${PROJECT_ROOT}/src'."
echo "Activate the virtualenv with: source '${VENV_DIR}/bin/activate'"
echo "Then convert an SBML model with: chaste-sbml --help"
