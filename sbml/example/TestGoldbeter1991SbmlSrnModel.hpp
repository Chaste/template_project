#ifndef TESTGOLDBETER1991SBMLSRNMODEL_HPP_
#define TESTGOLDBETER1991SBMLSRNMODEL_HPP_

#include <algorithm>
#include <limits>

#include <cxxtest/TestSuite.h>

#include "AbstractCellBasedTestSuite.hpp"
#include "Cell.hpp"
#include "CellPropertyRegistry.hpp"
#include "FixedG1GenerationalCellCycleModel.hpp"
#include "SimulationTime.hpp"
#include "SmartPointers.hpp"
#include "TransitCellProliferativeType.hpp"
#include "WildTypeCellMutationState.hpp"

// The headers generated from Goldbeter1991.xml by chaste-sbml. The SRN model header
// pulls in the ODE system header, but include it explicitly since we name it below.
#include "Goldbeter1991SbmlOdeSystem.hpp"
#include "Goldbeter1991SbmlSrnModel.hpp"

// This is a serial test.
#include "FakePetscSetup.hpp"

namespace
{
// Native time units per hour. The model declares no time unit, so chaste-sbml
// assumes seconds (the SBML Level 2 default) and scales the derivatives by this
// factor, because Chaste integrates in hours.
constexpr double TIMESCALE_MULTIPLIER = Goldbeter1991SbmlOdeSystem::TIMESCALE_MULTIPLIER;
} // namespace

class TestGoldbeter1991SbmlSrnModel : public AbstractCellBasedTestSuite
{
public:
    /**
     * Goldbeter 1991 is a mitotic *oscillator*: it settles onto a limit cycle
     * rather than to a fixed point. We track each variable's range over several
     * whole cycles and check that envelope, which is a property of the cycle
     * itself and so does not depend on where we stop.
     */
    void TestLimitCycleEnvelope()
    {
        // Written in the model's native units (seconds); converted to hours.
        const double sample_time = 100.0 / TIMESCALE_MULTIPLIER; // 4 cycles (~25s each)
        const double settle_time = 100.0 / TIMESCALE_MULTIPLIER; // discard the transient start
        const double dt = 0.001 / TIMESCALE_MULTIPLIER;

        const double end_time = settle_time + sample_time;
        unsigned num_steps = (unsigned)(end_time / dt);

        SimulationTime* p_simulation_time = SimulationTime::Instance();
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

        // Step to the end time
        constexpr double inf = std::numeric_limits<double>::infinity();
        double min_C = inf, max_C = -inf;
        double min_M = inf, max_M = -inf;
        double min_X = inf, max_X = -inf;

        while (!p_simulation_time->IsFinished())
        {
            p_simulation_time->IncrementTimeOneStep();
            if (p_cell->ReadyToDivide())
            {
                p_cell->Divide();
            }

            if (p_simulation_time->GetTime() < settle_time)
            {
                continue;
            }

            // Start recording each variable's range once the transient start has passed.
            Goldbeter1991SbmlSrnModel* p_srn
                = dynamic_cast<Goldbeter1991SbmlSrnModel*>(p_cell->GetSrnModel());
            const double C = p_srn->GetStateVariable("C"); // cyclin
            const double M = p_srn->GetStateVariable("M"); // active cdc2 kinase
            const double X = p_srn->GetStateVariable("X"); // active cyclin protease

            min_C = std::min(min_C, C);
            max_C = std::max(max_C, C);
            min_M = std::min(min_M, M);
            max_M = std::max(max_M, M);
            min_X = std::min(min_X, X);
            max_X = std::max(max_X, X);
        }

        // The limit cycle each variable traces out. A model that stopped
        // oscillating or changed amplitude would fail.
        TS_ASSERT_DELTA(min_C, 0.1895, 2e-2);
        TS_ASSERT_DELTA(max_C, 0.5815, 2e-2);
        TS_ASSERT_DELTA(min_M, 0.0060, 1e-2);
        TS_ASSERT_DELTA(max_M, 0.7093, 2e-2);
        TS_ASSERT_DELTA(min_X, 0.0001, 1e-2);
        TS_ASSERT_DELTA(max_X, 0.6416, 2e-2);

        // It must actually be oscillating: a fixed point would collapse each
        // range to zero.
        TS_ASSERT_LESS_THAN(0.3, max_C - min_C);
        TS_ASSERT_LESS_THAN(0.5, max_M - min_M);
        TS_ASSERT_LESS_THAN(0.5, max_X - min_X);
    }
};

#endif /*TESTGOLDBETER1991SBMLSRNMODEL_HPP_*/
