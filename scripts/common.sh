#!/usr/bin/env bash
# Shared variables and helpers for the project scripts.
# Source this file (do not execute it directly) from the other scripts.

# Resolve paths relative to this file so they hold regardless of the caller.
common_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${common_dir}/.." && pwd)"

# If the Chaste source directory is not set, try to find it in a few common locations.
if [[ -z "${CHASTE_SOURCE_DIR:-}" ]]; then
	for _candidate in \
		"${PROJECT_ROOT}/../Chaste" \
		"${PROJECT_ROOT}/../../Chaste" \
		"${HOME}/Chaste" \
		"/home/chaste/src"
	do
		if [[ -f "${_candidate}/CMakeLists.txt" ]]; then
			CHASTE_SOURCE_DIR="$(cd -- "${_candidate}" && pwd)"
			break
		fi
	done
	if [[ -z "${CHASTE_SOURCE_DIR:-}" ]]; then
		echo "Error: could not find Chaste source directory." >&2
		echo "Set CHASTE_SOURCE_DIR to the location of your Chaste source." >&2
		exit 1
	fi
fi

CHASTE_PROJECTS_DIR="${CHASTE_SOURCE_DIR}/projects"

CHASTE_BUILD_DIR="${CHASTE_BUILD_DIR:-${PROJECT_ROOT}/build}"
if [[ "${CHASTE_BUILD_DIR}" == "${PROJECT_ROOT}" ]]; then
	echo "Error: CHASTE_BUILD_DIR must not be the project root '${PROJECT_ROOT}'." >&2
	echo "Set CHASTE_BUILD_DIR to a separate build directory." >&2
	exit 1
fi

if [[ -z "${CHASTE_TEST_OUTPUT:-}" ]]; then
	export CHASTE_TEST_OUTPUT="${PROJECT_ROOT}/output"
fi

# The name of this project is the name of the project directory.
PROJECT_NAME="$(basename "${PROJECT_ROOT}")"

if [[ -f "${PROJECT_ROOT}/dynamic/config.yaml" ]]; then
	# Enable pychaste if this project has Python bindings set up.
	Chaste_ENABLE_PYCHASTE=ON
	BUILD_PROJECT_PYTHON_BINDINGS=ON
else
	Chaste_ENABLE_PYCHASTE="${Chaste_ENABLE_PYCHASTE:-OFF}"
	BUILD_PROJECT_PYTHON_BINDINGS=OFF
fi

# Set the number of parallel jobs for building and testing.
if ! [[ "${NCORES:-}" =~ ^[1-9][0-9]*$ ]]; then
	NCORES="$(nproc)"
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

# Abort unless the Chaste source directory exists.
require_source() {
	if [[ ! -f "${CHASTE_SOURCE_DIR}/CMakeLists.txt" ]]; then
		echo "Error: '${CHASTE_SOURCE_DIR}' is not a Chaste source directory." >&2
		echo "Set CHASTE_SOURCE_DIR to the location of your Chaste source." >&2
		exit 1
	fi
}

# Remove a file or directory (recursively), refusing unsafe targets (an empty
# path, the filesystem root, or the project root) to guard against catastrophic
# deletes. For a symlink, only the link itself is removed, not its target.
safe_rm() {
	local path="$1"
	if [[ -z "${path}" || "${path}" == "/" || "${path}" == "${PROJECT_ROOT}"  || "${path}" == "${CHASTE_SOURCE_DIR}" ]]; then
		echo "Error: refusing to remove unsafe path '${path}'." >&2
		exit 1
	fi
	rm -rf "${path}"
}
