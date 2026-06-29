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
| `fortests/SbmlTestOdeSolution.{hpp,cpp}` | `OdeSolution` Recording per-step parameters, for tests. |
