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

#ifndef SBML_TEST_ODE_SOLUTION_HPP_
#define SBML_TEST_ODE_SOLUTION_HPP_

#include <string>
#include <vector>

#include "AbstractOdeSystem.hpp"
#include "OdeSolution.hpp"

/**
 * An OdeSolution that additionally records the model parameters at each time step.
 *
 * Chaste's OdeSolution stores only a single parameter snapshot - GetVariableAtIndex
 * returns mParameters[i] with no time index, and the header notes it "assumes that
 * mParameters is constant through time". A parameter changed by an event (e.g. a
 * compartment resized by an event assignment) is therefore reported with its final
 * value at every time step.
 *
 * An SBML model is solved one sample-grid point at a time, stopping at each event; the
 * parameters are constant between events and change (via AdjustParameters) at them.
 * RecordPoint stores the system's current parameters alongside each grid point, so
 * GetParameterSeries can return a genuinely time-resolved series.
 */
class SbmlTestOdeSolution : public OdeSolution
{
private:
    /** Parameter values for each stored time step. */
    std::vector<std::vector<double> > mParametersPerStep;

public:
    /**
     * Append a single solution point, recording the system's current parameter values for it.
     * Building the solution one grid point at a time (rather than one segment at a time) keeps
     * the output on the requested sample grid even when an event fires between grid points.
     *
     * @param time the time of the point
     * @param rY the state variables at this point
     * @param pSystem the ODE system, queried for its current parameter values
     */
    void RecordPoint(double time, const std::vector<double>& rY, AbstractOdeSystem* pSystem);

    /**
     * @param rName the parameter name
     * @param pSystem the ODE system, used to resolve the parameter's index
     * @return the parameter's value at each stored time step
     */
    std::vector<double> GetParameterSeries(const std::string& rName, AbstractOdeSystem* pSystem) const;

    /**
     * Evaluate a derived quantity at every stored step with that step's recorded parameters
     * restored first. A derived quantity that depends on an event-modified parameter (e.g. the
     * amount conversion amt__X = X * compartment for a boundary species X changed by an event)
     * is otherwise computed with the parameter's final value at every point, losing the time
     * resolution. Restoring the per-step parameters fixes this.
     *
     * @param rName the derived quantity name
     * @param pSystem the ODE system, used to compute the derived quantities
     * @return the derived quantity's value at each stored time step
     */
    std::vector<double> GetDerivedQuantitySeries(const std::string& rName, AbstractOdeSystem* pSystem) const;

    /** @return the parameter values recorded for each stored time step. */
    const std::vector<std::vector<double> >& rGetParametersPerStep() const
    {
        return mParametersPerStep;
    }
};

#endif // SBML_TEST_ODE_SOLUTION_HPP_
