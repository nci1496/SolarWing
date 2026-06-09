#include "basicsettingsdialog.h"
#include "ui_basicsettingsdialog.h"
#include <QColorDialog>
#include <QSettings>
#include <QFontComboBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QPushButton>

BasicSettingsDialog::BasicSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BasicSettingsDialog)
    , m_curveBgColor(Qt::white)
    , m_textColor(Qt::black)
    , m_font("Microsoft YaHei", 10)
    , m_integratorType(0)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Connect color buttons
    connect(ui->bgColorButton, &QPushButton::clicked,
            this, &BasicSettingsDialog::onSelectBgColor);
    connect(ui->textColorButton, &QPushButton::clicked,
            this, &BasicSettingsDialog::onSelectTextColor);
    connect(ui->fontComboBox, &QFontComboBox::currentFontChanged,
            this, &BasicSettingsDialog::onFontChanged);
    connect(ui->fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int val) {
                m_font.setPointSize(val);
                emit settingsChanged();
            });

    // Connect algorithm radio buttons
    connect(ui->eulerRadio, &QRadioButton::toggled, [this](bool checked) {
        if (checked) { m_integratorType = 0; emit settingsChanged(); }
    });
    connect(ui->rk2Radio, &QRadioButton::toggled, [this](bool checked) {
        if (checked) { m_integratorType = 1; emit settingsChanged(); }
    });
    connect(ui->rk4Radio, &QRadioButton::toggled, [this](bool checked) {
        if (checked) { m_integratorType = 2; emit settingsChanged(); }
    });

    // Connect dialog buttons
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    loadSettings();
    updateColorButtons();
}

BasicSettingsDialog::~BasicSettingsDialog()
{
    delete ui;
}

QStringList BasicSettingsDialog::integratorNames()
{
    return { "欧拉法", "二阶龙格-库塔法", "四阶龙格-库塔法" };
}

void BasicSettingsDialog::loadSettings()
{
    QSettings settings("SolarWing", "BasicSettings");
    settings.beginGroup("colors");
    m_curveBgColor = settings.value("bgColor", QColor(Qt::white)).value<QColor>();
    m_textColor    = settings.value("textColor", QColor(Qt::black)).value<QColor>();
    settings.endGroup();

    settings.beginGroup("font");
    m_font.setFamily(settings.value("family", "Microsoft YaHei").toString());
    m_font.setPointSize(settings.value("size", 10).toInt());
    settings.endGroup();

    m_integratorType = settings.value("integratorType", 0).toInt();

    // Sync to UI
    ui->fontComboBox->setCurrentFont(m_font);
    ui->fontSizeSpinBox->setValue(m_font.pointSize());

    switch (m_integratorType) {
    case 0: ui->eulerRadio->setChecked(true); break;
    case 1: ui->rk2Radio->setChecked(true);  break;
    case 2: ui->rk4Radio->setChecked(true);  break;
    }
}

void BasicSettingsDialog::saveSettings()
{
    QSettings settings("SolarWing", "BasicSettings");
    settings.beginGroup("colors");
    settings.setValue("bgColor", m_curveBgColor);
    settings.setValue("textColor", m_textColor);
    settings.endGroup();

    settings.beginGroup("font");
    settings.setValue("family", m_font.family());
    settings.setValue("size", m_font.pointSize());
    settings.endGroup();

    settings.setValue("integratorType", m_integratorType);
}

void BasicSettingsDialog::onSelectBgColor()
{
    QColor c = QColorDialog::getColor(m_curveBgColor, this, "选择曲线背景颜色");
    if (c.isValid()) {
        m_curveBgColor = c;
        updateColorButtons();
        emit settingsChanged();
    }
}

void BasicSettingsDialog::onSelectTextColor()
{
    QColor c = QColorDialog::getColor(m_textColor, this, "选择文字颜色");
    if (c.isValid()) {
        m_textColor = c;
        updateColorButtons();
        emit settingsChanged();
    }
}

void BasicSettingsDialog::onFontChanged()
{
    m_font = ui->fontComboBox->currentFont();
    m_font.setPointSize(ui->fontSizeSpinBox->value());
    emit settingsChanged();
}

void BasicSettingsDialog::updateColorButtons()
{
    // Use stylesheet to show the selected color in button
    auto styleButton = [](QPushButton *btn, const QColor &color) {
        QString rgb = QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue());
        btn->setStyleSheet(QString("background-color: rgb(%1); color: %2;")
                               .arg(rgb)
                               .arg(color.red() > 128 ? "black" : "white"));
    };
    styleButton(ui->bgColorButton, m_curveBgColor);
    styleButton(ui->textColorButton, m_textColor);
}

void BasicSettingsDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        saveSettings();
        emit settingsChanged();
    }
    QDialog::done(r);
}
