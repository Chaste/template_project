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

#ifndef ABSTRACT_SBML_SRN_MODEL_HPP_
#define ABSTRACT_SBML_SRN_MODEL_HPP_

#include <boost/serialization/base_object.hpp>

#include "AbstractOdeSrnModel.hpp"
#include "AbstractSbmlOdeSystem.hpp"
#include "ChasteSerialization.hpp"
#include "ClassIsAbstract.hpp"

/**
 * A base class for SRN models generated from SBML
 */

class AbstractSbmlSrnModel : public AbstractOdeSrnModel
{
private:
    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive / unarchive the AbstractSbmlSrnModel.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template <class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractOdeSrnModel>(*this);
    }

protected:
    /**
     * Protected copy-constructor for use by CreateSrnModel().
     *
     * The only way to copy an instance of a subclass of AbstractCellCycleModel is
     * by calling CreateSrnModel(), which ensures that the instance is copied
     * correctly.
     *
     * This copy-constructor helps subclasses of AbstractSrnModel to
     * ensure that all their members are copied over correctly. It is primarily
     * used during cell division to set member variables for a daughter cell.
     * Note that the SRN model of the parent cell will have run ResetForDivision()
     * just before calling CreateSrnModel(), so performing an exact copy of the
     * parent cell's SRN model is suitable behaviour. Any further initialisation
     * specific to the daughter cell can be completed via InitialiseDaughterCell().
     *
     * @param rModel the SRN model to copy.
     */
    AbstractSbmlSrnModel(const AbstractSbmlSrnModel& rModel);

    using AbstractOdeSrnModel::Initialise;
    /**
     * Overridden Initialise() method to set up the ODE system.
     *
     * @param pOdeSystem pointer to an ODE system
     */
    void Initialise(AbstractSbmlOdeSystem* pOdeSystem);

public:
    /**
     * Constructor.
     *
     * @param stateSize The number of state variables in the ODE system.
     * @param pOdeSolver An optional pointer to a cell-cycle model ODE solver
     *                   object (allows the use of different ODE solvers)
     */
    AbstractSbmlSrnModel(unsigned stateSize,
                         boost::shared_ptr<AbstractCellCycleModelOdeSolver> pOdeSolver = boost::shared_ptr<AbstractCellCycleModelOdeSolver>());

    /**
     * Destructor.
     */
    virtual ~AbstractSbmlSrnModel();

    /**
     * @return the value of a given state variable.
     *
     * @param rName the name of the state variable
     */
    double GetStateVariable(const std::string& rName);

    /**
     * Outputs SRN model parameters to file.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputSrnModelParameters(out_stream& rParamsFile);

    /**
     * Overridden SimulateToCurrentTime() method for custom behaviour
     */
    void SimulateToCurrentTime();
};

// Register abstract class with Boost serialization
CLASS_IS_ABSTRACT(AbstractSbmlSrnModel)

// Register the CellCycleModel<OdeSolver> classes with Boost serialization
#include "CellCycleModelOdeSolverExportWrapper.hpp"
EXPORT_CELL_CYCLE_MODEL_ODE_SOLVER(AbstractSbmlSrnModel)

#endif // ABSTRACT_SBML_SRN_MODEL_HPP_
