QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    dialogs/basicsettingsdialog.cpp \
    dialogs/paramsettingsdialog.cpp \
    simulation/simulationtypes.cpp \
    simulation/pidcontroller.cpp \
    simulation/solarwingmodel.cpp \
    simulation/integrator.cpp \
    simulation/solarwingsimulator.cpp \
    simulation/test_simulation.cpp \
    simulation/test_pid_tuning.cpp \
    simulation/test_simulation_diagnostics.cpp \
    widgets/plotwidget.cpp \
    widgets/animationwidget.cpp \
    ../../QCustomPlot/qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    dialogs/basicsettingsdialog.h \
    dialogs/paramsettingsdialog.h \
    simulation/simulationtypes.h \
    simulation/pidcontroller.h \
    simulation/solarwingmodel.h \
    simulation/integrator.h \
    simulation/solarwingsimulator.h \
    simulation/test_simulation.h \
    simulation/test_pid_tuning.h \
    simulation/test_simulation_diagnostics.h \
    widgets/plotwidget.h \
    widgets/animationwidget.h \
    ../../QCustomPlot/qcustomplot.h

FORMS += \
    mainwindow.ui \
    dialogs/basicsettingsdialog.ui \
    dialogs/paramsettingsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
