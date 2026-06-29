# Walkthrough: importing the Goldbeter 1991 model from BioModels

This walkthrough imports [BIOMD0000000003](https://biomodels.org/BIOMD0000000003) — the
**Goldbeter 1991** minimal cascade model of the mitotic oscillator — into a Chaste user
project as a sub-cellular reaction network (SRN) model, then builds and tests it.

It assumes you have already created your project from this template and answered **yes**
to the SBML prompt in `setup_project.py`, so that:

* the SBML base classes are present in `src/`,
* `cell_based` is listed in `CMakeLists.txt`, and
* `chaste-codegen-sbml` is installed in `.virtualenv/`.

Run every command below from your project's root directory.

## 1. Activate the virtualenv

```sh
source .virtualenv/bin/activate
```

## 2. Download the SBML model and give it a clean name

The generated C++ class names come from the **file name**, so download the model and
rename it to something C++-friendly. Here we use `Goldbeter1991`:

```sh
curl -L "https://www.ebi.ac.uk/biomodels/model/download/BIOMD0000000003?filename=BIOMD0000000003_url.xml" -o Goldbeter1991.xml
```

## 3. Convert the model into a Chaste model

Goldbeter 1991 is a sub-cellular reaction network, so use `--model-type srn`:

```sh
chaste_codegen_sbml Goldbeter1991.xml --model-type srn --output-dir src/
```

This generates four files in `src/`:

* `Goldbeter1991SbmlOdeSystem.hpp` / `.cpp` — the ODE system (state variables `C`, `M`, `X`).
* `Goldbeter1991SbmlSrnModel.hpp` / `.cpp` — the SRN model that wraps the ODE system.

These have the base classes (`AbstractSbmlOdeSystem`, `AbstractSbmlSrnModel`).

## 4. Create the test file

Create `test/TestGoldbeter1991SbmlSrnModel.hpp` with the following contents. It
builds a cell carrying the imported SRN model, integrates it to a steady state,
and checks the concentrations of cyclin (`C`), active cdc2 kinase (`M`), and
active cyclin protease (`X`).

```cpp
#ifndef TESTGOLDBETER1991SBMLSRNMODEL_HPP_
#define TESTGOLDBETER1991SBMLSRNMODEL_HPP_

#include <cxxtest/TestSuite.h>

#include "AbstractCellBasedTestSuite.hpp"
#include "Cell.hpp"
#include "NoCellCycleModel.hpp"
#include "SimulationTime.hpp"
#include "SmartPointers.hpp"
#include "StemCellProliferativeType.hpp"
#include "WildTypeCellMutationState.hpp"

// The header generated from Goldbeter1991.xml in step 3.
#include "Goldbeter1991SbmlSrnModel.hpp"

// This is a serial test.
#include "FakePetscSetup.hpp"

class TestGoldbeter1991SbmlSrnModel : public AbstractCellBasedTestSuite
{
public:
    void TestSteadyStateSimulation()
    {
        // Integrate to t = 1000 in 1000 steps (AbstractCellBasedTestSuite has set the start time to 0).
        SimulationTime* p_simulation_time = SimulationTime::Instance();
        p_simulation_time->SetEndTimeAndNumberOfTimeSteps(1000.0, 1000);

        // Create the SRN model from the imported SBML model and attach it to a cell.
        Goldbeter1991SbmlSrnModel* p_srn_model = new Goldbeter1991SbmlSrnModel();

        MAKE_PTR(WildTypeCellMutationState, p_state);
        MAKE_PTR(StemCellProliferativeType, p_stem_type);
        NoCellCycleModel* p_cc_model = new NoCellCycleModel();

        CellPtr p_cell(new Cell(p_state, p_cc_model, p_srn_model));
        p_cell->SetCellProliferativeType(p_stem_type);
        p_cell->InitialiseCellCycleModel();
        p_cell->InitialiseSrnModel();

        // Step the simulation to the end time, advancing the SRN model each step.
        while (!p_simulation_time->IsFinished())
        {
            p_simulation_time->IncrementTimeOneStep();
            p_srn_model->SimulateToCurrentTime();
        }

        // Check the steady state of the mitotic oscillator.
        TS_ASSERT_DELTA(p_srn_model->GetStateVariable("C"), 0.5470, 1e-2);
        TS_ASSERT_DELTA(p_srn_model->GetStateVariable("M"), 0.2936, 1e-2);
        TS_ASSERT_DELTA(p_srn_model->GetStateVariable("X"), 0.0067, 1e-3);
    }
};

#endif /*TESTGOLDBETER1991SBMLSRNMODEL_HPP_*/
```

A ready-made copy of this file lives next to this walkthrough at
[`TestGoldbeter1991SbmlSrnModel.hpp`](TestGoldbeter1991SbmlSrnModel.hpp); you can simply
copy it into your `test/` directory:

```sh
cp examples/goldbeter_1991/TestGoldbeter1991SbmlSrnModel.hpp test/
```

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
* See [chaste-codegen-sbml](https://github.com/Chaste/chaste-codegen-sbml)
  for more information.
