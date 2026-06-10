#ifndef SIMULATION_PIDCONTROLLER_H
#define SIMULATION_PIDCONTROLLER_H

#include "simulationtypes.h"

namespace Simulation {

class PIDController
{
public:
    PIDController();

    void setParameters(const PIDParameters &parameters);
    const PIDParameters &parameters() const;

    void reset();
    double update(double error, double dt);

    double integralTerm() const;
    double previousError() const;

private:
    PIDParameters m_parameters;
    double m_integral;
    double m_previousError;
    bool m_hasPreviousError;
};

} // namespace Simulation

#endif // SIMULATION_PIDCONTROLLER_H
