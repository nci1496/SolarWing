#include "paramsettingsdialog.h"
#include "ui_paramsettingsdialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QRadioButton>

QString SimulationParams::validate() const
{
    if (massPerMeter < 1.0 || massPerMeter > 5.0)
        return QString("单位长度质量 k 应在 1~5 kg/m 范围内，当前值: %1").arg(massPerMeter);
    if (gravity < 9.8 || gravity > 10.0)
        return QString("重力加速度 g 应在 9.8~10 kg/s^2 范围内，当前值: %1").arg(gravity);
    if (hookMass < 10.0 || hookMass > 15.0)
        return QString("机械吊钩质量 m1 应在 10~15 kg 范围内，当前值: %1").arg(hookMass);
    if (cableAngle < 0.0 || cableAngle > 0.05)
        return QString("钢丝绳倾斜角度 θ 应在 0~0.05° 范围内，当前值: %1").arg(cableAngle);
    if (quadCoefA < 0.015 || quadCoefA > 0.025)
        return QString("二次系数 A 应在 0.015~0.025 范围内，当前值: %1").arg(quadCoefA);
    if (quadCoefB < 0.02 || quadCoefB > 0.04)
        return QString("一次系数 B 应在 0.02~0.04 范围内，当前值: %1").arg(quadCoefB);
    if (maxDisplacement <= 0.0)
        return QString("最高位移应大于 0，当前值: %1").arg(maxDisplacement);
    return QString();
}

ParamSettingsDialog::ParamSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ParamSettingsDialog)
    , m_simState(Stopped)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Bind slider to spinbox for Kp, Ki, Kd
    connect(ui->kpSlider, &QSlider::valueChanged, this, [this](int val) {
        ui->kpSpinBox->setValue(val / 100.0);
    });
    connect(ui->kiSlider, &QSlider::valueChanged, this, [this](int val) {
        ui->kiSpinBox->setValue(val / 100.0);
    });
    connect(ui->kdSlider, &QSlider::valueChanged, this, [this](int val) {
        ui->kdSpinBox->setValue(val / 100.0);
    });

    connect(ui->kpSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double val) { ui->kpSlider->setValue(int(val * 100)); });
    connect(ui->kiSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double val) { ui->kiSlider->setValue(int(val * 100)); });
    connect(ui->kdSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double val) { ui->kdSlider->setValue(int(val * 100)); });

    // Sync target type radio buttons
    connect(ui->constantTargetRadio, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_params.targetType = SimulationParams::TargetConstant;
    });
    connect(ui->quadraticTargetRadio, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_params.targetType = SimulationParams::TargetQuadratic;
    });

    // Sync all param widgets to m_params
    QList<QDoubleSpinBox*> paramSpinBoxes = {
        ui->massSpinBox, ui->gravitySpinBox, ui->hookMassSpinBox, ui->cableAngleSpinBox,
        ui->kpSpinBox, ui->kiSpinBox, ui->kdSpinBox,
        ui->constantTargetSpinBox, ui->coefASpinBox, ui->coefBSpinBox,
        ui->simTimeSpinBox, ui->simStepSpinBox, ui->maxDisplacementSpinBox
    };
    for (QDoubleSpinBox *spinBox : paramSpinBoxes) {
        connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &ParamSettingsDialog::onParamChanged);
    }

    // Control buttons
    connect(ui->startButton, &QPushButton::clicked, this, &ParamSettingsDialog::onStart);
    connect(ui->pauseButton, &QPushButton::clicked, this, &ParamSettingsDialog::onPause);
    connect(ui->stopButton, &QPushButton::clicked, this, &ParamSettingsDialog::onStop);

    // Dialog buttons
    connect(ui->okButton, &QPushButton::clicked, this, [this]() {
        if (!validateAll()) return;
        done(Accepted);
    });
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    loadSettings();
    syncUI();
    updateIndicators();
}

ParamSettingsDialog::~ParamSettingsDialog()
{
    delete ui;
}

void ParamSettingsDialog::syncUI()
{
    m_syncingUi = true;

    ui->massSpinBox->setValue(m_params.massPerMeter);
    ui->gravitySpinBox->setValue(m_params.gravity);
    ui->hookMassSpinBox->setValue(m_params.hookMass);
    ui->cableAngleSpinBox->setValue(m_params.cableAngle);

    ui->kpSpinBox->setValue(m_params.kp);
    ui->kiSpinBox->setValue(m_params.ki);
    ui->kdSpinBox->setValue(m_params.kd);
    ui->kpSlider->setValue(int(m_params.kp * 100));
    ui->kiSlider->setValue(int(m_params.ki * 100));
    ui->kdSlider->setValue(int(m_params.kd * 100));

    if (m_params.targetType == SimulationParams::TargetConstant) {
        ui->constantTargetRadio->setChecked(true);
        ui->constantTargetSpinBox->setValue(m_params.targetConstant);
    } else {
        ui->quadraticTargetRadio->setChecked(true);
        ui->coefASpinBox->setValue(m_params.quadCoefA);
        ui->coefBSpinBox->setValue(m_params.quadCoefB);
    }

    ui->simTimeSpinBox->setValue(m_params.simDuration);
    ui->simStepSpinBox->setValue(m_params.simStep);
    ui->maxDisplacementSpinBox->setValue(m_params.maxDisplacement);

    m_syncingUi = false;
}

void ParamSettingsDialog::onParamChanged()
{
    if (m_syncingUi) {
        return;
    }

    // Read all params from UI
    m_params.massPerMeter = ui->massSpinBox->value();
    m_params.gravity = ui->gravitySpinBox->value();
    m_params.hookMass = ui->hookMassSpinBox->value();
    m_params.cableAngle = ui->cableAngleSpinBox->value();

    m_params.kp = ui->kpSpinBox->value();
    m_params.ki = ui->kiSpinBox->value();
    m_params.kd = ui->kdSpinBox->value();

    m_params.targetConstant = ui->constantTargetSpinBox->value();
    m_params.quadCoefA = ui->coefASpinBox->value();
    m_params.quadCoefB = ui->coefBSpinBox->value();

    m_params.simDuration = ui->simTimeSpinBox->value();
    m_params.simStep = ui->simStepSpinBox->value();
    m_params.maxDisplacement = ui->maxDisplacementSpinBox->value();

    emit paramsChanged();
}

void ParamSettingsDialog::onStart()
{
    if (!validateAll()) return;
    m_simState = Running;
    updateIndicators();
    emit startRequested();
}

void ParamSettingsDialog::onPause()
{
    if (m_simState != Running) return;
    m_simState = Paused;
    updateIndicators();
    emit pauseRequested();
}

void ParamSettingsDialog::onStop()
{
    m_simState = Stopped;
    updateIndicators();
    emit stopRequested();
}

void ParamSettingsDialog::updateIndicators()
{
    QString runStyle = "background-color: #ff4444; color: white; padding: 4px; border-radius: 4px;";
    QString pauseStyle = "background-color: #ffaa00; color: white; padding: 4px; border-radius: 4px;";
    QString readyStyle = "background-color: #44cc44; color: white; padding: 4px; border-radius: 4px;";
    QString dimStyle = "background-color: #888888; color: white; padding: 4px; border-radius: 4px;";

    ui->runIndicator->setStyleSheet(m_simState == Running ? runStyle : dimStyle);
    ui->pauseIndicator->setStyleSheet(m_simState == Paused ? pauseStyle : dimStyle);
    ui->statusIndicator->setStyleSheet(m_simState == Stopped ? readyStyle : dimStyle);
}

bool ParamSettingsDialog::validateAll()
{
    onParamChanged();
    QString err = m_params.validate();
    if (!err.isEmpty()) {
        QMessageBox::warning(this, "参数错误", err);
        return false;
    }
    return true;
}

void ParamSettingsDialog::loadSettings()
{
    QSettings settings("SolarWing", "ParamSettings");
    settings.beginGroup("dynamics");
    m_params.massPerMeter = settings.value("massPerMeter", 2.0).toDouble();
    m_params.gravity = settings.value("gravity", 9.8).toDouble();
    m_params.hookMass = settings.value("hookMass", 12.0).toDouble();
    m_params.cableAngle = settings.value("cableAngle", 0.02).toDouble();
    settings.endGroup();

    settings.beginGroup("pid");
    m_params.kp = settings.value("kp", 1.0).toDouble();
    m_params.ki = settings.value("ki", 0.0).toDouble();
    m_params.kd = settings.value("kd", 0.0).toDouble();
    settings.endGroup();

    settings.beginGroup("target");
    m_params.targetType = (SimulationParams::TargetType)settings.value("targetType", 0).toInt();
    m_params.targetConstant = settings.value("targetConstant", 1.0).toDouble();
    m_params.quadCoefA = settings.value("quadCoefA", 0.02).toDouble();
    m_params.quadCoefB = settings.value("quadCoefB", 0.03).toDouble();
    settings.endGroup();

    settings.beginGroup("sim");
    m_params.simDuration = settings.value("simDuration", 10.0).toDouble();
    m_params.simStep = settings.value("simStep", 0.01).toDouble();
    m_params.maxDisplacement = settings.value("maxDisplacement", 1.5).toDouble();
    settings.endGroup();
}

void ParamSettingsDialog::done(int r)
{
    if (r == Accepted) {
        QSettings settings("SolarWing", "ParamSettings");
        settings.beginGroup("dynamics");
        settings.setValue("massPerMeter", m_params.massPerMeter);
        settings.setValue("gravity", m_params.gravity);
        settings.setValue("hookMass", m_params.hookMass);
        settings.setValue("cableAngle", m_params.cableAngle);
        settings.endGroup();

        settings.beginGroup("pid");
        settings.setValue("kp", m_params.kp);
        settings.setValue("ki", m_params.ki);
        settings.setValue("kd", m_params.kd);
        settings.endGroup();

        settings.beginGroup("target");
        settings.setValue("targetType", (int)m_params.targetType);
        settings.setValue("targetConstant", m_params.targetConstant);
        settings.setValue("quadCoefA", m_params.quadCoefA);
        settings.setValue("quadCoefB", m_params.quadCoefB);
        settings.endGroup();

        settings.beginGroup("sim");
        settings.setValue("simDuration", m_params.simDuration);
        settings.setValue("simStep", m_params.simStep);
        settings.setValue("maxDisplacement", m_params.maxDisplacement);
        settings.endGroup();

        emit paramsChanged();
    }
    QDialog::done(r);
}
