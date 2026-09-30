# Python bindings

This template builds [PyChaste](https://chaste.github.io/) Python bindings for your
project's C++ classes using [cppwg](https://github.com/Chaste/cppwg), so you can use
your project from Python. For a complete example, see
[example/README.md](example/README.md).

Run every command below from your project's root directory. See the
[top-level README](../README.md) for the project layout and the build cycle.

## Prerequisites
Creating bindings requires:

* [cppwg](https://github.com/Chaste/cppwg): used at configure time to generate the
  wrappers
* PyChaste's native runtime dependencies: `petsc4py` and `vtk` (Python VTK wrappers).

The [`chaste/base`](https://hub.docker.com/r/chaste/base) Docker image provides all
of these. The user project virtualenv is created with the
`--system-site-packages` flag so it can see them, as they are installed as system packages.

If you are **not** working inside the chaste/base image,
install these dependencies into the system Python (or `pip install` them into project virtualenv) yourself
before running configuration: `petsc4py`, and `vtk`. Note that the versions must match the versions of PETSc and VTK on your system.

## Enable Python bindings

When you run `setup_project.py`, answer **yes** to:

```
Do you want to create Python bindings for this project?
```

This keeps the binding scaffolding and wires it to your project name:

* `bindings/config.yaml`: the cppwg configuration listing the classes to wrap,
* `bindings/CMakeLists.txt`: builds the bindings as part of the project,
* `bindings/package/`: the installable Python package (its `template_project/`
  subdirectory is renamed to `<project_name>/`).

If you answer no, the whole `bindings/` directory is removed and the project is a plain C++ project.

## Configure and compile

From the project directory, with `CHASTE_SOURCE_DIR` pointing at your Chaste source:

```sh
scripts/configure.sh   # configures the build with PyChaste enabled
scripts/compile.sh     # builds the project, including the Python bindings
```

## Install the bindings into the project virtualenv

```sh
bindings/install.sh
```

This creates the project virtualenv and installs both PyChaste and your
project's bindings package into it.

## Use your project from Python

Activate the virtualenv

```sh
source .virtualenv/bin/activate
```

```python
import myproject  # replace with your project name
hello = myproject.Hello_myproject("Hello from Python!")
print(hello.GetMessage())
```

## Add your own C++ classes to the bindings

To expose a new class created in `src/`, add it in `bindings/config.yaml` under `classes:`

For example:

```yaml
modules:
  - name: all
    source_locations:
      - src/
    classes:
      - name: Hello
```

See [example/README.md](example/README.md) for a full
example.

## Troubleshooting the bindings

**Wrapper generation fails during `scripts/configure.sh`.** cppwg runs at
configure time, and the full cppwg output is written to `cppwg.log` in the project's
build tree, at `${CHASTE_BUILD_DIR}/projects/<project_name>/bindings/cppwg.log`;
read it to see which class or header caused the failure.
This could be due to an error in `bindings/config.yaml` e.g. a misspelt class.  Fix `config.yaml`,
and re-run `configure.sh` to regenerate the wrappers.

**Configure fails with "No Python wrapper sources were generated".** cppwg ran
but produced no wrappers, usually because no classes in the list were matched from `bindings/config.yaml`.
Check the `classes:` and `source_locations:`
entries in `config.yaml` against `cppwg.log`, then re-configure.

**Compilation fails with missing PyChaste or Chaste headers.** Make sure PyChaste is enabled. Try also deleting the build directory (`$CHASTE_BUILD_DIR`, by default
`build/` in the project) and re-run `configure.sh`.

**`import myproject` fails at runtime**, typically with an error importing
`petsc4py` or `vtk`. Those are PyChaste's native runtime dependencies
and must be available. See
[Prerequisites](#prerequisites). Try activating the project virtualenv if not already active, or try installing the dependencies yourself.

> See also https://chaste.github.io/pychaste/dev-guide/
