项目名称：
SolarWing

开发框架：
Qt Widgets + C++

开发工具：
Qt Creator

项目目标：
实现太阳翼展开位移控制仿真系统

主要模块：
1. 主界面
2. 参数设置
3. PID控制
4. 数值积分
5. 曲线显示
6. 动画显示
7. 参数保存读取


1.项目文件
（仅为大致内容，如有需要可以创建）

SolarWing
│
├── main.cpp
│
├── MainWindow.h
├── MainWindow.cpp
├── MainWindow.ui
│
├── dialogs
│   ├── BasicSettingDialog.h
│   ├── BasicSettingDialog.cpp
│   ├── BasicSettingDialog.ui
│   │
│   ├── ParamSettingDialog.h
│   ├── ParamSettingDialog.cpp
│   └── ParamSettingDialog.ui
│
├── simulation
│   ├── SolarWingModel.h
│   ├── SolarWingModel.cpp
│   │
│   ├── PIDController.h
│   ├── PIDController.cpp
│   │
│   ├── Integrator.h
│   └── Integrator.cpp
│
├── widgets
│   ├── PlotWidget.h
│   ├── PlotWidget.cpp
│   │
│   ├── AnimationWidget.h
│   └── AnimationWidget.cpp
│
├── data
│   ├── SettingsManager.h
│   └── SettingsManager.cpp
│
└── resources
    ├── solarwing.png
    └── resource.qrc

2.项目任务见task/total_task.md，里面描述详细内容

3.项目流程初步：
Phase1 UI框架
Phase2 仿真核心
Phase3 曲线系统
Phase4 动画系统
Phase5 参数系统
Phase6 保存读取
Phase7 交互功能
Phase8 验收优化

4.项目要求：
每次仅完成指定任务，验收完成即可，其余文件如无必要，不修改。

有关于环境的，不需要你做任何修改和编译，我会单独在QtCreator里面编译运行