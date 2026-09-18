# Walkthrough: importing the Goldbeter 1991 model from BioModels

This walkthrough imports [BIOMD0000000003](https://biomodels.org/BIOMD0000000003) — the
**Goldbeter 1991** minimal cascade model of the mitotic oscillator — into a Chaste user
project as a sub-cellular reaction network (SRN) model, then builds and tests it.

It assumes you have already created your project from this template and answered **yes**
to the SBML prompt in `setup_project.py`, so that:

* the SBML base classes are present in `src/`,
* `cell_based` is listed in `CMakeLists.txt`, and
* `chaste-sbml` is installed in `.virtualenv/`.

Run every command below from your project's root directory.

## 1. Activate the virtualenv

```sh
source .virtualenv/bin/activate
```

## 2. Get the SBML model with a clean name

The generated C++ class names come from the **file name**, so the model needs a
C++-friendly name. Here we use `Goldbeter1991`.

A copy of the model is included next to this walkthrough, so just copy it in:

```sh
cp examples/sbml_example/Goldbeter1991.xml .
```

Alternatively, download it from BioModels and rename it yourself (this needs network
access to `www.ebi.ac.uk`):

```sh
curl -L "https://www.ebi.ac.uk/biomodels/model/download/BIOMD0000000003?filename=BIOMD0000000003_url.xml" -o Goldbeter1991.xml
```

## 3. Convert the model into a Chaste model

Goldbeter 1991 is a sub-cellular reaction network, so use `--model-type srn`:

```sh
chaste-sbml Goldbeter1991.xml --model-type srn --output-dir src/
```

This generates four files in `src/`:

* `Goldbeter1991SbmlOdeSystem.hpp` / `.cpp` — the ODE system (state variables `C`, `M`, `X`).
* `Goldbeter1991SbmlSrnModel.hpp` / `.cpp` — the SRN model that wraps the ODE system.

These have the base classes (`AbstractSbmlOdeSystem`, `AbstractSbmlSrnModel`).

## 4. Create the test file

A ready-made test lives next to this walkthrough at
[`TestGoldbeter1991SbmlSrnModel.hpp`](TestGoldbeter1991SbmlSrnModel.hpp). Copy it into
your `test/` directory:

```sh
cp examples/sbml_example/TestGoldbeter1991SbmlSrnModel.hpp test/
```

It builds a cell carrying the imported SRN model, runs it, and checks the behaviour of
cyclin (`C`), active cdc2 kinase (`M`) and active cyclin protease (`X`).

**Times are written in the model's native units.** Goldbeter 1991 declares no
time unit, so `chaste-sbml` assumes seconds (the SBML Level 2 default) and
scales the generated derivatives by 3600 because Chaste integrates in hours.
The test divides its times by that factor, so `100.0 / TIMESCALE_MULTIPLIER`
means 100 native units:

```cpp
constexpr double TIMESCALE_MULTIPLIER = Goldbeter1991SbmlOdeSystem::TIMESCALE_MULTIPLIER;
```

> Pass `--timescale ms|s|m|h` when generating if your model's real unit differs.

Then register the test by adding its file name to `test/ContinuousTestPack.txt`:

```
TestGoldbeter1991SbmlSrnModel.hpp
```

## 5. Build and run the test

With `CHASTE_SOURCE_DIR` pointing at your Chaste source tree:

```sh
scripts/configure.sh   # register the project and configure the Chaste build
scripts/compile.sh     # build the project (including the generated model)
scripts/test.sh        # run the project's tests
```

You should see `TestGoldbeter1991SbmlSrnModel` pass, confirming the steady-state values:

```
C ≈ 0.547   M ≈ 0.294   X ≈ 0.0067
```

## Next steps

* To import a different model, repeat steps 2–4 with your own `.xml` file, choosing
  `--model-type generic`, `srn`, or `cell-cycle` to match the model.
* See the [chaste-sbml documentation](https://chaste.github.io/chaste-sbml/) for the full
  set of command-line options.
