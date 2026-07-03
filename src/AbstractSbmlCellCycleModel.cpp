/*

Copyright (c) 2005-2025, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "AbstractOdeBasedCellCycleModel.hpp"
#include "BackwardEulerIvpOdeSolver.hpp"
#include "CellCycleModelOdeSolver.hpp"
#include "CvodeAdaptor.hpp"
#include "SbmlEventType.hpp"
#include "StemCellProliferativeType.hpp"
#include "TransitCellProliferativeType.hpp"

#include "AbstractSbmlCellCycleModel.hpp"

AbstractSbmlCellCycleModel::AbstractSbmlCellCycleModel(boost::shared_ptr<AbstractCellCycleModelOdeSolver> pOdeSolver)
        : AbstractOdeBasedCellCycleModel(SimulationTime::Instance()->GetTime(), pOdeSolver)
{
    if (!mpOdeSolver)
    {
#ifdef CHASTE_CVODE
        // Default to CVODE where available
        mpOdeSolver = CellCycleModelOdeSolver<AbstractSbmlCellCycleModel, CvodeAdaptor>::Instance();
        mpOdeSolver->Initialise();
        mpOdeSolver->SetMaxSteps(10000); // Safe defaults
        mpOdeSolver->SetTolerances(1e-6, 1e-8);
        // CVODE needs to be instructed to check for stopping events
        mpOdeSolver->CheckForStoppingEvents();
#else
        // Default to Chaste Runge-Kutta solver where CVODE is not available
        mpOdeSolver = CellCycleModelOdeSolver<AbstractSbmlCellCycleModel, RungeKutta4IvpOdeSolver>::Instance();
        mpOdeSolver->Initialise();
        this->SetDt(0.0001); // Safe default
#endif // CHASTE_CVODE
    }

    assert(mpOdeSolver->IsSetUp());
}

AbstractSbmlCellCycleModel::AbstractSbmlCellCycleModel(const AbstractSbmlCellCycleModel& rModel)
        : AbstractOdeBasedCellCycleModel(rModel)
{
    /*
     * Set each member variable of the new Cell Cycle model that inherits
     * its value from the parent.
     *
     * Note 1: some of the new Cell Cycle model's member variables
     * will already have been correctly initialized in its constructor.
     *
     * Note 2: one or more of the new Cell Cycle model's member variables
     * may be set/overwritten as soon as InitialiseDaughterCell() is called on
     * the new Cell Cycle model.
     *
     * Note 3: Only set the variables defined in this class. Variables defined
     * in parent classes will be defined there.
     */
}

AbstractSbmlCellCycleModel::~AbstractSbmlCellCycleModel()
{
}

void AbstractSbmlCellCycleModel::AdjustOdeParameters(double currentTime)
{
    static_cast<AbstractSbmlOdeSystem*>(mpOdeSystem)->AdjustParameters(currentTime);
}

bool AbstractSbmlCellCycleModel::CanCellTerminallyDifferentiate()
{
    return false;
}

double AbstractSbmlCellCycleModel::GetAverageTransitCellCycleTime()
{
    // A default value, should be overridden in subclasses
    return 1.25;
}

double AbstractSbmlCellCycleModel::GetAverageStemCellCycleTime()
{
    // A default value, should be overridden in subclasses
    return 1.25;
}

double AbstractSbmlCellCycleModel::GetStateVariable(const std::string& rName)
{
    assert(mpOdeSystem != nullptr);
    return mpOdeSystem->GetStateVariable(rName);
}

void AbstractSbmlCellCycleModel::OutputCellCycleModelParameters(out_stream& rParamsFile)
{
    // No new parameters to output, so just call method on direct parent class
    AbstractOdeBasedCellCycleModel::OutputCellCycleModelParameters(rParamsFile);
}

bool AbstractSbmlCellCycleModel::ReadyToDivide()
{
    if (!mReadyToDivide)
    {
        bool was_ready_to_divide = mReadyToDivide;
        double previous_divide_time = mDivideTime;

        // Solves ODE to current time and update cell division flag and time
        bool stopping_event_occurred = AbstractOdeBasedCellCycleModel::ReadyToDivide();

        if (stopping_event_occurred)
        {
            // Reset division flag and time if stopping event is not cell division
            if (!static_cast<AbstractSbmlOdeSystem*>(mpOdeSystem)->HasEventOccurred(SbmlEventType::CELL_DIVISION))
            {
                mReadyToDivide = was_ready_to_divide;
                mDivideTime = previous_divide_time;
            }
        }
    }
    return mReadyToDivide;
}

void AbstractSbmlCellCycleModel::ResetForDivision()
{
    assert(mReadyToDivide);
    AbstractOdeBasedCellCycleModel::ResetForDivision();

    assert(mpOdeSystem != nullptr);
    static_cast<AbstractSbmlOdeSystem*>(mpOdeSystem)->ResetEventsOccurred();
}

// Register class with Boost serialization
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(AbstractSbmlCellCycleModel)

// Register the CellCycleModel<OdeSolver> classes with Boost serialization
#include "CellCycleModelOdeSolverExportWrapper.hpp"
EXPORT_CELL_CYCLE_MODEL_ODE_SOLVER(AbstractSbmlCellCycleModel)
