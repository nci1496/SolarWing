#ifndef SIMULATION_SOLARWINGSIMULATOR_H
#define SIMULATION_SOLARWINGSIMULATOR_H

#include "integrator.h"
#include "pidcontroller.h"
#include "solarwingmodel.h"

namespace Simulation {

class SolarWingSimulator
{
public:
    SolarWingSimulator();

    void setParameters(const SimulationParameters &parameters);
    const SimulationParameters &parameters() const;

    void reset();
    SimulationSample step();
    SimulationResult run();

    bool hasFinished() const;
    double currentTime() const;
    const SimulationState &currentState() const;

private:
    SimulationSample buildSample(double time, double target, double tension) const;

    SimulationParameters m_parameters;
    SolarWingModel m_model;
    PIDController m_pid;
    SimulationState m_state;
    double m_time;
};

} // namespace Simulation

#endif // SIMULATION_SOLARWINGSIMULATOR_H
