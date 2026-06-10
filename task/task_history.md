# SolarWing 任务历史与阶段路线图

## 1. 项目背景
- 项目名称：SolarWing
- 技术栈：Qt Widgets + C++
- 目标：实现太阳翼展开时变重力卸载控制的可视化编程与仿真系统
- 需求来源：`task/total_task.md`
- 协作约束：每次仅完成指定任务，验收完成即可，其余文件如无必要不修改

## 2. 当前项目状态
- 当前仓库仅包含最小 Qt Widgets 骨架：`main.cpp`、`mainwindow.h`、`mainwindow.cpp`、`mainwindow.ui`
- 尚未建立如下模块目录与文件：`dialogs/`、`simulation/`、`widgets/`、`data/`、`resources/`
- `task/current_task.md` 与本文件用于多轮 AI 协作接力维护

## 3. 全局阶段路线图

### Phase 1：基础骨架
**目标**
- 建立主窗口布局骨架
- 预留左侧三图区域与右侧动画区域
- 预留菜单、状态栏、控制入口
- 明确主窗口负责调度，子模块负责具体功能

**建议涉及内容**
- `mainwindow.ui`
- `mainwindow.h`
- `mainwindow.cpp`
- 视情况准备目录：`dialogs/`、`simulation/`、`widgets/`、`data/`、`resources/`

**验收标准**
- 主窗口结构符合课程要求的大体布局
- 可以作为后续绘图、动画、菜单、控制逻辑的挂载骨架
- 暂不接入业务计算与数据流

---

### Phase 2：设置对话框
**目标**
- 完成“基本设置”对话框
- 完成“参数设置”对话框
- 建立输入约束、参数范围校验与错误提示

**包含功能**
- 基本设置：曲线背景颜色、文字颜色、字体、字号、积分算法单选
- 参数设置：动力学参数、PID 参数、目标设置、仿真参数、调试操作区
- 参数非法时弹窗提示

**验收标准**
- 两个对话框可正常打开
- 控件齐全、布局清晰
- 输入校验符合任务要求中的参数范围

---

### Phase 3：仿真核心
**目标**
- 建立系统动力学模型
- 建立 PID 控制器
- 建立数值积分器策略
- 统一单步仿真数据输出接口

**包含功能**
- SolarWingModel
- PIDController
- Integrator（Euler / RK2 / RK4）
- 单步输出：实际位移、目标位移、误差、拉力、时间

**验收标准**
- 可根据输入参数进行单步或整段仿真
- 能根据所选算法得到对应输出序列
- 数据接口可直接供曲线与动画模块使用

---

### Phase 4：曲线可视化
**目标**
- 绘制三幅曲线图
- 支持十字光标、单击显示坐标点、双击清空坐标点
- 正确显示实际位移与参考位移

**包含功能**
- 拉力曲线
- 位移误差曲线
- 实际位移 / 参考位移曲线
- 坐标轴、刻度、标题、颜色区分

**验收标准**
- 三幅图位置正确
- 标注正确、刻度正确、曲线更新正确
- 鼠标交互符合课程要求

---

### Phase 5：动画系统
**目标**
- 实现太阳翼展开动画
- 显示参考位置与实际位置
- 达到最高位置时报警并停止仿真

**包含功能**
- 太阳翼位图
- 卷扬机构椭圆
- 钢丝绳连线
- 当前位置与目标位置文本显示
- 上限报警逻辑

**验收标准**
- 动画能随仿真数据变化
- 可区分实际位移与目标位移
- 报警触发后停止仿真

---

### Phase 6：控制与交互
**目标**
- 完成菜单、快捷键、右键菜单
- 完成开始 / 暂停 / 停止控制逻辑
- 使用 `QTimer` 驱动仿真刷新

**包含功能**
- “设置”菜单
- “控制”菜单
- 快捷键：`Ctrl+Alt+F`、`Ctrl+Alt+P`、`Alt+S`、`Alt+P`、`Alt+T`
- 右键弹出控制快捷菜单
- 仿真运行状态管理

**验收标准**
- 菜单与快捷键可正常使用
- 开始 / 暂停 / 停止逻辑正确
- `QTimer` 被正确用于刷新仿真过程

---

### Phase 7：参数保存读取与验收
**目标**
- 保存与读取所有对话框参数
- 支持课程要求的保存文件复现
- 按评分点进行统一验收整理

**包含功能**
- 参数序列化与反序列化
- 保存示例：`StepTest`、`SinTest`
- 为录屏与实验报告准备复现场景

**验收标准**
- 所有关键参数可保存与读取
- 保存文件可复现实验效果
- 满足课程任务中的代码、界面、功能、录屏要求

## 4. 推荐执行顺序
1. Phase 1：基础骨架
2. Phase 2：设置对话框
3. Phase 3：仿真核心
4. Phase 4：曲线可视化
5. Phase 5：动画系统
6. Phase 6：控制与交互
7. Phase 7：参数保存读取与验收

## 5. 单次协作规则
- 每一轮只做 `task/current_task.md` 中定义的一个子任务
- 如果当前任务未验收，不提前进入下一个阶段
- 除非当前任务确有需要，否则不修改无关文件
- 每次完成后，在本文件末尾追加“已完成记录”

## 6. 当前推荐起始任务
- 当前推荐执行：Phase 1 的首个子任务“主窗口 UI 骨架搭建”
- 原因：当前工程仍为空白窗口，后续模块都需要稳定的界面挂载点

## 7. 已完成记录
- 2026-06-09：完成项目任务拆解，建立全局阶段路线图与当前任务卡片维护机制，尚未开始功能实现。
- 2026-06-09：Phase 1 主窗口 UI 骨架搭建。完成主窗口 1200×750 尺寸与标题设置；中心区域左右分栏（左侧 2/3 曲线区 / 右侧 1/3 动画区）；左侧垂直排列三块 QGroupBox 占位（拉力曲线、位移误差曲线、位移曲线）；右侧动画区 QGroupBox 占位；菜单栏预留「设置」「控制」入口；状态栏初始化提示文本。涉及文件：mainwindow.ui、mainwindow.cpp。
- 2026-06-09：Phase 2 设置对话框骨架。新增 `dialogs/basicsettingsdialog.*` 与 `dialogs/paramsettingsdialog.*`，完成“基本设置”与“参数设置”两个对话框的 UI 布局、默认值、参数保存读取占位、范围校验入口与调试操作区；在主窗口中补充“设置/控制”菜单动作及快捷键连接（`Ctrl+Alt+F`、`Ctrl+Alt+P`、`Alt+S`、`Alt+P`、`Alt+T`），状态栏可响应开始/暂停/停止操作。涉及文件：`SolarWing.pro`、`mainwindow.h`、`mainwindow.cpp`、`dialogs/basicsettingsdialog.h`、`dialogs/basicsettingsdialog.cpp`、`dialogs/basicsettingsdialog.ui`、`dialogs/paramsettingsdialog.h`、`dialogs/paramsettingsdialog.cpp`、`dialogs/paramsettingsdialog.ui`。
- 2026-06-10：Phase 3 仿真核心。新增 `simulation/` 核心模块，完成统一仿真数据结构、目标位移策略、PID 控制器、太阳翼动力学模型、Euler/RK2/RK4 三种积分器及 `SolarWingSimulator` 统一单步/整段仿真接口；输出样本包含时间、目标位移、实际位移、误差、拉力、速度，可直接供后续曲线和动画模块使用。涉及文件：`SolarWing.pro`、`simulation/simulationtypes.h`、`simulation/simulationtypes.cpp`、`simulation/pidcontroller.h`、`simulation/pidcontroller.cpp`、`simulation/solarwingmodel.h`、`simulation/solarwingmodel.cpp`、`simulation/integrator.h`、`simulation/integrator.cpp`、`simulation/solarwingsimulator.h`、`simulation/solarwingsimulator.cpp`。
- 2026-06-10：维护协作任务文档。已将 `task/current_task.md` 切换为“建立仿真诊断日志”，目标聚焦定位二次目标异常原因；明确本轮禁止修改动力学模型、边界条件与 PID 参数，要求通过关键时刻日志判断异常更接近目标生成、动力学方向、边界条件或 PID 输出中的哪一类，以便后续 AI 接力。涉及文件：`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 3 仿真诊断日志。新增 `simulation/test_simulation_diagnostics.*`，为二次目标异常场景建立专用诊断入口，并输出 `t=0.0/0.1/0.5/1.0/2.0/4.0` 关键时刻的 `target`、`actual`、`velocity`、`acceleration`、`tension`、`error` 快照；基于日志给出当前诊断结论：异常最接近 **C. 边界条件**，依据是目标正常增长、PID 输出保持非负，而位移被限制在 0 时速度仍持续为负，显示边界状态处理放大了异常。涉及文件：`SolarWing.pro`、`mainwindow.cpp`、`simulation/test_simulation_diagnostics.h`、`simulation/test_simulation_diagnostics.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 3 边界条件修复。修正 `SolarWingSimulator::step()` 的下边界状态处理：当积分结果位移跌破 0 时，同时将位移钳回 0 并清除残留负速度，避免“位移触底但速度继续向下”的状态不一致；复测二次目标诊断日志后，目标值继续正常增长，实际位移不再长期锁死在 0，末端速度不再持续明显为负，说明本轮边界异常已解除。涉及文件：`simulation/solarwingsimulator.cpp`、`simulation/test_simulation_diagnostics.cpp`、`task/current_task.md`、`task/task_history.md`。
