# SBML models

This template can turn an [SBML](https://sbml.org/) model into a Chaste model using
[chaste-sbml](https://github.com/Chaste/chaste-sbml) (see its
[documentation](https://chaste.github.io/chaste-sbml/)). For a complete, worked example
see [example/README.md](example/README.md).

Run every command below from your project's root directory. See the
[top-level README](../README.md) for the project layout and the build cycle.

## 1. Enable SBML support

When you run `setup_project.py`, answer **yes** to:

```
Do you want to create an SBML user project?
```

## 2. Activate the virtualenv

```sh
source .virtualenv/bin/activate
```

(If you ever need to (re)install the generator or refresh the base classes by hand, run
`sbml/scripts/install.sh`.)

## 3. Convert an SBML model into a Chaste model

```sh
chaste-sbml my_model.xml --model-type srn --output-dir src/
```

* `--model-type` is one of `generic`, `srn` (sub-cellular reaction network), or
  `cell-cycle`.
* The generated class and file names are derived from the **input file name**, suffixed
  with `Sbml`. For example `my_model.xml` produces `MyModelSbmlOdeSystem.{hpp,cpp}`, plus
  `MyModelSbmlSrnModel.{hpp,cpp}` (for `srn`) or `MyModelSbmlCellCycleModel.{hpp,cpp}`
  (for `cell-cycle`).
* `--tests` writes a placeholder CxxTest skeleton `Test<Name>.hpp` next to the model, and
  `--no-tests` suppresses it. Use `--test-output-dir test/` to put a generated
  placeholder somewhere sensible instead (it implies `--tests`).
* `--timescale ms|s|m|h` sets the model's native time unit, which the generator converts
  to Chaste's hours. Omit it to auto-detect from the SBML.

## 4. Write a test

Add a test under `test/` that `#include`s the generated model, then list it in
`test/ContinuousTestPack.txt`. See the walkthrough for a full example.

## 5. Build and run the test

From the project directory (with `CHASTE_SOURCE_DIR` pointing at your Chaste source):

```sh
scripts/configure.sh   # register the project and configure the Chaste build
scripts/compile.sh     # build the project
scripts/test.sh        # run the project's tests
```

## The SBML base classes

`setup_project.py` copies these into `src/` from the installed `chaste-sbml` package (via
`chaste-sbml --copy-base-classes`), so they always match the generator version. They are
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

