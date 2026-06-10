#ifndef SIMULATION_SIMULATIONTYPES_H
#define SIMULATION_SIMULATIONTYPES_H

#include <QString>
#include <QVector>

namespace Simulation {

enum class IntegratorType {
    Euler = 0,
    RK2,
    RK4
};

enum class TargetType {
    Constant = 0,
    Quadratic
};

struct ModelParameters {
    double massPerMeter = 2.0;
    double gravity = 9.8;
    double hookMass = 12.0;
    double cableAngleDeg = 0.02;
    double damping = 2.0;
    double stiffness = 8.0;
};

struct PIDParameters {
    double kp = 1.0;
    double ki = 0.0;
    double kd = 0.0;
    double outputMin = 0.0;
    double outputMax = 5000.0;
};

struct TargetParameters {
    TargetType type = TargetType::Constant;
    double constantTarget = 1.0;
    double quadA = 0.02;
    double quadB = 0.03;

    double valueAt(double time) const;
};

struct SimulationParameters {
    ModelParameters model;
    PIDParameters pid;
    TargetParameters target;
    IntegratorType integrator = IntegratorType::RK4;
    double totalTime = 10.0;
    double timeStep = 0.01;
    double initialDisplacement = 0.0;
    double initialVelocity = 0.0;
};

struct SimulationState {
    double displacement = 0.0;
    double velocity = 0.0;
};

struct SimulationSample {
    double time = 0.0;
    double targetDisplacement = 0.0;
    double actualDisplacement = 0.0;
    double error = 0.0;
    double tension = 0.0;
    double velocity = 0.0;
};

struct SimulationResult {
    QVector<SimulationSample> samples;

    bool isEmpty() const { return samples.isEmpty(); }
    int size() const { return samples.size(); }
};

QString integratorTypeToString(IntegratorType type);

} // namespace Simulation

#endif // SIMULATION_SIMULATIONTYPES_H
