#!/usr/bin/env bash
set -euo pipefail

# Create the project virtualenv and install the SBML code generator into it.

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# The project root is the parent of this script's directory.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${script_dir}/.." && pwd)"

# Abort with an error if the given command is not on PATH.
require_command() {
	if ! command -v "$1" >/dev/null 2>&1; then
		echo "Error: $1 is not available on PATH." >&2
		exit 1
	fi
}

require_command python3

# The SBML generator formats its output with clang-format; warn (non-fatal) if absent.
if ! command -v clang-format >/dev/null 2>&1; then
	echo "Warning: clang-format is not on PATH; chaste_codegen_sbml needs it to format generated code." >&2
fi

# Create the project virtualenv (idempotent; shared with Python bindings if both are set up).
venv_dir="${PROJECT_ROOT}/.virtualenv"
python3 -m venv "${venv_dir}"

# Install the SBML code generator from GitHub.
"${venv_dir}/bin/pip" install --upgrade pip
"${venv_dir}/bin/pip" install "git+https://github.com/Chaste/chaste-codegen-sbml@develop"

echo ""
echo "Installed chaste-codegen-sbml into '${venv_dir}'."
echo "Activate the virtualenv with: source '${venv_dir}/bin/activate'"
echo "Then convert an SBML model with: chaste_codegen_sbml --help"
