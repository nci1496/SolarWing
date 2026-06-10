#include "solarwingmodel.h"

#include <QtMath>

namespace Simulation {

SolarWingModel::SolarWingModel() = default;

void SolarWingModel::setParameters(const ModelParameters &parameters)
{
    m_parameters = parameters;
}

const ModelParameters &SolarWingModel::parameters() const
{
    return m_parameters;
}

double SolarWingModel::equivalentMass(const SimulationState &state) const
{
    const double deployedMass = m_parameters.massPerMeter * qMax(0.0, state.displacement);
    return m_parameters.hookMass + deployedMass;
}

double SolarWingModel::gravityLoad(const SimulationState &state) const
{
    const double angleRad = qDegreesToRadians(m_parameters.cableAngleDeg);
    return equivalentMass(state) * m_parameters.gravity * qCos(angleRad);
}

double SolarWingModel::acceleration(const SimulationState &state, double tension) const
{
    const double mass = qMax(1e-6, equivalentMass(state));
    const double dampingForce = m_parameters.damping * state.velocity;
    const double elasticForce = m_parameters.stiffness * state.displacement;
    const double netForce = tension - gravityLoad(state) - dampingForce - elasticForce;
    return netForce / mass;
}

} // namespace Simulation
