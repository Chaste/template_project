# A template user project for use with Chaste.

You now simply log in to github, then click the big green "Use this template" button to use a copy of this repository as the basis of your own new repository under your github username/organisation (this 'template' status avoids complications with forks all being linked back to this repo).

Alternatively, if you aren't a github user, you can download a zip (see Releases button) and start your own repository with that.

Then see the [User Projects](https://chaste.github.io/docs/user-guides/user-projects/) guide page on the Chaste website for more information.

If you clone this repository, you should make sure to rename the **template_project** folder with your project name and run the `setup_project.py` script to avoid conflicts if you have multiple projects.

## What's in here

| Path | |
| --- | --- |
| `src/`, `test/`, `apps/` | your project's C++ code; only `src/` is compiled into the project library |
| `scripts/` | configure / compile / test helpers |
| `bindings/` | optional PyChaste Python bindings; see [bindings/README.md](bindings/README.md) |
| `sbml/` | optional SBML model import; see [sbml/README.md](sbml/README.md) |

`setup_project.py` asks whether you want each optional capability and removes the
directory if you decline, so a plain C++ project keeps neither.

## Building

There are simple convenience scripts available to help with the build process. From the project directory, with `CHASTE_SOURCE_DIR` pointing at your Chaste source:

```sh
scripts/configure.sh   # symlink the project into Chaste/projects and configure the build
scripts/compile.sh     # build the project
scripts/test.sh        # run the project's tests
```

If you need more detailed control of the build process, see the
[full Chaste build guide](https://chaste.github.io/docs/dev-guides/cmake-build-guide/).
