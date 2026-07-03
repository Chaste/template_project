# A template user project for use with Chaste.

You now simply log in to github, then click the big green "Use this template" button to use a copy of this repository as the basis of your own new repository under your github username/organisation (this 'template' status avoids complications with forks all being linked back to this repo).

Alternatively, if you aren't a github user, you can download a zip (see Releases button) and start your own repository with that.

Then see the [User Projects](https://chaste.github.io/docs/user-guides/user-projects/) guide page on the Chaste website for more information.

If you clone this repository, you should make sure to rename the template_project folder with your project name and run the 'setup_project.py' script to avoid conflicts if you have multiple projects.

## SBML models

This template can turn an [SBML](https://sbml.org/) model into a Chaste model using
[chaste-codegen-sbml](https://github.com/Chaste/chaste-codegen-sbml). For a complete,
worked example see [examples/goldbeter_1991/README.md](examples/goldbeter_1991/README.md).

### 1. Enable SBML support

When you run `setup_project.py`, answer **yes** to:

```
Do you want to create an SBML user project?
```

### 2. Activate the virtualenv

```sh
source .virtualenv/bin/activate
```

(If you ever need to (re)install the generator by hand, run `scripts/sbml_install.sh`.)

### 3. Convert an SBML model into a Chaste model

```sh
chaste_codegen_sbml my_model.xml --model-type srn --output-dir src/
```

* `--model-type` is one of `generic`, `srn` (sub-cellular reaction network), or
  `cell-cycle`.
* The generated class and file names are derived from the **input file name**, suffixed
  with `Sbml`. For example `my_model.xml` produces `MyModelSbmlOdeSystem.{hpp,cpp}`, plus
  `MyModelSbmlSrnModel.{hpp,cpp}` (for `srn`) or `MyModelSbmlCellCycleModel.{hpp,cpp}`
  (for `cell-cycle`).

### 4. Write a test

Add a test under `test/` that `#include`s the generated model, then list it in
`test/ContinuousTestPack.txt`. See the walkthrough for a full example.

### 5. Build and run the test

From the project directory (with `CHASTE_SOURCE_DIR` pointing at your Chaste source):

```sh
scripts/configure.sh   # register the project and configure the Chaste build
scripts/compile.sh     # build the project
scripts/test.sh        # run the project's tests
```

### The SBML base classes

These live in `src/` and are required by the generated code:

| File | Purpose |
| --- | --- |
| `AbstractSbmlOdeSystem.{hpp,cpp}` | Base ODE system for a generated SBML model. |
| `AbstractSbmlSrnModel.{hpp,cpp}` | Base sub-cellular reaction network (SRN) model. |
| `AbstractSbmlCellCycleModel.{hpp,cpp}` | Base cell-cycle model. |
| `SbmlEventType.hpp` | Enum of SBML event types (e.g. cell division). |
| `SbmlMath.{hpp,cpp}` | Math helper functions used by generated equations. |
| `fortests/SbmlTestHelpers.{hpp,cpp}` | Utility helpers for tests. |
| `fortests/SbmlTestOdeSolution.{hpp,cpp}` | `OdeSolution` recording per-step parameters, for tests. |

## Python bindings

This template can build [PyChaste](https://chaste.github.io/) Python bindings for your
project's C++ classes using [cppwg](https://github.com/Chaste/cppwg), so you can drive
your project from Python. For a complete, worked example — writing a new C++ `Force` and
using it in a Python simulation — see
[examples/my_force/README.md](examples/my_force/README.md).

### Prerequisites

Building and installing the bindings needs, in addition to a Chaste source tree:

* [cppwg](https://github.com/Chaste/cppwg) (with cross-module inheritance support — the
  `imports` and `external_bases` config keys), used at configure time to generate the
  wrappers, and
* PyChaste's native runtime dependencies — `petsc4py`, `mpi4py` and `vtk` — importable from
  the Python interpreter used to run your bindings.

The latest [`chaste/base`](https://hub.docker.com/r/chaste/base) Docker image already provides all
of these, and `scripts/bindings_install.sh` creates the project virtualenv with
`--system-site-packages` so it can see them. Pull the latest image with `docker pull chaste/base`. If you are **not** working inside that image,
`pip install` these dependencies into the system Python (or the project virtualenv) yourself
before running configuration: `petsc4py`, `mpi4py`, and `vtk`, which all build against the versions of PETSc, MPI, and VTK on your system.

### 1. Enable Python bindings

When you run `setup_project.py`, answer **yes** to:

```
Do you want to create Python bindings for this project?
```

This keeps the binding scaffolding and wires it to your project name:

* `dynamic/config.yaml` — the cppwg configuration listing the classes to wrap,
* `dynamic/CMakeLists.txt` — builds the bindings as part of the project,
* `src/py/` — the installable Python package (renamed to `<project_name>/`).

If you answer no, this scaffolding is removed and the project is a plain C++ project.

### 2. Configure and compile

From the project directory (with `CHASTE_SOURCE_DIR` pointing at your Chaste source):

```sh
scripts/configure.sh   # registers the project and configures the Chaste build
scripts/compile.sh     # builds the project, including the Python bindings
```

`configure.sh` automatically enables PyChaste when `dynamic/config.yaml` is present.

### 3. Install the bindings into the project virtualenv

```sh
scripts/bindings_install.sh
```

This creates a project virtualenv in `.virtualenv/` and installs both PyChaste and your
project's bindings package into it.

### 4. Activate the virtualenv and use your project from Python

```sh
source .virtualenv/bin/activate
```

```python
import myproject                      # replace with your project name
hello = myproject.Hello_myproject("Hello from Python!")
print(hello.GetMessage())
```

### 5. Add your own C++ classes to the bindings

To expose a new class, add it to `src/`, then list it in `dynamic/config.yaml`:

* add the header to `source_includes:`, and
* add `- name: YourClass` under the `all` module's `classes:`.

Then recompile (`scripts/compile.sh`) and reinstall (`scripts/bindings_install.sh`).

If your class inherits from a Chaste class that is wrapped in PyChaste (for example a
custom `AbstractForce` subclass), import PyChaste's compiled module under the `all`
module's `imports:` and name the base class under `external_bases:` so cppwg can link the
inheritance across modules:

```yaml
modules:
  - name: all
    imports:
      - chaste._pychaste_all
    external_bases:
      - AbstractForce
    classes:
      - name: YourClass
```

> **Class names for templated classes.** A templated class is wrapped once per
> dimension, with the dimensions appended after an underscore. For example a class
> templated over `<unsigned DIM>` becomes `YourClass_2` / `YourClass_3`, and one templated
> over `<ELEMENT_DIM, SPACE_DIM>` becomes `YourClass_2_2` / `YourClass_3_3`. (PyChaste's own
> classes additionally expose no-underscore aliases such as `OffLatticeSimulation2_2`.) If
> you are unsure of a generated name, run
> `print([n for n in dir(myproject) if "YourClass" in n])`.

See [examples/my_force/README.md](examples/my_force/README.md) for a full walkthrough of
this process.

### Troubleshooting the bindings

**Wrapper generation fails during `scripts/configure.sh`.** cppwg runs at configure time, so
an error in `dynamic/config.yaml` (a misspelt class, a missing header, an unmatched template
signature) fails the configure step immediately. The full cppwg output is written to
`cppwg.log` in the project's build tree, at
`${CHASTE_BUILD_DIR}/projects/<project_name>/dynamic/cppwg.log`; read it to see which class
or header caused the failure, fix `dynamic/config.yaml`, and re-run `scripts/configure.sh`.
Wrappers are regenerated on every configure, so your edits are always picked up.

**Configure fails with "No Python wrapper sources were generated".** cppwg ran but produced
no wrappers — usually because no classes under the `all` module actually matched (for
example the header was not found on the include path, or every class name was misspelt).
Check the `classes:` and `source_includes:` entries in `dynamic/config.yaml` against
`cppwg.log`, then re-configure.

**Compilation fails with missing PyChaste or Chaste headers.** Make sure PyChaste is enabled
(it is automatically when `dynamic/config.yaml` is present) and that your Chaste source tree
is built with PyChaste support. To force a clean rebuild of just the wrappers,
`make <project_name>_wrappers` from the build directory, or run `scripts/clean.sh` followed by `scripts/configure.sh`.

**`import myproject` fails at runtime**, typically with an error importing `petsc4py`,
`mpi4py` or `vtk`. Those are PyChaste's native runtime dependencies and must be importable
from the interpreter running your script — see [Prerequisites](#prerequisites). Activate the
project virtualenv (`source .virtualenv/bin/activate`), which is created with
`--system-site-packages` so it can see them, or install them yourself when working outside
the `chaste/base` image.
