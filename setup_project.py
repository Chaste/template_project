# Copyright (c) 2005-2026, University of Oxford.
# All rights reserved.
#
# University of Oxford means the Chancellor, Masters and Scholars of the
# University of Oxford, having an administrative office at Wellington
# Square, Oxford OX1 2JD, UK.
#
# This file is part of Chaste.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#  * Neither the name of the University of Oxford nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""Set up a Chaste user project from this template.

Run this script from the project directory once it has been renamed to your project name.
"""

import argparse
import os
import re
import subprocess


class Settings:
    """Paths and substitutions for this template."""

    # The Chaste components the template depends on by default.
    DEFAULT_COMPONENTS = ["continuum_mechanics", "global", "io", "linalg", "mesh", "ode", "pde"]

    # The optional Chaste components the user can choose to depend on.
    OPTIONAL_COMPONENTS = ["cell_based", "crypt", "heart", "lung"]

    def __init__(self) -> None:
        self.update()

    def update(self) -> None:
        """Recompute the paths and substitutions from the current project directory.

        Call this after the project directory is renamed so the values derived from it are refreshed.
        """
        # The project directory and its name (taken from the directory this script lives in).
        self.PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))
        self.PROJECT_NAME = os.path.basename(self.PROJECT_ROOT)

        # Full paths to the example source files.
        self.TEMPLATE_SOURCE_FILES = [
            os.path.join(self.PROJECT_ROOT, "apps", "src", "ExampleApp.cpp"),
            os.path.join(self.PROJECT_ROOT, "src", "Hello.cpp"),
            os.path.join(self.PROJECT_ROOT, "src", "Hello.hpp"),
            os.path.join(self.PROJECT_ROOT, "test", "TestHello.hpp"),
        ]

        # Substitutions for the project name in the CMakeLists.txt files.
        self.BASE_CMAKELISTS = os.path.join(self.PROJECT_ROOT, "CMakeLists.txt")
        self.APPS_CMAKELISTS = os.path.join(self.PROJECT_ROOT, "apps", "CMakeLists.txt")
        self.TEST_CMAKELISTS = os.path.join(self.PROJECT_ROOT, "test", "CMakeLists.txt")

        self.CMAKE_PROJECT_SUBSTITUTIONS = [
            (
                self.BASE_CMAKELISTS,
                "chaste_do_project(template_project)",
                f"chaste_do_project({self.PROJECT_NAME})",
            ),
            (
                self.APPS_CMAKELISTS,
                "chaste_do_apps_project(template_project)",
                f"chaste_do_apps_project({self.PROJECT_NAME})",
            ),
            (
                self.TEST_CMAKELISTS,
                "chaste_do_test_project(template_project)",
                f"chaste_do_test_project({self.PROJECT_NAME})",
            ),
        ]

        # Map of template text -> project text applied to the source and test files.
        # These are deliberately specific to avoid rewriting the printed "Hello world" message.
        self.SOURCE_SUBSTITUTIONS = {
            " TestHello": f" TestHello_{self.PROJECT_NAME}",
            "TestHello.hpp": f"TestHello_{self.PROJECT_NAME}.hpp",
            "HELLO": f"HELLO_{self.PROJECT_NAME.upper()}",
            "Hello world(": f"Hello_{self.PROJECT_NAME} world(",
            "class Hello": f"class Hello_{self.PROJECT_NAME}",
            "Hello::": f"Hello_{self.PROJECT_NAME}::",
            "Hello(": f"Hello_{self.PROJECT_NAME}(",
            "Hello.hpp": f"Hello_{self.PROJECT_NAME}.hpp",
        }

        self.TEST_PACK_FILES = [os.path.join(self.PROJECT_ROOT, "test", "ContinuousTestPack.txt")]


def find_and_replace(filename: str, pattern: str, replacement: str, regex: bool = False) -> None:
    """Replace occurrences of pattern with replacement in a file, in place.

    By default pattern is treated as literal text. Pass regex=True to treat it as
    a regular expression, matched with re.MULTILINE so ^ and $ anchor to line
    boundaries.
    """
    if not regex:
        pattern = re.escape(pattern)
    with open(filename, "r") as f:
        contents = f.read()
    with open(filename, "w") as f:
        f.write(re.sub(pattern, replacement, contents, flags=re.MULTILINE))


def print_banner(*lines: str) -> None:
    """Print the given lines framed in a banner box."""
    width = max(len(line) for line in lines)
    border = "*" * (width + 4)
    print(border)
    for line in lines:
        print(f"* {line.ljust(width)} *")
    print(border)


def ask_for_response(question: str) -> bool:
    """Prompt the user with a yes/no question and return the answer as a bool.

    An empty response defaults to yes; any unrecognised response re-prompts.
    """
    # Display the question
    print(question)

    # Define permitted yes/no answers
    yes = {"yes", "y", "ye", ""}
    no = {"no", "n"}

    # Take the lower case raw input
    choice = input().lower()

    # Decide on the choice
    if choice in yes:
        return True
    elif choice in no:
        return False
    else:
        return ask_for_response("Please respond with yes or no:")


def append_to_file_name(text_to_append: str, file: str) -> str:
    """Insert text_to_append before the file's extension and rename it.

    Returns the new path, e.g. 'Hello.cpp' -> 'Hello_myproject.cpp'.
    """
    root, ext = os.path.splitext(file)
    new_name = root + text_to_append + ext
    os.rename(file, new_name)
    return new_name


def is_git_repository(path: str) -> bool:
    """Return True if path is inside a git working tree."""
    try:
        result = subprocess.run(
            ["git", "-C", path, "rev-parse", "--is-inside-work-tree"],
            capture_output=True,
        )
    except FileNotFoundError:
        return False
    return result.returncode == 0


def setup(settings: Settings) -> None:
    """Customise the template for this project, after confirming the chosen settings."""
    # Confirm the template directory has been renamed to the project name before making any changes.
    print(f"This project will be set up using '{settings.PROJECT_NAME}' (the directory name) as the project name.")
    if not ask_for_response("Do you want to proceed? [Y/n] "):
        print(f"Rename the '{settings.PROJECT_NAME}' directory to your project name, then run this script again.")
        return

    # Recompute settings in case the directory name has changed
    settings.update()

    # Check that the project name is a valid C++ name.
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", settings.PROJECT_NAME):
        print(f"""Error: the project name '{settings.PROJECT_NAME}' contains characters other than letters, digits,
            and underscores. Renaming the directory is recommended.""")
        return

    # Ask which Chaste components this project depends on
    components: list[str] = []
    for component in settings.OPTIONAL_COMPONENTS:
        if ask_for_response(f"Does this project depend on the {component} component? [Y/n] "):
            components.append(component)

    # Summarise the chosen options and confirm before making any changes
    print("")
    print("Summary:")
    print(f"  Project name:      {settings.PROJECT_NAME}")
    print(f"  Chaste components: {', '.join(components) if components else '(template default)'}")
    print("")
    if not ask_for_response("Proceed with these settings? [Y/n] "):
        print("No changes made.")
        return

    # Append the project name to the example source files (avoids clashes between projects)
    suffix = "_" + settings.PROJECT_NAME
    appended_file_names = [append_to_file_name(suffix, file) for file in settings.TEMPLATE_SOURCE_FILES]

    # Substitute the project name into the source and test files
    files_to_sub = appended_file_names + settings.TEST_PACK_FILES
    for file in files_to_sub:
        for old, new in settings.SOURCE_SUBSTITUTIONS.items():
            find_and_replace(file, old, new)

    # Set the project name in the CMakeLists.txt files
    for cmake_file, template_text, project_text in settings.CMAKE_PROJECT_SUBSTITUTIONS:
        find_and_replace(cmake_file, template_text, project_text)

    # Replace the default components if any optional components were selected
    if components:
        find_and_replace(settings.BASE_CMAKELISTS, " ".join(settings.DEFAULT_COMPONENTS), " ".join(components))


def reset(settings: Settings) -> None:
    """Reset the template to its original state using git, discarding setup's changes.

    The project must be a git repository: the tracked template files are restored
    to their committed state and the setup-renamed example files are removed.
    """
    # The project can only be reset from a git repository.
    if not is_git_repository(settings.PROJECT_ROOT):
        print("Error: the project can only be reset if it is a git repository.")
        raise SystemExit(1)

    # Confirm before discarding any changes.
    print_banner(
        "Caution: This will reset the template to its original state!!!",
        "Any changes to the example source and CMakeLists.txt files will be lost!!!",
    )
    if not ask_for_response("Proceed? [Y/n] "):
        print("Aborted.")
        raise SystemExit(1)

    # Remove the setup-renamed example source files (these are untracked by git).
    suffix = "_" + settings.PROJECT_NAME
    for original in settings.TEMPLATE_SOURCE_FILES:
        root, ext = os.path.splitext(original)
        renamed = root + suffix + ext
        if os.path.exists(renamed):
            os.remove(renamed)

    # Restore the tracked template files to their committed state.
    tracked_files = settings.TEMPLATE_SOURCE_FILES + settings.TEST_PACK_FILES + [
        settings.BASE_CMAKELISTS,
        settings.APPS_CMAKELISTS,
        settings.TEST_CMAKELISTS,
    ]
    subprocess.run(
        ["git", "-C", settings.PROJECT_ROOT, "checkout", "HEAD", "--", *tracked_files],
        check=True,
    )


def parse_args() -> argparse.Namespace:
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(
        prog="setup_project",
        description="Set up a Chaste user project from the template.",
    )
    parser.add_argument(
        "--reset",
        action="store_true",
        help="Restore the template to its original state",
    )
    return parser.parse_args()


def main() -> None:
    """Set up the project from the template, or restore the template with --reset."""
    args = parse_args()
    settings = Settings()

    if args.reset:
        reset(settings)
    else:
        setup(settings)


if __name__ == "__main__":
    main()
