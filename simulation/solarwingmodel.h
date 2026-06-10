#ifndef SIMULATION_SOLARWINGMODEL_H
#define SIMULATION_SOLARWINGMODEL_H

#include "simulationtypes.h"

namespace Simulation {

class SolarWingModel
{
public:
    SolarWingModel();

    void setParameters(const ModelParameters &parameters);
    const ModelParameters &parameters() const;

    double equivalentMass(const SimulationState &state) const;
    double gravityLoad(const SimulationState &state) const;
    double acceleration(const SimulationState &state, double tension) const;

private:
    ModelParameters m_parameters;
};

} // namespace Simulation

#endif // SIMULATION_SOLARWINGMODEL_H
