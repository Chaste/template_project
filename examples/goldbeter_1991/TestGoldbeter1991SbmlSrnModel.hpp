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

// The header generated from Goldbeter1991.xml by chaste-sbml.
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
