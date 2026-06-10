#include "pidcontroller.h"

#include <QtGlobal>

namespace Simulation {

PIDController::PIDController()
    : m_integral(0.0)
    , m_previousError(0.0)
    , m_hasPreviousError(false)
{
}

void PIDController::setParameters(const PIDParameters &parameters)
{
    m_parameters = parameters;
}

const PIDParameters &PIDController::parameters() const
{
    return m_parameters;
}

void PIDController::reset()
{
    m_integral = 0.0;
    m_previousError = 0.0;
    m_hasPreviousError = false;
}

double PIDController::update(double error, double dt)
{
    if (dt <= 0.0) {
        return 0.0;
    }

    m_integral += error * dt;
    const double derivative = m_hasPreviousError ? (error - m_previousError) / dt : 0.0;

    double output = m_parameters.kp * error
                    + m_parameters.ki * m_integral
                    + m_parameters.kd * derivative;

    output = qBound(m_parameters.outputMin, output, m_parameters.outputMax);

    m_previousError = error;
    m_hasPreviousError = true;
    return output;
}

double PIDController::integralTerm() const
{
    return m_integral;
}

double PIDController::previousError() const
{
    return m_previousError;
}

} // namespace Simulation
