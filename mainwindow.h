#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

#include "dialogs/paramsettingsdialog.h"
#include "simulation/simulationtypes.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class AnimationWidget;
class BasicSettingsDialog;
class QAction;
class QMenu;
class PlotWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openBasicSettings();
    void openParamSettings();
    void onSimulationStart();
    void onSimulationPause();
    void onSimulationStop();
    void onAnimationStopRequested();

private:
    void setupMenus();
    void setupPlotWidgets();
    void loadSimulationPlots();
    Simulation::SimulationParameters createDefaultPlotSimulationParameters() const;
    Simulation::SimulationParameters simulationParametersFromDialog() const;
    void applyBasicSettingsToSimulationParameters();
    void syncDialogsFromCurrentParameters();
    void refreshSimulationPlots();
    double currentDisplacementUpperLimit() const;

    Ui::MainWindow *ui;
    BasicSettingsDialog *m_basicSettingsDialog;
    ParamSettingsDialog *m_paramSettingsDialog;

    QAction *m_basicSettingsAction;
    QAction *m_paramSettingsAction;
    QAction *m_startAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;

    PlotWidget *m_forcePlot;
    PlotWidget *m_errorPlot;
    PlotWidget *m_displacementPlot;
    AnimationWidget *m_animationWidget;
    Simulation::SimulationParameters m_currentSimulationParameters;
    bool m_stopHookTriggered;
};

#endif // MAINWINDOW_H
