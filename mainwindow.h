#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialogs/paramsettingsdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class BasicSettingsDialog;
class QAction;
class QMenu;

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

private:
    void setupMenus();

    Ui::MainWindow *ui;
    BasicSettingsDialog *m_basicSettingsDialog;
    ParamSettingsDialog *m_paramSettingsDialog;

    QAction *m_basicSettingsAction;
    QAction *m_paramSettingsAction;
    QAction *m_startAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;
};

#endif // MAINWINDOW_H
