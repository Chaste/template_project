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

## 2. Get the SBML model with a clean name

The generated C++ class names come from the **file name**, so the model needs a
C++-friendly name. Here we use `Goldbeter1991`.

A copy of the model is included next to this walkthrough, so just copy it in:

```sh
cp examples/goldbeter_1991/Goldbeter1991.xml .
```

Alternatively, download it from BioModels and rename it yourself (this needs network
access to `www.ebi.ac.uk`):

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
#include "CellPropertyRegistry.hpp"
#include "FixedG1GenerationalCellCycleModel.hpp"
#include "SimulationTime.hpp"
#include "SmartPointers.hpp"
#include "TransitCellProliferativeType.hpp"
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
        // Run until t = 100 with dt = 0.001, by which time the mitotic oscillator
        // has settled to the reference steady state.
        SimulationTime* p_simulation_time = SimulationTime::Instance();
        double end_time = 100;
        double dt = 0.001;
        unsigned num_steps = (unsigned)(end_time / dt);
        p_simulation_time->SetEndTimeAndNumberOfTimeSteps(end_time, num_steps + 1);

        // Create a cell carrying the imported SRN model.
        boost::shared_ptr<AbstractCellProperty> p_healthy_state(
            CellPropertyRegistry::Instance()->Get<WildTypeCellMutationState>());
        boost::shared_ptr<AbstractCellProperty> p_transit_type(
            CellPropertyRegistry::Instance()->Get<TransitCellProliferativeType>());

        FixedG1GenerationalCellCycleModel* p_cell_model = new FixedG1GenerationalCellCycleModel();
        Goldbeter1991SbmlSrnModel* p_srn_model = new Goldbeter1991SbmlSrnModel();

        CellPtr p_cell(new Cell(p_healthy_state, p_cell_model, p_srn_model, false, CellPropertyCollection()));
        p_cell->SetCellProliferativeType(p_transit_type);
        p_cell->InitialiseCellCycleModel();
        p_cell->InitialiseSrnModel();

        // Step the simulation to the end time.
        while (!p_simulation_time->IsFinished())
        {
            p_simulation_time->IncrementTimeOneStep();
            if (p_cell->ReadyToDivide())
            {
                p_cell->Divide();
            }
        }

        // Check the steady state of the mitotic oscillator.
        Goldbeter1991SbmlSrnModel* p_srn = dynamic_cast<Goldbeter1991SbmlSrnModel*>(p_cell->GetSrnModel());
        TS_ASSERT_DELTA(p_srn->GetStateVariable("C"), 0.5470, 1e-2);
        TS_ASSERT_DELTA(p_srn->GetStateVariable("M"), 0.2936, 1e-2);
        TS_ASSERT_DELTA(p_srn->GetStateVariable("X"), 0.0067, 1e-3);
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
