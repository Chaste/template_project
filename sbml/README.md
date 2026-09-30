# SBML models

This template turns [SBML](https://sbml.org/) models into Chaste models using
[chaste-sbml](https://github.com/Chaste/chaste-sbml) (see
[documentation](https://chaste.github.io/chaste-sbml/)). For a worked example
see [example/README.md](example/README.md).

Run every command below from your project's root directory. See the
[top-level README](../README.md) for information about project layout and the build process.

## Enable SBML support

When you run `setup_project.py`, answer **yes** to:

```
Do you want to create an SBML user project?
```

## Convert an SBML model into a Chaste model

> If you ever need to re-install chaste-sbml or refresh the SBML base classes, run
> `sbml/install.sh`. This is already run during user project setup by `setup_project.py`.

Activate the virtualenv

```sh
source .virtualenv/bin/activate
```

Convert the model

```sh
chaste-sbml my_model.xml --output-dir src/
```

> The generated class and file names are derived from the input file name, suffixed
> with `Sbml`. For example `my_model.xml` produces `MyModelSbmlOdeSystem.{hpp,cpp}`.

See the [command-line options](https://chaste.github.io/chaste-sbml/command-line.html) for
the rest of what `chaste-sbml` can do, including `--model-type` to generate a sub-cellular
reaction network or cell-cycle model, and `--timescale` to set the model's native time unit.

## Add a test

Add a test under `test/` that `#include`s the generated model, then list it in
`test/ContinuousTestPack.txt`. See the walkthrough for a full example.

## Build and run the test

From the project directory, with `CHASTE_SOURCE_DIR` pointing at your Chaste source:

```sh
scripts/configure.sh   # configure the build
scripts/compile.sh     # build the project
scripts/test.sh        # run the project's tests
```

## The SBML base classes

`sbml/install.sh` copies these into `src/` from the installed `chaste-sbml` package (via
`chaste-sbml --copy-base-classes`). They are
required by the generated code:

| File | Purpose |
| --- | --- |
| `AbstractSbmlOdeSystem.{hpp,cpp}` | Base ODE system for a generated SBML model. |
| `AbstractSbmlSrnModel.{hpp,cpp}` | Base sub-cellular reaction network (SRN) model. |
| `AbstractSbmlCellCycleModel.{hpp,cpp}` | Base cell-cycle model. |
| `SbmlEventType.hpp` | Enum of SBML event types (e.g. cell division). |
| `SbmlMath.hpp` | Math helper functions used by generated equations. |
| `SbmlOdeSolverSetup.hpp` | ODE solver setup shared by the generated models. |
| `fortests/SbmlTestHelpers.{hpp,cpp}` | Utility helpers for tests. |
| `fortests/SbmlTestOdeSolution.{hpp,cpp}` | `OdeSolution` recording per-step parameters, for tests. |

