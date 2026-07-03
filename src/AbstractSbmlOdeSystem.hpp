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

#ifndef ABSTRACT_SBML_ODE_SYSTEM_HPP_
#define ABSTRACT_SBML_ODE_SYSTEM_HPP_

#include <boost/serialization/base_object.hpp>

#include "AbstractOdeSystem.hpp"
#include "ChasteSerialization.hpp"
#include "ClassIsAbstract.hpp"
#include "SbmlEventType.hpp"

/**
 * Abstract SBML ODE System class.
 *
 * Sets up variables and functions for an ODE system imported from SBML.
 *
 * Instances can store event state internally in the mEvent* vectors - the
 * vectors may be empty if the model does not have any events defined.
 */
class AbstractSbmlOdeSystem : public AbstractOdeSystem
{
private:
    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive / unarchive the AbstractSbmlOdeSystem.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template <class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive& boost::serialization::base_object<AbstractOdeSystem>(*this);
        archive & mNumberOfParameters;
        archive & mNumberOfEvents;
        archive & mEventSatisfied;
        archive & mEventClampActive;
        archive & mEventTriggered;
        archive & mEventType;
        archive & mEventAdjustedStateVars;
        archive & mEventAdjustedStateValues;
        archive & mEventAdjustedStatePriority;
        archive & mEventAdjustedParameters;
        archive & mEventAdjustedParameterValues;
        archive & mEventAdjustedParameterPriority;
    }

    /**
     * Process the events in the model.
     *
     * @param time The current time
     * @param rY The current state variables
     *
     * @return How close we are to the time of the next event
     */
    virtual double ProcessModelEvents(double time, const std::vector<double>& rY) = 0;

    /**
     * Run the assignment rules to update state.
     *
     * @param time The current time
     * @param rY The current state variables
     */
    virtual void RunAssignmentRules(double time) = 0;

    /**
     * Run the initial assignments to set initial state.
     *
     * @param time The current time
     */
    virtual void RunInitialAssignments(double time) = 0;

    /**
     * Run the reactions to update state.
     *
     * @param time The current time
     * @param rY The current state variables
     */
    virtual void RunReactions(double time) = 0;

    /**
     * Update variable parameters from current ODE system parameter settings.
     *
     * @param time The current time
     */
    virtual void UpdateParameters(double time) = 0;

    /**
     * Update state variables from the given ODE system state.
     *
     * @param time The current time
     * @param rStateVariables The state variables to use
     */
    virtual void UpdateStateVariables(double time, const std::vector<double>& rStateVariables) = 0;

protected:
    /** The number of parameters in the model */
    unsigned mNumberOfParameters;

    /** The number of events in the model */
    unsigned mNumberOfEvents;

    // Event handling
    std::vector<bool> mEventSatisfied;
    /**
     * Per-event clamp flag used only by the CVODE root function. Frozen at the start of each
     * Solve segment (a snapshot of mEventSatisfied in CalculateStoppingEvent) and cleared
     * permanently the instant an event's trigger first goes false (the event re-arms). While
     * set, the event's root distance is forced large-negative so CVODE reports no spurious
     * root for a trigger already active at the initial condition. Being monotonic within a
     * segment (only ever cleared, never re-set) it stays constant across CVODE's in-step root
     * bracketing, so events localize at the true crossing instead of the step endpoint, while
     * an event that re-arms and re-fires within the same segment is still detected.
     */
    std::vector<bool> mEventClampActive;
    std::vector<bool> mEventTriggered;
    std::vector<SbmlEventType> mEventType;
    std::vector<bool> mEventAdjustedStateVars;
    std::vector<double> mEventAdjustedStateValues;
    std::vector<double> mEventAdjustedStatePriority;
    std::vector<bool> mEventAdjustedParameters;
    std::vector<double> mEventAdjustedParameterValues;
    std::vector<double> mEventAdjustedParameterPriority;

    /**
     * Run the equations governing the model to update state.
     *
     * @param time The current time
     * @param rY The current state variables
     */
    void RunModelRules(double time, const std::vector<double>& rY);

public:
    /**
     * Constructor.
     *
     * @param numberOfStateVariables The number of state variables in the model
     * @param numberOfParameters The number of parameters in the model
     * @param numberOfEvents The number of events in the model
     */
    AbstractSbmlOdeSystem(unsigned numberOfStateVariables, unsigned numberOfParameters, unsigned numberOfEvents);

    /**
     * Destructor.
     */
    virtual ~AbstractSbmlOdeSystem();

    /**
     * Adjust parameters and state variables after a stopping event
     *
     * @param time The current time
     */
    void AdjustParameters(double time);

    /**
     * Calculate whether the conditions to trigger an event have been met
     * (Used by CVODE solver to find exact stopping position)
     *
     * @param time The current time
     * @param rY The current state variables
     *
     * @return How close we are to the root of the stopping condition
     */
    double CalculateRootFunction(double time, const std::vector<double>& rY) override;

    /**
     * Calculate whether the conditions to trigger an event have been met
     *
     * @param time The current time
     * @param rY The current state variables
     *
     * @return True if conditions for an event are met, false otherwise
     */
    bool CalculateStoppingEvent(double time, const std::vector<double>& rY) override;

    /**
     * Check if a specific type of event has occurred.
     *
     * @param eventType The type of event to check
     *
     * @return True if the type of event has occurred, false otherwise
     */
    bool HasEventOccurred(SbmlEventType eventType);

    /**
     * Reset the flags that indicate which events have been triggered.
     */
    void ResetEventsOccurred();
};

// Register abstract class with Boost serialization
CLASS_IS_ABSTRACT(AbstractSbmlOdeSystem)

#endif // ABSTRACT_SBML_ODE_SYSTEM_HPP_
