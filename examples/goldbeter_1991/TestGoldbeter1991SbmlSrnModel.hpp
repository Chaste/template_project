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

// The header generated from Goldbeter1991.xml by chaste_codegen_sbml.
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
