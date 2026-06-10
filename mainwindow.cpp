#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogs/basicsettingsdialog.h"
#include "simulation/solarwingsimulator.h"
#include "simulation/test_pid_tuning.h"
#include "simulation/test_simulation.h"
#include "simulation/test_simulation_diagnostics.h"
#include "widgets/animationwidget.h"
#include "widgets/plotwidget.h"

#include <QAction>
#include <QColor>
#include <QMenu>
#include <QMessageBox>
#include <QStatusBar>

namespace {

Simulation::IntegratorType integratorTypeFromIndex(int index)
{
    switch (index) {
    case 0:
        return Simulation::IntegratorType::Euler;
    case 1:
        return Simulation::IntegratorType::RK2;
    case 2:
    default:
        return Simulation::IntegratorType::RK4;
    }
}

SimulationParams dialogParamsFromSimulation(const Simulation::SimulationParameters &params)
{
    SimulationParams dialogParams;
    dialogParams.massPerMeter = params.model.massPerMeter;
    dialogParams.gravity = params.model.gravity;
    dialogParams.hookMass = params.model.hookMass;
    dialogParams.cableAngle = params.model.cableAngleDeg;
    dialogParams.kp = params.pid.kp;
    dialogParams.ki = params.pid.ki;
    dialogParams.kd = params.pid.kd;
    dialogParams.targetType = params.target.type == Simulation::TargetType::Quadratic
                                  ? SimulationParams::TargetQuadratic
                                  : SimulationParams::TargetConstant;
    dialogParams.targetConstant = params.target.constantTarget;
    dialogParams.quadCoefA = params.target.quadA;
    dialogParams.quadCoefB = params.target.quadB;
    dialogParams.simDuration = params.totalTime;
    dialogParams.simStep = params.timeStep;
    dialogParams.maxDisplacement = params.maxDisplacement;
    return dialogParams;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_basicSettingsDialog(nullptr)
    , m_paramSettingsDialog(nullptr)
    , m_basicSettingsAction(nullptr)
    , m_paramSettingsAction(nullptr)
    , m_startAction(nullptr)
    , m_pauseAction(nullptr)
    , m_stopAction(nullptr)
    , m_forcePlot(nullptr)
    , m_errorPlot(nullptr)
    , m_displacementPlot(nullptr)
    , m_animationWidget(nullptr)
    , m_currentSimulationParameters(createDefaultPlotSimulationParameters())
    , m_stopHookTriggered(false)
{
    ui->setupUi(this);
    ui->statusbar->showMessage(tr("系统就绪"));
    ui->mainHorizontalLayout->setStretch(0, 2);
    ui->mainHorizontalLayout->setStretch(1, 1);

    setupPlotWidgets();
    refreshSimulationPlots();

    Simulation::Test::runSimulationSmokeTest();
    Simulation::Test::runPidTuningTests();
    Simulation::Test::runQuadraticDiagnosticTest();

    setupMenus();
}

MainWindow::~MainWindow()
{
    delete m_basicSettingsDialog;
    delete m_paramSettingsDialog;
    delete ui;
}

void MainWindow::setupPlotWidgets()
{
    m_forcePlot = new PlotWidget(tr("拉力曲线"), this);
    m_forcePlot->setAxisLabels(tr("时间 (s)"), tr("拉力"));

    m_errorPlot = new PlotWidget(tr("位移误差曲线"), this);
    m_errorPlot->setAxisLabels(tr("时间 (s)"), tr("误差"));

    m_displacementPlot = new PlotWidget(tr("位移曲线"), this);
    m_displacementPlot->setAxisLabels(tr("时间 (s)"), tr("位移"));

    m_animationWidget = new AnimationWidget(this);
    m_animationWidget->setDisplacementUpperLimit(currentDisplacementUpperLimit());
    connect(m_animationWidget, &AnimationWidget::stopRequested,
            this, &MainWindow::onAnimationStopRequested);

    ui->forcePlotLayout->addWidget(m_forcePlot);
    ui->errorPlotLayout->addWidget(m_errorPlot);
    ui->displacementPlotLayout->addWidget(m_displacementPlot);

    delete ui->animationPlaceholder;
    ui->animationPanelLayout->addWidget(m_animationWidget);
}

void MainWindow::loadSimulationPlots()
{
    Simulation::SolarWingSimulator simulator;
    simulator.setParameters(m_currentSimulationParameters);
    const Simulation::SimulationResult result = simulator.run();

    if (result.isEmpty()) {
        m_forcePlot->clearSeries();
        m_errorPlot->clearSeries();
        m_displacementPlot->clearSeries();
        if (m_animationWidget) {
            m_animationWidget->setDisplacementUpperLimit(currentDisplacementUpperLimit());
            m_animationWidget->setActualDisplacement(0.0);
            m_animationWidget->setTargetDisplacement(0.0);
        }
        m_stopHookTriggered = false;
        ui->statusbar->showMessage(tr("仿真结果为空，无法绘制曲线"));
        return;
    }

    QVector<double> timeValues;
    QVector<double> forceValues;
    QVector<double> errorValues;
    QVector<double> targetValues;
    QVector<double> actualValues;

    timeValues.reserve(result.size());
    forceValues.reserve(result.size());
    errorValues.reserve(result.size());
    targetValues.reserve(result.size());
    actualValues.reserve(result.size());

    for (const Simulation::SimulationSample &sample : result.samples) {
        timeValues.append(sample.time);
        forceValues.append(sample.tension);
        errorValues.append(sample.error);
        targetValues.append(sample.targetDisplacement);
        actualValues.append(sample.actualDisplacement);
    }

    m_forcePlot->setSingleSeries(tr("拉力"), timeValues, forceValues, QColor(231, 76, 60));
    m_errorPlot->setSingleSeries(tr("误差"), timeValues, errorValues, QColor(241, 196, 15));

    QVector<PlotWidget::SeriesData> displacementSeries;
    displacementSeries.append({tr("参考位移"), timeValues, targetValues, QColor(52, 152, 219)});
    displacementSeries.append({tr("实际位移"), timeValues, actualValues, QColor(46, 204, 113)});
    m_displacementPlot->setSeriesList(displacementSeries);

    if (m_animationWidget && !result.samples.isEmpty()) {
        m_animationWidget->setDisplacementUpperLimit(currentDisplacementUpperLimit());
        m_animationWidget->setActualDisplacement(result.samples.constLast().actualDisplacement);
        m_animationWidget->setTargetDisplacement(result.samples.constLast().targetDisplacement);

        if (m_animationWidget->isLimitExceeded()) {
            m_stopHookTriggered = true;
            ui->statusbar->showMessage(tr("已达到位移上限，已触发停止钩子"));
        } else {
            m_stopHookTriggered = false;
        }
    }
}

Simulation::SimulationParameters MainWindow::createDefaultPlotSimulationParameters() const
{
    Simulation::SimulationParameters params;
    params.totalTime = 4.0;
    params.timeStep = 0.01;
    params.integrator = Simulation::IntegratorType::RK4;

    params.model.massPerMeter = 2.0;
    params.model.hookMass = 12.0;
    params.model.gravity = 9.8;
    params.model.cableAngleDeg = 0.02;
    params.model.damping = 1.0;
    params.model.stiffness = 2.0;
    params.maxDisplacement = 1.5;

    params.pid.kp = 320.0;
    params.pid.ki = 25.0;
    params.pid.kd = 45.0;
    params.pid.outputMin = 0.0;
    params.pid.outputMax = 5000.0;

    params.target.type = Simulation::TargetType::Quadratic;
    params.target.quadA = 0.02;
    params.target.quadB = 0.03;
    return params;
}

Simulation::SimulationParameters MainWindow::simulationParametersFromDialog() const
{
    Simulation::SimulationParameters params = m_currentSimulationParameters;
    if (!m_paramSettingsDialog) {
        return params;
    }

    const SimulationParams dialogParams = m_paramSettingsDialog->params();
    params.model.massPerMeter = dialogParams.massPerMeter;
    params.model.gravity = dialogParams.gravity;
    params.model.hookMass = dialogParams.hookMass;
    params.model.cableAngleDeg = dialogParams.cableAngle;

    params.pid.kp = dialogParams.kp;
    params.pid.ki = dialogParams.ki;
    params.pid.kd = dialogParams.kd;

    params.target.type = dialogParams.targetType == SimulationParams::TargetQuadratic
                             ? Simulation::TargetType::Quadratic
                             : Simulation::TargetType::Constant;
    params.target.constantTarget = dialogParams.targetConstant;
    params.target.quadA = dialogParams.quadCoefA;
    params.target.quadB = dialogParams.quadCoefB;

    params.totalTime = dialogParams.simDuration;
    params.timeStep = dialogParams.simStep;
    params.maxDisplacement = dialogParams.maxDisplacement;
    return params;
}

void MainWindow::applyBasicSettingsToSimulationParameters()
{
    if (!m_basicSettingsDialog) {
        return;
    }

    m_currentSimulationParameters.integrator = integratorTypeFromIndex(m_basicSettingsDialog->integratorType());
}

void MainWindow::syncDialogsFromCurrentParameters()
{
    if (m_paramSettingsDialog) {
        m_paramSettingsDialog->setParams(dialogParamsFromSimulation(m_currentSimulationParameters));
    }
}

void MainWindow::refreshSimulationPlots()
{
    applyBasicSettingsToSimulationParameters();
    if (m_paramSettingsDialog) {
        m_currentSimulationParameters = simulationParametersFromDialog();
    }
    loadSimulationPlots();
}

double MainWindow::currentDisplacementUpperLimit() const
{
    return m_currentSimulationParameters.maxDisplacement;
}

void MainWindow::setupMenus()
{
    m_basicSettingsAction = new QAction(tr("基本设置"), this);
    m_basicSettingsAction->setShortcut(QKeySequence(tr("Ctrl+Alt+F")));
    connect(m_basicSettingsAction, &QAction::triggered, this, &MainWindow::openBasicSettings);

    m_paramSettingsAction = new QAction(tr("参数设置"), this);
    m_paramSettingsAction->setShortcut(QKeySequence(tr("Ctrl+Alt+P")));
    connect(m_paramSettingsAction, &QAction::triggered, this, &MainWindow::openParamSettings);

    m_startAction = new QAction(tr("开始"), this);
    m_startAction->setShortcut(QKeySequence(tr("Alt+S")));
    connect(m_startAction, &QAction::triggered, this, &MainWindow::onSimulationStart);

    m_pauseAction = new QAction(tr("暂停"), this);
    m_pauseAction->setShortcut(QKeySequence(tr("Alt+P")));
    connect(m_pauseAction, &QAction::triggered, this, &MainWindow::onSimulationPause);

    m_stopAction = new QAction(tr("停止"), this);
    m_stopAction->setShortcut(QKeySequence(tr("Alt+T")));
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::onSimulationStop);

    ui->settingsMenu->addAction(m_basicSettingsAction);
    ui->settingsMenu->addAction(m_paramSettingsAction);
    ui->controlMenu->addAction(m_startAction);
    ui->controlMenu->addAction(m_pauseAction);
    ui->controlMenu->addAction(m_stopAction);
}

void MainWindow::openBasicSettings()
{
    if (!m_basicSettingsDialog) {
        m_basicSettingsDialog = new BasicSettingsDialog(this);
        connect(m_basicSettingsDialog, &BasicSettingsDialog::settingsChanged, this, [this]() {
            applyBasicSettingsToSimulationParameters();
            refreshSimulationPlots();
        });
    }
    m_basicSettingsDialog->show();
    m_basicSettingsDialog->raise();
    m_basicSettingsDialog->activateWindow();
}

void MainWindow::openParamSettings()
{
    if (!m_paramSettingsDialog) {
        m_paramSettingsDialog = new ParamSettingsDialog(this);
        m_paramSettingsDialog->setParams(dialogParamsFromSimulation(m_currentSimulationParameters));
        connect(m_paramSettingsDialog, &ParamSettingsDialog::paramsChanged, this, [this]() {
            m_currentSimulationParameters = simulationParametersFromDialog();
            refreshSimulationPlots();
        });
        connect(m_paramSettingsDialog, &ParamSettingsDialog::startRequested,
                this, &MainWindow::onSimulationStart);
        connect(m_paramSettingsDialog, &ParamSettingsDialog::startRequested, this, [this]() {
            m_currentSimulationParameters = simulationParametersFromDialog();
            refreshSimulationPlots();
        });
        connect(m_paramSettingsDialog, &ParamSettingsDialog::pauseRequested,
                this, &MainWindow::onSimulationPause);
        connect(m_paramSettingsDialog, &ParamSettingsDialog::stopRequested,
                this, &MainWindow::onSimulationStop);
    }
    syncDialogsFromCurrentParameters();
    m_paramSettingsDialog->show();
    m_paramSettingsDialog->raise();
    m_paramSettingsDialog->activateWindow();
}

void MainWindow::onSimulationStart()
{
    if (m_stopHookTriggered) {
        ui->statusbar->showMessage(tr("已触发停止钩子，请先调整参数后再继续"));
        return;
    }

    ui->statusbar->showMessage(tr("仿真开始，曲线已刷新"));
}

void MainWindow::onSimulationPause()
{
    ui->statusbar->showMessage(tr("仿真暂停"));
}

void MainWindow::onSimulationStop()
{
    m_stopHookTriggered = false;
    ui->statusbar->showMessage(tr("仿真停止"));
}

void MainWindow::onAnimationStopRequested()
{
    if (m_stopHookTriggered) {
        return;
    }

    m_stopHookTriggered = true;
    QMessageBox::warning(this,
                         tr("上限报警"),
                         tr("太阳翼位置已达到设置的最高值，仿真已停止。"));
    onSimulationStop();
}
