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

import os
import re
import shutil
import subprocess


class Settings:
    """Paths and substitutions for this template."""

    # The Chaste components the template depends on by default.
    DEFAULT_COMPONENTS = ["continuum_mechanics", "global", "io", "linalg", "mesh", "ode", "pde"]

    # The optional Chaste components the user can choose to depend on.
    OPTIONAL_COMPONENTS = ["cell_based", "crypt", "heart", "lung"]

    def __init__(self) -> None:
        """Set the paths and substitutions from the current project directory."""
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

        # Python binding template files (substituted if Python bindings opted in, deleted otherwise).
        self.PYTHON_BINDING_FILES = [
            os.path.join(self.PROJECT_ROOT, "dynamic", "config.yaml"),
            os.path.join(self.PROJECT_ROOT, "dynamic", "CMakeLists.txt"),
            os.path.join(self.PROJECT_ROOT, "src", "py", "MANIFEST.in"),
            os.path.join(self.PROJECT_ROOT, "src", "py", "setup.cfg"),
            os.path.join(self.PROJECT_ROOT, "src", "py", "template_project", "__init__.py"),
        ]

        # Substitutions applied to all Python binding files.
        self.PYTHON_BINDING_SUBSTITUTIONS = {
            "template_project": self.PROJECT_NAME,
        }

        # Additional substitutions applied only to dynamic/config.yaml.
        self.PYTHON_CONFIG_SUBSTITUTIONS = {
            "Hello.hpp": f"Hello_{self.PROJECT_NAME}.hpp",
            "name: Hello": f"name: Hello_{self.PROJECT_NAME}",
        }

        # Python package template directory (renamed to <project_name>/ during setup).
        self.PYTHON_PKG_TEMPLATE_DIR = os.path.join(self.PROJECT_ROOT, "src", "py", "template_project")


def find_and_replace(filename: str, old_string: str, new_string: str) -> None:
    """Replace every occurrence of old_string with new_string in a file, in place."""
    with open(filename, "r") as f:
        contents = f.read()
    with open(filename, "w") as f:
        f.write(contents.replace(old_string, new_string))


def ask_for_response(question: str, default: bool = False) -> bool:
    """Prompt the user with a yes/no question and return the answer as a bool.

    An empty response returns default; any unrecognised response re-prompts.
    """
    # Define permitted yes/no answers
    yes = {"yes", "y", "ye"}
    no = {"no", "n"}

    # Show the default option in uppercase
    options = "[Y/n]" if default else "[y/N]"
    choice = input(f"{question} {options} ").lower()

    # Decide on the choice
    if choice == "":
        return default
    elif choice in yes:
        return True
    elif choice in no:
        return False
    else:
        return ask_for_response("Please respond with yes or no:", default)


def append_to_file_name(text_to_append: str, file: str) -> str:
    """Insert text_to_append before the file's extension and rename it.

    Returns the new path, e.g. 'Hello.cpp' -> 'Hello_myproject.cpp'.
    """
    root, ext = os.path.splitext(file)
    new_name = root + text_to_append + ext
    os.rename(file, new_name)
    return new_name


def print_banner(*lines: str) -> None:
    """Print the given lines framed in a banner box."""
    width = max(len(line) for line in lines)
    border = "*" * (width + 4)
    print(border)
    for line in lines:
        print(f"* {line.ljust(width)} *")
    print(border)


def create_virtualenv(settings: Settings) -> None:
    """Create the project virtualenv for the Python bindings.

    Creates .virtualenv/ with --system-site-packages so it can see PyChaste's native
    runtime dependencies (petsc4py, mpi4py, vtk) provided by the system Python. On any
    failure this is non-fatal: it prints the manual command so the user can create it
    themselves. The compiled bindings are installed into this virtualenv later by
    scripts/bindings_install.sh, once the project has been built.
    """
    venv_dir = os.path.join(settings.PROJECT_ROOT, ".virtualenv")
    try:
        subprocess.run(["python3", "-m", "venv", "--system-site-packages", venv_dir], check=True)
    except (subprocess.CalledProcessError, OSError) as error:
        print("")
        print(f"WARNING: could not create the project virtualenv automatically ({error}).")
        print("Create it manually with:")
        print(f"  python3 -m venv --system-site-packages {venv_dir}")
        return

    # Warn (non-fatal) if PyChaste's native runtime dependencies are not visible to the
    # venv. They are provided by the system Python (e.g. in the chaste/base Docker image),
    # not pip-installed; the bindings will fail to import at runtime without them.
    venv_python = os.path.join(venv_dir, "bin", "python")
    missing = [
        module
        for module in ("petsc4py", "mpi4py", "vtk")
        if subprocess.run([venv_python, "-c", f"import {module}"], capture_output=True).returncode != 0
    ]
    if missing:
        print("")
        print(f"WARNING: PyChaste runtime dependencies not found: {', '.join(missing)}.")
        print("These are provided by the system Python (e.g. in the chaste/base image) and are")
        print("needed to import the bindings. Install them on your system before using the bindings.")


def is_setup(settings: Settings) -> bool:
    """Return True if the project has already been set up (any of the example files are renamed)."""
    return not all(os.path.exists(file) for file in settings.TEMPLATE_SOURCE_FILES)


def setup(settings: Settings) -> None:
    """Customise the template for this project, after confirming the chosen settings."""
    # Abort if the project has already been configured.
    if is_setup(settings):
        print_banner(
            "ERROR: This Chaste user project has already been set up.",
            "If you want to run setup again, use a fresh copy of the template.",
            "",
            "Alternatively, try the steps below to reset this template.",
            "Note that any changes you have made will be lost forever!!!",
            "1. Run 'git checkout -- .' in the project directory to restore the original files.",
            "2. Run 'git clean -f -- .' in the project directory to remove all new files.",
            "3. Run this script again to set up the project.",
        )
        raise SystemExit(1)

    # Confirm the template directory has been renamed to the project name before making any changes.
    print("Make sure to rename the 'template_project' directory to your project name before running this script.")
    print(f"The current project name is '{settings.PROJECT_NAME}' (same as the current directory name).")
    if not ask_for_response("Do you want to proceed?", default=True):
        return

    # Check that the project name is a valid C++ name.
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", settings.PROJECT_NAME):
        print(
            f"ERROR: the project name '{settings.PROJECT_NAME}' is not a valid C++ name. "
            "Renaming the directory is recommended."
        )
        raise SystemExit(1)

    # Ask which Chaste components this project depends on
    components: list[str] = []
    for component in settings.OPTIONAL_COMPONENTS:
        if ask_for_response(f"Does this project depend on the {component} component?"):
            components.append(component)

    # Ask whether to create Python bindings
    python_bindings = ask_for_response("Do you want to create Python bindings for this project?")

    # Summarise the chosen options and confirm before making any changes
    print("")
    print("Summary:")
    print(f"  Project name:      {settings.PROJECT_NAME}")
    print(f"  Chaste components: {', '.join(components) if components else '(template default)'}")
    print(f"  Python bindings:   {'Yes' if python_bindings else 'No'}")
    print("")
    if not ask_for_response("Proceed with these settings?"):
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
    find_and_replace(
        settings.BASE_CMAKELISTS, "chaste_do_project(template_project)", f"chaste_do_project({settings.PROJECT_NAME})"
    )
    find_and_replace(
        settings.APPS_CMAKELISTS,
        "chaste_do_apps_project(template_project)",
        f"chaste_do_apps_project({settings.PROJECT_NAME})",
    )
    find_and_replace(
        settings.TEST_CMAKELISTS,
        "chaste_do_test_project(template_project)",
        f"chaste_do_test_project({settings.PROJECT_NAME})",
    )

    # Replace the default components if any optional components were selected
    if components:
        find_and_replace(settings.BASE_CMAKELISTS, " ".join(settings.DEFAULT_COMPONENTS), " ".join(components))

    # Set up or remove Python bindings
    if python_bindings:
        # Substitute the project name into all Python binding files
        for file in settings.PYTHON_BINDING_FILES:
            for old, new in settings.PYTHON_BINDING_SUBSTITUTIONS.items():
                find_and_replace(file, old, new)
        # Substitute class names and headers into config.yaml
        config_yaml = os.path.join(settings.PROJECT_ROOT, "dynamic", "config.yaml")
        for old, new in settings.PYTHON_CONFIG_SUBSTITUTIONS.items():
            find_and_replace(config_yaml, old, new)
        # Rename the Python package directory (template_project/ -> <project_name>/)
        new_pkg_dir = os.path.join(settings.PROJECT_ROOT, "src", "py", settings.PROJECT_NAME)
        os.rename(settings.PYTHON_PKG_TEMPLATE_DIR, new_pkg_dir)
        # Create the project virtualenv (the compiled bindings are installed later
        # by scripts/bindings_install.sh).
        create_virtualenv(settings)
    else:
        # Remove the Python binding template files
        shutil.rmtree(os.path.join(settings.PROJECT_ROOT, "dynamic"))
        shutil.rmtree(os.path.join(settings.PROJECT_ROOT, "src", "py"))

    # Summarise the changes that were made
    print("")
    print("Setup complete.")
    print(f"The following changes were made for project '{settings.PROJECT_NAME}':")
    print("* Substituted the project name in all files.")

    if components:
        print(f"* Set Chaste components in CMakeLists.txt to: {', '.join(components)}.")
    print("* Renamed the template files:")

    for original, renamed in zip(settings.TEMPLATE_SOURCE_FILES, appended_file_names):
        print(f"  - {os.path.basename(original)} -> {os.path.basename(renamed)}")

    if python_bindings:
        print("* Set up Python bindings in dynamic/ and src/py/.")
        print("* Created the project virtualenv in .virtualenv/.")
    else:
        print("* Removed Python bindings template files in dynamic/ and src/py/.")


def main() -> None:
    """Set up the project from the template."""
    settings = Settings()
    setup(settings)


if __name__ == "__main__":
    main()
