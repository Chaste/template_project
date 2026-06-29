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

#ifndef SBML_TEST_HELPERS_HPP_
#define SBML_TEST_HELPERS_HPP_

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "AbstractOdeSystem.hpp"
#include "OdeSolution.hpp"

namespace sbmltesthelpers
{
/** Export results to a CSV file.
 *
 * The first column is time, followed by the state variables, derived quantities and
 * parameters.
 *
 * @param rFilename The name of the file to create.
 * @param rOdeSolution The OdeSolution containing the results.
 * @param rOdeSystem The ODE system (for variable names and derived quantities).
 * @param pParamsPerStep Optional time-resolved parameter values (one row per time step,
 *        as recorded by SbmlTestOdeSolution). When null the parameters are written from the
 *        system's single current snapshot, repeated on every row.
 */
void ExportCsv(const std::string& rFilename,
               OdeSolution& rOdeSolution,
               AbstractOdeSystem& rOdeSystem,
               const std::vector<std::vector<double> >* pParamsPerStep = nullptr);

/** Calculate the maximum of a vector of doubles.
 * @param vec The vector of doubles.
 * @return The maximum value.
 */
inline double Max(const std::vector<double>& vec);

/** Calculate the mean of a vector of doubles.
 * @param vec The vector of doubles.
 * @return The mean value.
 */
inline double Mean(const std::vector<double>& vec);

/** Calculate the minimum of a vector of doubles.
 * @param vec The vector of doubles.
 * @return The minimum value.
 */
inline double Min(const std::vector<double>& vec);

/** Calculate the standard deviation of a vector of doubles.
 * @param vec The vector of doubles.
 * @return The standard deviation value.
 */
inline double Stdev(const std::vector<double>& vec);

/** Convert a double to a string.
 * @param value The double value.
 * @return The string representation.
 */
std::string ToString(double value, unsigned precision = 9);

/** Calculate the qth quantile of a vector of doubles.
 * @param vec The vector of doubles, assumed to be sorted.
 * @param q The quantile to calculate (between 0 and 1).
 * @return The qth quantile value.
 */
inline double Quantile(const std::vector<double>& vec, double q);

/** Calculate the variance of a vector of doubles.
 * @param vec The vector of doubles.
 * @return The variance value.
 */
inline double Variance(const std::vector<double>& vec);
} // namespace sbmltesthelpers

// max
inline double sbmltesthelpers::Max(const std::vector<double>& vec)
{
    if (vec.empty())
    {
        throw std::invalid_argument("Cannot calculate maximum of an empty vector.");
    }
    return *std::max_element(vec.begin(), vec.end());
}

// mean
inline double sbmltesthelpers::Mean(const std::vector<double>& vec)
{
    if (vec.empty())
    {
        throw std::invalid_argument("Cannot calculate mean of an empty vector.");
    }
    return std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
}

// min
inline double sbmltesthelpers::Min(const std::vector<double>& vec)
{
    if (vec.empty())
    {
        throw std::invalid_argument("Cannot calculate minimum of an empty vector.");
    }
    return *std::min_element(vec.begin(), vec.end());
}

// stdev
inline double sbmltesthelpers::Stdev(const std::vector<double>& vec)
{
    return std::sqrt(sbmltesthelpers::Variance(vec));
}

// quantile
inline double sbmltesthelpers::Quantile(const std::vector<double>& vec, double q)
{
    if (vec.empty())
    {
        throw std::invalid_argument("Cannot calculate quantile of an empty vector.");
    }

    if (q < 0.0 || q > 1.0)
    {
        throw std::out_of_range("Quantile must be between 0.0 and 1.0");
    }

    std::vector<double> sorted_vec = vec;
    std::sort(sorted_vec.begin(), sorted_vec.end());

    size_t n = sorted_vec.size();
    size_t index = static_cast<size_t>(q * (n - 1));

    // Even number of elements
    if (n % 2 == 0)
    {
        return (sorted_vec[index] + sorted_vec[index - 1]) / 2.0;
    }

    // Odd number of elements
    return sorted_vec[index];
}

// variance
inline double sbmltesthelpers::Variance(const std::vector<double>& vec)
{
    if (vec.size() < 2)
    {
        throw std::invalid_argument("Variance requires at least two data points.");
    }
    double mean_val = sbmltesthelpers::Mean(vec);
    double accum = 0.0;
    for (double val : vec)
    {
        accum += (val - mean_val) * (val - mean_val);
    }
    return accum / (vec.size() - 1);
}

#endif // SBML_TEST_HELPERS_HPP_
