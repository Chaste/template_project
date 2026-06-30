# A template user project for use with Chaste.

You now simply log in to github, then click the big green "Use this template" button to use a copy of this repository as the basis of your own new repository under your github username/organisation (this 'template' status avoids complications with forks all being linked back to this repo).

Alternatively, if you aren't a github user, you can download a zip (see Releases button) and start your own repository with that.

Then see the [User Projects](https://chaste.github.io/docs/user-guides/user-projects/) guide page on the Chaste website for more information.

If you clone this repository, you should make sure to rename the template_project folder with your project name and run the 'setup_project.py' script to avoid conflicts if you have multiple projects.

## Python bindings

This template can build [PyChaste](https://chaste.github.io/) Python bindings for your
project's C++ classes using [cppwg](https://github.com/Chaste/cppwg), so you can drive
your project from Python. For a complete, worked example — writing a new C++ `Force` and
using it in a Python simulation — see
[examples/my_force/README.md](examples/my_force/README.md).

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
custom `AbstractForce` subclass), also import PyChaste's compiled module under the `all`
module's `imports:` so cppwg can link the inheritance across modules:

```yaml
modules:
  - name: all
    imports:
      - chaste._pychaste_all
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
