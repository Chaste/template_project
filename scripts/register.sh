#!/usr/bin/env bash
set -euo pipefail

# Register this project with Chaste.
# The project is only built if it appears under the Chaste/projects/ directory
# or a symlink there points back here.

# Abort if number of arguments is incorrect.
if [[ $# -gt 0 ]]; then
	echo "Usage: $(basename "$0")" >&2
	exit 1
fi

# Import common variables and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/common.sh"

# Check that the Chaste source directory exists.
require_source

# Register the project
project_link="${CHASTE_PROJECTS_DIR}/${PROJECT_NAME}"

mkdir -p "${CHASTE_PROJECTS_DIR}"

if [[ -L "${project_link}" && "${project_link}" -ef "${PROJECT_ROOT}" ]]; then
	: # Already registered: a symlink under Chaste/projects/ points back to this project.
elif [[ ! -L "${project_link}" && "${project_link}" -ef "${PROJECT_ROOT}" ]]; then
	: # Already registered: the project itself lives directly under Chaste/projects/.
elif [[ -L "${project_link}" ]]; then
	# Repoint a stale/dangling symlink.
	ln -sfn "${PROJECT_ROOT}" "${project_link}"
	echo "Re-registered project '${PROJECT_NAME}' under '${CHASTE_PROJECTS_DIR}'."
elif [[ -e "${project_link}" ]]; then
	# Another project already exists with this name.
	echo "Error: '${project_link}' already exists and is not this project." >&2
	echo "Remove or rename it, then re-run register.sh." >&2
	exit 1
else
	# Create a new symlink under Chaste/projects/.
	ln -s "${PROJECT_ROOT}" "${project_link}"
	echo "Registered project '${PROJECT_NAME}' under '${CHASTE_PROJECTS_DIR}'."
fi
