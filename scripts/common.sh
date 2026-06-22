#!/usr/bin/env bash
# Shared variables and helpers for the project scripts.
# Source this file (do not execute it directly) from the other scripts.

# Resolve paths relative to this file so they hold regardless of the caller.
common_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${common_dir}/.." && pwd)"

CHASTE_BUILD_DIR="${CHASTE_BUILD_DIR:-${PROJECT_ROOT}/build}"

CHASTE_SOURCE_DIR="${CHASTE_SOURCE_DIR:-${PROJECT_ROOT}/../Chaste}"
CHASTE_PROJECTS_DIR="${CHASTE_SOURCE_DIR}/projects"

CHASTE_TEST_OUTPUT="${CHASTE_TEST_OUTPUT:-${PROJECT_ROOT}/output}"
export CHASTE_TEST_OUTPUT="${CHASTE_TEST_OUTPUT}"

# The name of this project is the name of the project directory.
PROJECT_NAME="$(basename "${PROJECT_ROOT}")"

# Set the number of parallel jobs for building and testing.
NCORES="${NCORES:-4}"
if ! [[ "${NCORES}" =~ ^[0-9]+$ ]] || [[ "${NCORES}" -lt 1 ]]; then
	echo "Error: NCORES must be a positive integer (got '${NCORES}')." >&2
	exit 1
fi

# Abort with an error if the given command is not on PATH.
require_command() {
	if ! command -v "$1" >/dev/null 2>&1; then
		echo "Error: $1 is not available on PATH." >&2
		exit 1
	fi
}

# Abort unless the build has been configured.
require_configured() {
	if [[ ! -f "${CHASTE_BUILD_DIR}/CMakeCache.txt" ]]; then
		echo "Error: build is not configured at '${CHASTE_BUILD_DIR}'. Run configure.sh first." >&2
		exit 1
	fi
}

# Recursively remove a directory, refusing unsafe targets (an empty path, the
# filesystem root, or the project root) to guard against catastrophic deletes.
safe_remove_dir() {
	local dir="$1"
	if [[ -z "${dir}" || "${dir}" == "/" || "${dir}" == "${PROJECT_ROOT}" ]]; then
		echo "Error: refusing to remove unsafe path '${dir}'." >&2
		exit 1
	fi
	rm -rf "${dir}"
}
