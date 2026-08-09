#include "MyForce.hpp"

template <unsigned DIM>
MyForce<DIM>::MyForce(double forceMagnitude)
        : AbstractForce<DIM>(),
          mForceMagnitude(forceMagnitude)
{
}

template <unsigned DIM>
void MyForce<DIM>::AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation)
{
    // Build a constant force vector pointing in the positive x-direction.
    c_vector<double, DIM> force = zero_vector<double>(DIM);
    force[0] = mForceMagnitude;

    // Apply it to every node in the population.
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
         cell_iter != rCellPopulation.End();
         ++cell_iter)
    {
        unsigned node_index = rCellPopulation.GetLocationIndexUsingCell(*cell_iter);
        rCellPopulation.GetNode(node_index)->AddAppliedForceContribution(force);
    }
}

template <unsigned DIM>
double MyForce<DIM>::GetForceMagnitude() const
{
    return mForceMagnitude;
}

template <unsigned DIM>
void MyForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<ForceMagnitude>" << mForceMagnitude << "</ForceMagnitude>\n";

    // Call the method on the direct parent class.
    AbstractForce<DIM>::OutputForceParameters(rParamsFile);
}

// Explicit instantiation
template class MyForce<1>;
template class MyForce<2>;
template class MyForce<3>;

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(MyForce)
