#include "integrator.h"

namespace Simulation {

namespace {

SimulationState derivative(const SolarWingModel &model,
                           const SimulationState &state,
                           double tension)
{
    SimulationState change;
    change.displacement = state.velocity;
    change.velocity = model.acceleration(state, tension);
    return change;
}

SimulationState addScaled(const SimulationState &base,
                          const SimulationState &delta,
                          double scale)
{
    SimulationState result;
    result.displacement = base.displacement + delta.displacement * scale;
    result.velocity = base.velocity + delta.velocity * scale;
    return result;
}

} // namespace

SimulationState Integrator::step(IntegratorType type,
                                 const SolarWingModel &model,
                                 const SimulationState &current,
                                 double tension,
                                 double dt)
{
    switch (type) {
    case IntegratorType::Euler:
        return stepEuler(model, current, tension, dt);
    case IntegratorType::RK2:
        return stepRK2(model, current, tension, dt);
    case IntegratorType::RK4:
        return stepRK4(model, current, tension, dt);
    }

    return stepRK4(model, current, tension, dt);
}

SimulationState Integrator::stepEuler(const SolarWingModel &model,
                                      const SimulationState &current,
                                      double tension,
                                      double dt)
{
    const SimulationState k1 = derivative(model, current, tension);
    return addScaled(current, k1, dt);
}

SimulationState Integrator::stepRK2(const SolarWingModel &model,
                                    const SimulationState &current,
                                    double tension,
                                    double dt)
{
    const SimulationState k1 = derivative(model, current, tension);
    const SimulationState mid = addScaled(current, k1, dt * 0.5);
    const SimulationState k2 = derivative(model, mid, tension);
    return addScaled(current, k2, dt);
}

SimulationState Integrator::stepRK4(const SolarWingModel &model,
                                    const SimulationState &current,
                                    double tension,
                                    double dt)
{
    const SimulationState k1 = derivative(model, current, tension);
    const SimulationState s2 = addScaled(current, k1, dt * 0.5);
    const SimulationState k2 = derivative(model, s2, tension);
    const SimulationState s3 = addScaled(current, k2, dt * 0.5);
    const SimulationState k3 = derivative(model, s3, tension);
    const SimulationState s4 = addScaled(current, k3, dt);
    const SimulationState k4 = derivative(model, s4, tension);

    SimulationState result;
    result.displacement = current.displacement + dt * (k1.displacement + 2.0 * k2.displacement + 2.0 * k3.displacement + k4.displacement) / 6.0;
    result.velocity = current.velocity + dt * (k1.velocity + 2.0 * k2.velocity + 2.0 * k3.velocity + k4.velocity) / 6.0;
    return result;
}

} // namespace Simulation
