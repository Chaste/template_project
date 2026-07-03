#!/usr/bin/env bash
set -euo pipefail

# Create the project virtualenv if it does not already exist.
#
# Usage: create_venv.sh
#
# Creates virtual environment with --system-site-packages so it can see native
# packages provided by the system Python (e.g. petsc4py, mpi4py and vtk)

if [[ $# -ne 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import common variables and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/common.sh"

require_command python3

if [[ ! -d "${VENV_DIR}" ]]; then
	python3 -m venv --system-site-packages "${VENV_DIR}"
fi
