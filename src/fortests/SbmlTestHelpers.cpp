
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "AbstractOdeSystem.hpp"
#include "OdeSolution.hpp"
#include "OutputFileHandler.hpp"

#include "SbmlTestHelpers.hpp"

void sbmltesthelpers::ExportCsv(const std::string& rFilename,
                                OdeSolution& rOdeSolution,
                                AbstractOdeSystem& rOdeSystem,
                                const std::vector<std::vector<double> >* pParamsPerStep)
{
    OutputFileHandler handler("");
    out_stream file = handler.OpenOutputFile(rFilename);

    // Times
    const std::vector<double>& time_data = rOdeSolution.rGetTimes();

    // State variables
    const std::vector<std::string>& svar_names = rOdeSystem.rGetStateVariableNames();
    const std::vector<std::vector<double> >& svar_data = rOdeSolution.rGetSolutions();

    // Derived quantities
    rOdeSolution.CalculateDerivedQuantitiesAndParameters(&rOdeSystem);
    const std::vector<std::string>& dq_names = rOdeSystem.rGetDerivedQuantityNames();
    const std::vector<std::vector<double> >& dq_data = rOdeSolution.rGetDerivedQuantities(&rOdeSystem);

    // Parameters
    const std::vector<std::string>& param_names = rOdeSystem.rGetParameterNames();
    const std::vector<double>& param_data = rOdeSolution.rGetParameters(&rOdeSystem);

    // Sanity checks
    if (time_data.empty())
    {
        throw std::invalid_argument("OdeSolution contains no time points.");
    }

    if (svar_data.empty() && dq_data.empty())
    {
        throw std::invalid_argument("OdeSolution contains no state variables or derived quantities.");
    }

    if (!svar_data.empty() && (svar_data.size() != time_data.size()))
    {
        throw std::length_error("Number of state variable data rows do not match time points.");
    }

    if (!dq_data.empty() && (dq_data.size() != time_data.size()))
    {
        throw std::length_error("Number of derived quantity data rows do not match time points.");
    }

    if ((svar_data.empty() && !svar_names.empty()) || (!svar_data.empty() && svar_data[0].size() != svar_names.size()))
    {
        throw std::length_error("Number of state variable names do not match data.");
    }

    if ((dq_data.empty() && !dq_names.empty()) || (!dq_data.empty() && dq_data[0].size() != dq_names.size()))
    {
        throw std::length_error("Number of derived quantity names do not match data.");
    }

    if ((param_data.empty() && !param_names.empty()) || (!param_data.empty() && param_data.size() != param_names.size()))
    {
        throw std::length_error("Number of parameter names do not match data.");
    }

    // Write column headings
    (*file) << "time";
    if (!svar_names.empty())
    {
        for (unsigned i = 0; i < svar_names.size(); i++)
        {
            (*file) << "," << svar_names[i];
        }
    }
    if (!dq_names.empty())
    {
        for (unsigned i = 0; i < dq_names.size(); i++)
        {
            (*file) << "," << dq_names[i];
        }
    }
    if (!param_names.empty())
    {
        for (unsigned i = 0; i < param_names.size(); i++)
        {
            (*file) << "," << param_names[i];
        }
    }
    (*file) << std::endl
            << std::flush;

    // Write data
    for (unsigned i = 0; i < time_data.size(); i++)
    {
        (*file) << time_data[i];
        if (!svar_data.empty())
        {
            for (unsigned j = 0; j < svar_data[i].size(); j++)
            {
                (*file) << "," << svar_data[i][j];
            }
        }
        if (!dq_data.empty())
        {
            for (unsigned j = 0; j < dq_data[i].size(); j++)
            {
                (*file) << "," << dq_data[i][j];
            }
        }
        if (pParamsPerStep != nullptr && i < pParamsPerStep->size())
        {
            // Time-resolved parameters (e.g. a parameter changed by an event).
            for (unsigned j = 0; j < (*pParamsPerStep)[i].size(); j++)
            {
                (*file) << "," << (*pParamsPerStep)[i][j];
            }
        }
        else if (!param_data.empty())
        {
            for (unsigned j = 0; j < param_data.size(); j++)
            {
                (*file) << "," << param_data[j];
            }
        }
        (*file) << std::endl
                << std::flush;
    }
    file->close();
}

std::string sbmltesthelpers::ToString(double value, unsigned precision)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}
