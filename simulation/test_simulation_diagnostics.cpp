#include "test_simulation_diagnostics.h"

#include "solarwingmodel.h"
#include "solarwingsimulator.h"

#include <QDebug>
#include <QList>
#include <QSet>
#include <QtMath>

namespace Simulation {
namespace Test {

namespace {

SimulationParameters createQuadraticDiagnosticParameters()
{
    SimulationParameters params;
    params.totalTime = 4.0;
    params.timeStep = 0.01;
    params.integrator = IntegratorType::RK4;

    params.model.massPerMeter = 2.0;
    params.model.hookMass = 12.0;
    params.model.gravity = 9.8;
    params.model.cableAngleDeg = 0.02;
    params.model.damping = 1.0;
    params.model.stiffness = 2.0;

    params.pid.kp = 320.0;
    params.pid.ki = 25.0;
    params.pid.kd = 45.0;
    params.pid.outputMin = 0.0;
    params.pid.outputMax = 5000.0;

    params.target.type = TargetType::Quadratic;
    params.target.quadA = 0.02;
    params.target.quadB = 0.03;
    return params;
}

double computeAcceleration(const SimulationParameters &params,
                           const SimulationSample &sample)
{
    SolarWingModel model;
    model.setParameters(params.model);

    SimulationState state;
    state.displacement = sample.actualDisplacement;
    state.velocity = sample.velocity;
    return model.acceleration(state, sample.tension);
}

void printSnapshot(const QString &scenario,
                   const SimulationParameters &params,
                   const SimulationSample &sample)
{
    const double acceleration = computeAcceleration(params, sample);
    qDebug().noquote() << QString("[diagnostic][%1] t=%2")
                          .arg(scenario)
                          .arg(sample.time, 0, 'f', 2);
    qDebug().noquote() << QString("  target=%1").arg(sample.targetDisplacement, 0, 'f', 6);
    qDebug().noquote() << QString("  actual=%1").arg(sample.actualDisplacement, 0, 'f', 6);
    qDebug().noquote() << QString("  velocity=%1").arg(sample.velocity, 0, 'f', 6);
    qDebug().noquote() << QString("  acceleration=%1").arg(acceleration, 0, 'f', 6);
    qDebug().noquote() << QString("  tension=%1").arg(sample.tension, 0, 'f', 6);
    qDebug().noquote() << QString("  error=%1").arg(sample.error, 0, 'f', 6);
}

void printConclusion(const SimulationResult &result)
{
    if (result.samples.isEmpty()) {
        qDebug().noquote() << "[diagnostic][quadratic] conclusion=D";
        qDebug().noquote() << "  basis=result is empty, no evidence available";
        return;
    }

    const SimulationSample &first = result.samples.first();
    const SimulationSample &last = result.samples.last();
    const bool targetMonotonic = last.targetDisplacement > first.targetDisplacement;
    const bool actualPinnedNearZero = qAbs(last.actualDisplacement) < 1e-6;
    const bool finalVelocityNegative = last.velocity < -1.0;
    const bool tensionMostlyPositive = last.tension >= 0.0;

    if (targetMonotonic && actualPinnedNearZero && finalVelocityNegative && tensionMostlyPositive) {
        qDebug().noquote() << "[diagnostic][quadratic] conclusion=C";
        qDebug().noquote() << "  basis=target grows normally, PID output remains non-negative, but displacement is clamped at 0 while velocity stays negative; this points to boundary handling amplifying the anomaly.";
        return;
    }

    qDebug().noquote() << "[diagnostic][quadratic] conclusion=D";
    qDebug().noquote() << "  basis=quadratic target path or output trend does not match the expected boundary-clamp symptom; inspect PID output next.";
}

} // namespace

void runQuadraticDiagnosticTest()
{
    qDebug() << "========== quadratic diagnostic begin ==========";

    const SimulationParameters params = createQuadraticDiagnosticParameters();
    SolarWingSimulator simulator;
    simulator.setParameters(params);
    const SimulationResult result = simulator.run();

    if (result.samples.isEmpty()) {
        qDebug().noquote() << "[diagnostic][quadratic] no samples generated";
        qDebug() << "========== quadratic diagnostic end ==========";
        return;
    }

    const QList<double> keyTimes = {0.0, 0.1, 0.5, 1.0, 2.0, 4.0};
    QSet<int> printedIndexes;
    for (double keyTime : keyTimes) {
        const int index = qBound(0,
                                 qRound(keyTime / params.timeStep),
                                 result.samples.size() - 1);
        if (printedIndexes.contains(index)) {
            continue;
        }
        printedIndexes.insert(index);
        printSnapshot("quadratic", params, result.samples.at(index));
    }

    printConclusion(result);
    qDebug() << "========== quadratic diagnostic end ==========";
}

} // namespace Test
} // namespace Simulation
