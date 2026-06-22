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
if ! git -C "${repo_root}" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
	echo "Error: '${repo_root}' is not a git repository." >&2
	exit 1
fi

# Confirm before discarding local changes, unless forced.
if [[ "${force}" -ne 1 ]]; then
	echo "This will reset the template to its original (committed) state:"
	echo "  - all tracked files reset to HEAD (local changes discarded)"
	echo "  - '${CHASTE_BUILD_DIR}' and '${CHASTE_TEST_OUTPUT}' removed"
	echo "  - untracked files created by setup_project.py removed"
	echo "  - this project's Chaste registration symlink removed"
	echo "  ('${script_dir}' is preserved.)"
	reply=""
	read -r -p "Proceed? [y/N] " reply || true
	case "${reply}" in
		y | Y | yes | Yes) ;;
		*) echo "Aborted."; exit 0 ;;
	esac
fi

# Remove this project's registration symlink under Chaste/projects/ (only if it
# is a symlink pointing back here, never a real directory living there).
project_link="${CHASTE_PROJECTS_DIR}/${PROJECT_NAME}"
if [[ -L "${project_link}" && "${project_link}" -ef "${repo_root}" ]]; then
	rm -f "${project_link}"
	echo "Removed Chaste registration symlink '${project_link}'."
fi

# Remove generated build/output directories. These may resolve outside the repo
# (via CHASTE_BUILD_DIR/CHASTE_TEST_OUTPUT), so handle them explicitly and guard
# against deleting the repository root or the filesystem root.
for dir in "${CHASTE_BUILD_DIR}" "${CHASTE_TEST_OUTPUT}"; do
	if [[ -n "${dir}" && "${dir}" != "/" && "${dir}" != "${repo_root}" ]]; then
		rm -rf "${dir}"
	fi
done

# Restore tracked files to their committed state, then remove any remaining
# untracked/ignored files (e.g. files renamed by setup_project.py), keeping
# this scripts/ directory.
git -C "${repo_root}" reset --hard
git -C "${repo_root}" clean -fdx --exclude=/scripts

echo "Reset template '${PROJECT_NAME}' to its original state."
