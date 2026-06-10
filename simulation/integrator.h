#ifndef SIMULATION_INTEGRATOR_H
#define SIMULATION_INTEGRATOR_H

#include "simulationtypes.h"
#include "solarwingmodel.h"

namespace Simulation {

class Integrator
{
public:
    static SimulationState step(IntegratorType type,
                                const SolarWingModel &model,
                                const SimulationState &current,
                                double tension,
                                double dt);

private:
    static SimulationState stepEuler(const SolarWingModel &model,
                                     const SimulationState &current,
                                     double tension,
                                     double dt);
    static SimulationState stepRK2(const SolarWingModel &model,
                                   const SimulationState &current,
                                   double tension,
                                   double dt);
    static SimulationState stepRK4(const SolarWingModel &model,
                                   const SimulationState &current,
                                   double tension,
                                   double dt);
};

} // namespace Simulation

#endif // SIMULATION_INTEGRATOR_H
