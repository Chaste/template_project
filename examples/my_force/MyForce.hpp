#ifndef MYFORCE_HPP_
#define MYFORCE_HPP_

#include "AbstractCellPopulation.hpp"
#include "AbstractForce.hpp"

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

/**
 * An example user-defined force for a Chaste cell-based simulation.
 *
 * It applies a constant force of a given magnitude, in the positive x-direction, to every
 * node in the cell population. This is deliberately simple: it exists to show how to add a
 * new C++ class to a user project and then drive it from Python via the project bindings.
 */
template <unsigned DIM>
class MyForce : public AbstractForce<DIM>
{
private:
    /** The magnitude of the constant force applied to each node. */
    double mForceMagnitude;

    /** Needed for serialization. */
    friend class boost::serialization::access;

    /**
     * Archive the object and its member variables.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template <class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive& boost::serialization::base_object<AbstractForce<DIM> >(*this);
        archive& mForceMagnitude;
    }

public:
    /**
     * Constructor.
     *
     * @param forceMagnitude the magnitude of the constant force (defaults to 1.0)
     */
    MyForce(double forceMagnitude = 1.0);

    /**
     * Overridden AddForceContribution() method.
     *
     * @param rCellPopulation reference to the cell population
     */
    void AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation) override;

    /**
     * @return the magnitude of the applied force.
     */
    double GetForceMagnitude() const;

    /**
     * Overridden OutputForceParameters() method.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputForceParameters(out_stream& rParamsFile) override;
};

#endif /*MYFORCE_HPP_*/
