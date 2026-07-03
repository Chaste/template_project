#!/usr/bin/env bash
set -euo pipefail

# Create the project virtualenv and install the SBML code generator into it.

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import common variables and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/common.sh"

require_command python3

# The SBML generator formats its output with clang-format; warn (non-fatal) if absent.
if ! command -v clang-format >/dev/null 2>&1; then
	echo "Warning: clang-format is not on PATH; chaste_codegen_sbml needs it to format generated code." >&2
fi

# Create the project virtualenv if it does not already exist (shared with the Python
# bindings, hence --system-site-packages inside create_venv.sh so it never hides PyChaste's
# native packages when a project has both SBML and Python bindings).
"${common_dir}/create_venv.sh"

# Install the SBML code generator from GitHub.
"${VENV_DIR}/bin/pip" install --upgrade pip
"${VENV_DIR}/bin/pip" install "git+https://github.com/Chaste/chaste-codegen-sbml@develop"

echo ""
echo "Installed chaste-codegen-sbml into '${VENV_DIR}'."
echo "Activate the virtualenv with: source '${VENV_DIR}/bin/activate'"
echo "Then convert an SBML model with: chaste_codegen_sbml --help"
