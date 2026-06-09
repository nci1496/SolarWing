#ifndef PARAMSETTINGSDIALOG_H
#define PARAMSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class ParamSettingsDialog;
}

struct SimulationParams {
    double massPerMeter = 2.0;
    double gravity = 9.8;
    double hookMass = 12.0;
    double cableAngle = 0.02;

    double kp = 1.0;
    double ki = 0.0;
    double kd = 0.0;

    enum TargetType { TargetConstant, TargetQuadratic };
    TargetType targetType = TargetConstant;
    double targetConstant = 1.0;
    double quadCoefA = 0.02;
    double quadCoefB = 0.03;

    double simDuration = 10.0;
    double simStep = 0.01;

    QString validate() const;
};

class ParamSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParamSettingsDialog(QWidget *parent = nullptr);
    ~ParamSettingsDialog() override;

    SimulationParams params() const { return m_params; }
    void setParams(const SimulationParams &p) { m_params = p; syncUI(); }

    enum SimState { Stopped, Running, Paused };
    SimState simState() const { return m_simState; }

signals:
    void paramsChanged();
    void startRequested();
    void pauseRequested();
    void stopRequested();

protected:
    void done(int r) override;

private slots:
    void onParamChanged();
    void onStart();
    void onPause();
    void onStop();
    void updateIndicators();

private:
    void loadSettings();
    void syncUI();
    bool validateAll();

    Ui::ParamSettingsDialog *ui;
    SimulationParams m_params;
    SimState m_simState;
};

#endif // PARAMSETTINGSDIALOG_H
