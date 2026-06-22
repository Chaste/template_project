#!/usr/bin/env bash
set -euo pipefail

usage() {
	echo "Usage: $(basename "$0") [-f|--force]" >&2
}

# Parse arguments.
force=0
if [[ $# -gt 1 ]]; then
	usage
	exit 1
fi
if [[ $# -eq 1 ]]; then
	case "$1" in
		-f | --force) force=1 ;;
		*) usage; exit 1 ;;
	esac
fi

# Import common variables and helpers.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
source "${script_dir}/common.sh"

# Check that git is available and that this is the project's git repository.
require_command git
if ! git -C "${PROJECT_ROOT}" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
	echo "Error: '${PROJECT_ROOT}' is not a git repository." >&2
	exit 1
fi

# Confirm before discarding local changes, unless forced.
if [[ "${force}" -ne 1 ]]; then
	echo "This will reset the template to its original (committed) state:"
	echo "All uncommitted changes will be removed, and untracked files will be deleted."
	echo "  ('${script_dir}' is preserved.)"
	reply=""
	read -r -p "Proceed? [Y/n] " reply || true
	case "${reply}" in
		y | Y | yes | Yes) ;;
		*) echo "Aborted."; exit 0 ;;
	esac
fi

# Remove this project's registration symlink under Chaste/projects/ (only if it
# is a symlink pointing back here, never a real directory living there).
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

# Restore tracked files to their committed state, then remove any remaining
# untracked/ignored files (e.g. files renamed by setup_project.py), keeping
# this scripts/ directory.
git -C "${PROJECT_ROOT}" reset --hard
git -C "${PROJECT_ROOT}" clean -fdx --exclude=/scripts

echo "Reset template '${PROJECT_NAME}' to its original state."
