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

#include "SbmlTestOdeSolution.hpp"

void SbmlTestOdeSolution::RecordPoint(double time, const std::vector<double>& rY, AbstractOdeSystem* pSystem)
{
    if (rGetTimes().empty())
    {
        SetOdeSystemInformation(pSystem->GetSystemInformation());
    }

    // Snapshot the system's current parameters for this point.
    std::vector<double> params(pSystem->GetNumberOfParameters());
    for (unsigned i = 0; i < params.size(); ++i)
    {
        params[i] = pSystem->GetParameter(i);
    }

    rGetTimes().push_back(time);
    rGetSolutions().push_back(rY);
    mParametersPerStep.push_back(params);

    SetNumberOfTimeSteps(rGetTimes().size());
}

std::vector<double> SbmlTestOdeSolution::GetParameterSeries(const std::string& rName, AbstractOdeSystem* pSystem) const
{
    unsigned index = pSystem->GetParameterIndex(rName);
    std::vector<double> series(mParametersPerStep.size());
    for (unsigned i = 0; i < series.size(); ++i)
    {
        series[i] = mParametersPerStep[i][index];
    }
    return series;
}

std::vector<double> SbmlTestOdeSolution::GetDerivedQuantitySeries(const std::string& rName, AbstractOdeSystem* pSystem) const
{
    unsigned index = pSystem->GetSystemInformation()->GetDerivedQuantityIndex(rName);

    // Save the system's current parameters so they can be restored afterwards.
    std::vector<double> saved(pSystem->GetNumberOfParameters());
    for (unsigned p = 0; p < saved.size(); ++p)
    {
        saved[p] = pSystem->GetParameter(p);
    }

    std::vector<double> series(rGetTimes().size());
    for (unsigned i = 0; i < series.size(); ++i)
    {
        // Restore this step's parameters so parameter-dependent derived quantities are time-resolved.
        for (unsigned p = 0; p < mParametersPerStep[i].size(); ++p)
        {
            pSystem->SetParameter(p, mParametersPerStep[i][p]);
        }
        series[i] = pSystem->ComputeDerivedQuantities(rGetTimes()[i], rGetSolutions()[i])[index];
    }

    for (unsigned p = 0; p < saved.size(); ++p)
    {
        pSystem->SetParameter(p, saved[p]);
    }
    return series;
}
