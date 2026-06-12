#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
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
class QString;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openBasicSettings();
    void openParamSettings();
    void saveScenarioConfig();
    void loadScenarioConfig();
    void onSimulationStart();
    void onSimulationPause();
    void onSimulationStop();
    void onAnimationStopRequested();
    void onPlaybackTick();
    void onAnimationSeekRequested(double progress);

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
    void advanceToSample(int index);
    void stopPlayback();
    bool ensureSettingsDialogs();
    bool loadScenarioConfigFromFile(const QString &filePath);
    void resetAnimationDisplay();

    Ui::MainWindow *ui;
    BasicSettingsDialog *m_basicSettingsDialog;
    ParamSettingsDialog *m_paramSettingsDialog;

    QAction *m_basicSettingsAction;
    QAction *m_paramSettingsAction;
    QAction *m_startAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;
    QAction *m_saveConfigAction;
    QAction *m_loadConfigAction;

    PlotWidget *m_forcePlot;
    PlotWidget *m_errorPlot;
    PlotWidget *m_displacementPlot;
    AnimationWidget *m_animationWidget;
    Simulation::SimulationParameters m_currentSimulationParameters;
    Simulation::SimulationResult m_lastResult;
    QTimer *m_playbackTimer;
    int m_currentSampleIndex;
    bool m_stopHookTriggered;
};

#endif // MAINWINDOW_H
