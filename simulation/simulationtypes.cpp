#include "simulationtypes.h"

namespace Simulation {

double TargetParameters::valueAt(double time) const
{
    if (type == TargetType::Quadratic) {
        return quadA * time * time + quadB * time;
    }
    return constantTarget;
}

QString integratorTypeToString(IntegratorType type)
{
    switch (type) {
    case IntegratorType::Euler:
        return QStringLiteral("Euler");
    case IntegratorType::RK2:
        return QStringLiteral("RK2");
    case IntegratorType::RK4:
        return QStringLiteral("RK4");
    }
    return QStringLiteral("Unknown");
}

} // namespace Simulation
