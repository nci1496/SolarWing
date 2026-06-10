#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogs/basicsettingsdialog.h"
#include "simulation/test_pid_tuning.h"
#include "simulation/test_simulation.h"
#include "simulation/test_simulation_diagnostics.h"

#include <QAction>
#include <QMenu>
#include <QStatusBar>

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
{
    ui->setupUi(this);
    ui->statusbar->showMessage(tr("系统就绪"));
    ui->mainHorizontalLayout->setStretch(0, 2);
    ui->mainHorizontalLayout->setStretch(1, 1);

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
    }
    m_basicSettingsDialog->show();
    m_basicSettingsDialog->raise();
    m_basicSettingsDialog->activateWindow();
}

void MainWindow::openParamSettings()
{
    if (!m_paramSettingsDialog) {
        m_paramSettingsDialog = new ParamSettingsDialog(this);
        connect(m_paramSettingsDialog, &ParamSettingsDialog::startRequested,
                this, &MainWindow::onSimulationStart);
        connect(m_paramSettingsDialog, &ParamSettingsDialog::pauseRequested,
                this, &MainWindow::onSimulationPause);
        connect(m_paramSettingsDialog, &ParamSettingsDialog::stopRequested,
                this, &MainWindow::onSimulationStop);
    }
    m_paramSettingsDialog->show();
    m_paramSettingsDialog->raise();
    m_paramSettingsDialog->activateWindow();
}

void MainWindow::onSimulationStart()
{
    ui->statusbar->showMessage(tr("仿真开始"));
}

void MainWindow::onSimulationPause()
{
    ui->statusbar->showMessage(tr("仿真暂停"));
}

void MainWindow::onSimulationStop()
{
    ui->statusbar->showMessage(tr("仿真停止"));
}
