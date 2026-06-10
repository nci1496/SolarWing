#include "solarwingsimulator.h"

#include <QtMath>

namespace Simulation {

SolarWingSimulator::SolarWingSimulator()
    : m_time(0.0)
{
    setParameters(SimulationParameters());
    reset();
}

void SolarWingSimulator::setParameters(const SimulationParameters &parameters)
{
    m_parameters = parameters;
    m_model.setParameters(parameters.model);
    m_pid.setParameters(parameters.pid);
}

const SimulationParameters &SolarWingSimulator::parameters() const
{
    return m_parameters;
}

void SolarWingSimulator::reset()
{
    m_time = 0.0;
    m_state.displacement = m_parameters.initialDisplacement;
    m_state.velocity = m_parameters.initialVelocity;
    m_pid.reset();
}

SimulationSample SolarWingSimulator::step()
{
    const double target = m_parameters.target.valueAt(m_time);
    const double error = target - m_state.displacement;
    const double tension = m_pid.update(error, m_parameters.timeStep);

    m_state = Integrator::step(m_parameters.integrator,
                               m_model,
                               m_state,
                               tension,
                               m_parameters.timeStep);

    if (m_state.displacement < 0.0) {
        m_state.displacement = 0.0;
        m_state.velocity = qMax(0.0, m_state.velocity);
    }

    m_time += m_parameters.timeStep;
    return buildSample(m_time, m_parameters.target.valueAt(m_time), tension);
}

SimulationResult SolarWingSimulator::run()
{
    reset();

    SimulationResult result;
    result.samples.reserve(static_cast<int>(m_parameters.totalTime / m_parameters.timeStep) + 1);

    result.samples.append(buildSample(m_time,
                                      m_parameters.target.valueAt(m_time),
                                      0.0));

    while (!hasFinished()) {
        result.samples.append(step());
    }

    return result;
}

bool SolarWingSimulator::hasFinished() const
{
    return m_time >= m_parameters.totalTime;
}

double SolarWingSimulator::currentTime() const
{
    return m_time;
}

const SimulationState &SolarWingSimulator::currentState() const
{
    return m_state;
}

SimulationSample SolarWingSimulator::buildSample(double time, double target, double tension) const
{
    SimulationSample sample;
    sample.time = time;
    sample.targetDisplacement = target;
    sample.actualDisplacement = m_state.displacement;
    sample.error = target - m_state.displacement;
    sample.tension = tension;
    sample.velocity = m_state.velocity;
    return sample;
}

} // namespace Simulation
