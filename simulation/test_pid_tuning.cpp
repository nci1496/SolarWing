#include "test_pid_tuning.h"

#include "solarwingsimulator.h"

#include <QDebug>
#include <QtMath>

namespace Simulation {
namespace Test {

namespace {

SimulationParameters createBaseParameters()
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

    params.pid.outputMin = 0.0;
    params.pid.outputMax = 5000.0;
    return params;
}

void printSummary(const QString &label,
                  const SimulationParameters &params,
                  const SimulationResult &result)
{
    qDebug().noquote() << QString("[%1] samples = %2")
                          .arg(label)
                          .arg(result.samples.size());

    if (result.samples.isEmpty()) {
        qDebug().noquote() << QString("[%1] result is empty").arg(label);
        return;
    }

    const SimulationSample &first = result.samples.first();
    const SimulationSample &last = result.samples.last();

    double maxAbsError = 0.0;
    for (const SimulationSample &sample : result.samples) {
        maxAbsError = qMax(maxAbsError, qAbs(sample.error));
    }

    qDebug().noquote() << QString("[%1] target type = %2, integrator = %3")
                          .arg(label)
                          .arg(params.target.type == TargetType::Constant ? "Constant" : "Quadratic")
                          .arg(integratorTypeToString(params.integrator));

    qDebug().noquote() << QString("[%1] PID = (kp=%2, ki=%3, kd=%4)")
                          .arg(label)
                          .arg(params.pid.kp, 0, 'f', 2)
                          .arg(params.pid.ki, 0, 'f', 2)
                          .arg(params.pid.kd, 0, 'f', 2);

    qDebug().noquote() << QString("[%1] first: t=%2 target=%3 actual=%4 error=%5 tension=%6 velocity=%7")
                          .arg(label)
                          .arg(first.time, 0, 'f', 2)
                          .arg(first.targetDisplacement, 0, 'f', 4)
                          .arg(first.actualDisplacement, 0, 'f', 4)
                          .arg(first.error, 0, 'f', 4)
                          .arg(first.tension, 0, 'f', 4)
                          .arg(first.velocity, 0, 'f', 4);

    qDebug().noquote() << QString("[%1] last: t=%2 target=%3 actual=%4 error=%5 tension=%6 velocity=%7")
                          .arg(label)
                          .arg(last.time, 0, 'f', 2)
                          .arg(last.targetDisplacement, 0, 'f', 4)
                          .arg(last.actualDisplacement, 0, 'f', 4)
                          .arg(last.error, 0, 'f', 4)
                          .arg(last.tension, 0, 'f', 4)
                          .arg(last.velocity, 0, 'f', 4);

    qDebug().noquote() << QString("[%1] metrics: finalAbsError=%2, maxAbsError=%3, finalSpeed=%4")
                          .arg(label)
                          .arg(qAbs(last.error), 0, 'f', 4)
                          .arg(maxAbsError, 0, 'f', 4)
                          .arg(qAbs(last.velocity), 0, 'f', 4);
}

void runSingleCase(const QString &label, const SimulationParameters &params)
{
    SolarWingSimulator simulator;
    simulator.setParameters(params);
    const SimulationResult result = simulator.run();
    printSummary(label, params, result);
}

} // namespace

void runPidTuningTests()
{
    qDebug() << "========== PID tuning tests begin ==========";

    SimulationParameters constantRk4 = createBaseParameters();
    constantRk4.target.type = TargetType::Constant;
    constantRk4.target.constantTarget = 1.0;
    constantRk4.pid.kp = 300.0;
    constantRk4.pid.ki = 20.0;
    constantRk4.pid.kd = 40.0;
    runSingleCase("pid-constant-rk4", constantRk4);

    SimulationParameters constantRk2 = constantRk4;
    constantRk2.integrator = IntegratorType::RK2;
    runSingleCase("pid-constant-rk2", constantRk2);

    SimulationParameters constantAggressive = constantRk4;
    constantAggressive.pid.kp = 420.0;
    constantAggressive.pid.ki = 35.0;
    constantAggressive.pid.kd = 55.0;
    runSingleCase("pid-constant-aggressive", constantAggressive);

    SimulationParameters quadraticRk4 = createBaseParameters();
    quadraticRk4.target.type = TargetType::Quadratic;
    quadraticRk4.target.quadA = 0.02;
    quadraticRk4.target.quadB = 0.03;
    quadraticRk4.pid.kp = 320.0;
    quadraticRk4.pid.ki = 25.0;
    quadraticRk4.pid.kd = 45.0;
    runSingleCase("pid-quadratic-rk4", quadraticRk4);

    qDebug() << "========== PID tuning tests end ==========";
}

} // namespace Test
} // namespace Simulation
