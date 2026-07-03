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

#ifndef ABSTRACT_SBML_CELL_CYCLE_MODEL_HPP_
#define ABSTRACT_SBML_CELL_CYCLE_MODEL_HPP_

#include <boost/serialization/base_object.hpp>

#include "AbstractCellCycleModelOdeSolver.hpp"
#include "AbstractOdeBasedCellCycleModel.hpp"
#include "AbstractSbmlOdeSystem.hpp"
#include "ChasteSerialization.hpp"
#include "ClassIsAbstract.hpp"

/**
 * A base class for cell cycle models generated from SBML
 */

class AbstractSbmlCellCycleModel : public AbstractOdeBasedCellCycleModel
{
private:
    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive / unarchive the AbstractSbmlCellCycleModel.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template <class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive& boost::serialization::base_object<AbstractOdeBasedCellCycleModel>(*this);
    }

protected:
    /**
     * Protected copy-constructor for use by CreateCellCycleModel().
     *
     * The only way to copy an instance of a subclass of AbstractCellCycleModel is
     * by calling CreateCellCycleModel(), which ensures that the instance is copied
     * correctly.
     *
     * This copy-constructor helps subclasses of AbstractCellCycleModel to
     * ensure that all their members are copied over correctly. It is primarily
     * used during cell division to set member variables for a daughter cell.
     * Note that the cell-cycle model of the parent cell will have run ResetForDivision()
     * just before calling CreateCellCycleModel(), so performing an exact copy of the
     * parent cell's cell-cycle model is suitable behaviour. Any further initialisation
     * specific to the daughter cell can be completed via InitialiseDaughterCell().
     *
     * @param rModel the cell-cycle model to copy.
     */
    AbstractSbmlCellCycleModel(const AbstractSbmlCellCycleModel& rModel);

public:
    /**
     * Default constructor calls base class.
     *
     * @param pOdeSolver An optional pointer to a cell-cycle model ODE solver object (allows the use of different ODE solvers)
     */
    AbstractSbmlCellCycleModel(boost::shared_ptr<AbstractCellCycleModelOdeSolver> pOdeSolver = boost::shared_ptr<AbstractCellCycleModelOdeSolver>());

    /**
     * Destructor.
     */
    virtual ~AbstractSbmlCellCycleModel();

    /**
     * Adjust any ODE parameters needed before solving until currentTime.
     *
     * @param currentTime  the time up to which the system will be solved.
     */
    void AdjustOdeParameters(double currentTime);

    /**
     * Overridden CanCellTerminallyDifferentiate() method.
     * @return whether cell can terminally differentiate
     */
    bool CanCellTerminallyDifferentiate();

    /**
     * Overridden GetAverageStemCellCycleTime() method.
     * @return time
     */
    double GetAverageStemCellCycleTime();

    /**
     * Overridden GetAverageTransitCellCycleTime() method.
     * @return time
     */
    double GetAverageTransitCellCycleTime();

    /**
     * @return the value of a given state variable.
     *
     * @param rName the name of the state variable
     */
    double GetStateVariable(const std::string& rName);

    /**
     * Outputs cell-cycle model parameters to file.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputCellCycleModelParameters(out_stream& rParamsFile);

    /**
     * See AbstractCellCycleModel::ResetForDivision()
     *
     * @return whether the cell is ready to divide (enter M phase).
     */
    bool ReadyToDivide() override;

    /**
     * Each cell-cycle model must be able to be reset 'after' a cell division.
     *
     * Actually, this method is called from Cell::Divide() to
     * reset the cell cycle just before the daughter cell is created.
     * CreateCellCycleModel() can then clone our state to generate a
     * cell-cycle model instance for the daughter cell.
     */
    void ResetForDivision() override;
};

// Register abstract class with Boost serialization
CLASS_IS_ABSTRACT(AbstractSbmlCellCycleModel)

// Register the CellCycleModel<OdeSolver> classes with Boost serialization
#include "CellCycleModelOdeSolverExportWrapper.hpp"
EXPORT_CELL_CYCLE_MODEL_ODE_SOLVER(AbstractSbmlCellCycleModel)

#endif // ABSTRACT_SBML_CELL_CYCLE_MODEL_HPP_
