#include "test_simulation.h"

#include "solarwingsimulator.h"

#include <QDebug>

namespace Simulation {
namespace Test {

void runSimulationSmokeTest()
{
    SolarWingSimulator simulator;

    SimulationParameters params;
    params.totalTime = 2.0;
    params.timeStep = 0.01;
    params.integrator = IntegratorType::RK4;
    params.target.type = TargetType::Constant;
    params.target.constantTarget = 1.0;

    params.pid.kp = 300.0;
    params.pid.ki = 20.0;
    params.pid.kd = 40.0;
    params.pid.outputMin = 0.0;
    params.pid.outputMax = 5000.0;

    params.model.massPerMeter = 2.0;
    params.model.hookMass = 12.0;
    params.model.gravity = 9.8;
    params.model.damping = 1.0;
    params.model.stiffness = 2.0;

    simulator.setParameters(params);
    const SimulationResult result = simulator.run();

    qDebug() << "[simulation test] sample count =" << result.samples.size();

    if (result.samples.isEmpty()) {
        qDebug() << "[simulation test] result is empty";
        return;
    }

    const SimulationSample &first = result.samples.first();
    const SimulationSample &last = result.samples.last();

    qDebug() << "[simulation test] first:"
             << "t =" << first.time
             << "target =" << first.targetDisplacement
             << "actual =" << first.actualDisplacement
             << "error =" << first.error
             << "tension =" << first.tension
             << "velocity =" << first.velocity;

    qDebug() << "[simulation test] last:"
             << "t =" << last.time
             << "target =" << last.targetDisplacement
             << "actual =" << last.actualDisplacement
             << "error =" << last.error
             << "tension =" << last.tension
             << "velocity =" << last.velocity;
}

} // namespace Test
} // namespace Simulation
