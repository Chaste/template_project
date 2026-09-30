#!/usr/bin/env bash
# Shared variables and helpers for the project scripts.
# Source this file from the other scripts (don't run it directly).

# Resolve all paths relative to this file.
_env_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${_env_dir}/.." && pwd)"

# The name of this project is the name of the project directory.
PROJECT_NAME="$(basename "${PROJECT_ROOT}")"

# The shared project virtualenv, used by the Python bindings and SBML install
# scripts which create it on demand.
VENV_DIR="${PROJECT_ROOT}/.virtualenv"

# If CHASTE_SOURCE_DIR is not set, try to find it in a few common locations.
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
elif [[ ! -f "${CHASTE_SOURCE_DIR}/CMakeLists.txt" ]]; then
	# Warn if the supplied CHASTE_SOURCE_DIR doesn't look right.
	echo "Warning: CHASTE_SOURCE_DIR='${CHASTE_SOURCE_DIR}' is not a Chaste source directory." >&2
fi
CHASTE_SOURCE_DIR="${CHASTE_SOURCE_DIR:-}"

# Left empty when no Chaste source was found, so it is never a bare "/projects".
CHASTE_PROJECTS_DIR="${CHASTE_SOURCE_DIR:+${CHASTE_SOURCE_DIR}/projects}"

CHASTE_BUILD_DIR="${CHASTE_BUILD_DIR:-${PROJECT_ROOT}/build}"
if [[ "${CHASTE_BUILD_DIR}" == "${PROJECT_ROOT}" || "${CHASTE_BUILD_DIR}" == "${CHASTE_SOURCE_DIR}" ]]; then
	echo "Error: CHASTE_BUILD_DIR must not be the project root or Chaste source directory." >&2
	echo "Set CHASTE_BUILD_DIR to a separate build directory." >&2
	exit 1
fi

if [[ -z "${CHASTE_TEST_OUTPUT:-}" ]]; then
	export CHASTE_TEST_OUTPUT="${PROJECT_ROOT}/output"
fi

Chaste_UPDATE_PROVENANCE="${Chaste_UPDATE_PROVENANCE:-OFF}"

# Enable PyChaste if this project has Python bindings set up. The root
# CMakeLists.txt gates add_subdirectory(bindings) on the same setting.
if [[ -f "${PROJECT_ROOT}/bindings/config.yaml" ]]; then
	Chaste_ENABLE_PYCHASTE=ON
else
	Chaste_ENABLE_PYCHASTE="${Chaste_ENABLE_PYCHASTE:-OFF}"
fi

# Set the number of parallel jobs for building and testing
if ! [[ "${NCORES:-}" =~ ^[1-9][0-9]*$ ]]; then
	NCORES="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)"
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

# Create a symlink for this project under Chaste/projects/ if necessary.
# Chaste only builds projects that appear there. Project directories can be
# either placed directly under Chaste/projects/ or symlinked there.
register_project() {
	require_source

	local _project_link="${CHASTE_PROJECTS_DIR}/${PROJECT_NAME}"

	mkdir -p "${CHASTE_PROJECTS_DIR}"

	if [[ -L "${_project_link}" && "${_project_link}" -ef "${PROJECT_ROOT}" ]]; then
		: # Already registered: a symlink under Chaste/projects/ points back to this project.
	elif [[ ! -L "${_project_link}" && "${_project_link}" -ef "${PROJECT_ROOT}" ]]; then
		: # Already registered: the project itself lives directly under Chaste/projects/.
	elif [[ -L "${_project_link}" ]]; then
		# Repoint a stale/dangling symlink.
		ln -sfn "${PROJECT_ROOT}" "${_project_link}"
		echo "Re-registered project '${PROJECT_NAME}' under '${CHASTE_PROJECTS_DIR}'."
	elif [[ -e "${_project_link}" ]]; then
		# Another project already exists with this name.
		echo "Error: '${_project_link}' already exists and is not this project." >&2
		echo "Remove or rename it, then re-run configuration." >&2
		exit 1
	else
		# Create a new symlink under Chaste/projects/.
		ln -s "${PROJECT_ROOT}" "${_project_link}"
		echo "Symlinked project '${PROJECT_NAME}' under '${CHASTE_PROJECTS_DIR}'."
	fi
}
