#ifndef BASICSETTINGSDIALOG_H
#define BASICSETTINGSDIALOG_H

#include <QDialog>
#include <QColor>
#include <QFont>
#include <QStringList>
#include <QPushButton>

namespace Ui {
class BasicSettingsDialog;
}

class BasicSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BasicSettingsDialog(QWidget *parent = nullptr);
    ~BasicSettingsDialog() override;

    QColor curveBgColor() const { return m_curveBgColor; }
    void setCurveBgColor(const QColor &c) { m_curveBgColor = c; }

    QColor textColor() const { return m_textColor; }
    void setTextColor(const QColor &c) { m_textColor = c; }

    QFont selectedFont() const { return m_font; }
    void setSelectedFont(const QFont &f) { m_font = f; }

    int fontSize() const { return m_font.pointSize(); }
    void setFontSize(int size) { m_font.setPointSize(size); }

    int integratorType() const { return m_integratorType; }
    void setIntegratorType(int t) { m_integratorType = t; }

    static QStringList integratorNames();

signals:
    void settingsChanged();

protected:
    void done(int r) override;

private slots:
    void onSelectBgColor();
    void onSelectTextColor();
    void onFontChanged();

private:
    void loadSettings();
    void saveSettings();
    void updateColorButtons();

    Ui::BasicSettingsDialog *ui;
    QColor m_curveBgColor;
    QColor m_textColor;
    QFont m_font;
    int m_integratorType;
};

#endif // BASICSETTINGSDIALOG_H
