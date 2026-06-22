#!/usr/bin/env bash
set -euo pipefail

usage() {
	echo "Usage: $(basename "$0")" >&2
}

# Abort if number of arguments is incorrect.
if [[ $# -ne 0 ]]; then
	usage
	exit 1
fi

# Import common variables and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/common.sh"

# Restore the template to its original state (reverse setup).
require_command python3
python3 "${PROJECT_ROOT}/setup_project.py" --reset

# Remove this project's symlink under Chaste/projects/ 
# (only if it is a symlink, not a real directory).
project_link="${CHASTE_PROJECTS_DIR}/${PROJECT_NAME}"
if [[ -L "${project_link}" && "${project_link}" -ef "${PROJECT_ROOT}" ]]; then
	safe_rm "${project_link}"
	echo "Removed Chaste registration symlink '${project_link}'."
fi

# Remove generated build/output directories. These may resolve outside the repo
# (via CHASTE_BUILD_DIR/CHASTE_TEST_OUTPUT), so handle them explicitly.
for dir in "${CHASTE_BUILD_DIR}" "${CHASTE_TEST_OUTPUT}"; do
	safe_rm "${dir}"
done

echo "Reset project template to its original state."
