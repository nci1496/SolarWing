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
- 先建立右侧动画区的可视化骨架
- 再逐步接入实际位移与目标位移
- 最后补齐上限报警与停止钩子

**包含功能**
- `AnimationWidget` 静态绘制卷扬机、钢丝绳、太阳翼
- 接入 `actualDisplacement` 驱动太阳翼高度变化
- 接入 `targetDisplacement` 显示目标位置参考线
- 同屏区分实际位置与目标位置
- 上限报警逻辑与停止接口预留

**验收标准**
- 打开程序即可看到静态太阳翼示意图
- 参数变化并重新仿真后，动画区位置可随实际位移更新
- 可明显区分目标位置与实际位置
- 达到上限条件时可触发报警并停止后续播放

---

### Phase 6：播放控制与交互
**目标**
- 使用 `QTimer` 按样本序列驱动逐步播放
- 在已有播放态基础上完成开始 / 暂停 / 停止
- 保持菜单、快捷键、右键菜单与播放状态一致

**包含功能**
- 基于 `SimulationResult.samples` 的播放索引推进
- `QTimer` 按固定间隔读取 `sample[i]`
- 开始 / 暂停 / 停止状态流转
- “设置”菜单
- “控制”菜单
- 快捷键：`Ctrl+Alt+F`、`Ctrl+Alt+P`、`Alt+S`、`Alt+P`、`Alt+T`
- 右键弹出控制快捷菜单

**验收标准**
- 右侧动画可按样本序列连续播放
- 开始、暂停、停止真正作用于播放过程而非仅提示文本
- 菜单、快捷键、右键菜单可正常驱动播放控制
- `QTimer` 被正确用于刷新仿真播放过程

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
   - 子任务 1：`AnimationWidget` 静态绘制（卷扬机 + 钢丝绳 + 太阳翼）
   - 子任务 2：接入 `actualDisplacement`，太阳翼高度随仿真结果变化
   - 子任务 3：接入 `targetDisplacement`，同屏区分目标位置与实际位置
   - 子任务 4：上限报警与停止钩子
6. Phase 6：播放控制与交互
   - 子任务 1：`QTimer` 样本序列播放骨架
   - 子任务 2：开始 / 暂停 / 停止状态流转
   - 子任务 3：菜单 / 快捷键 / 右键菜单联动
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
- 2026-06-10：Phase 4 曲线绘图控件骨架。新增 `widgets/plotwidget.*`，基于本地 `QCustomPlot` 封装通用曲线控件，支持标题、坐标轴、背景、网格、单曲线/多曲线数据接口与清空接口；主窗口左侧三块占位区域已替换为真实绘图控件，分别用于拉力、误差和位移曲线，其中位移图支持“参考位移/实际位移”双曲线显示，并先接入占位数据用于界面验证。涉及文件：`SolarWing.pro`、`mainwindow.h`、`mainwindow.cpp`、`mainwindow.ui`、`widgets/plotwidget.h`、`widgets/plotwidget.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 4 真实仿真数据接入。主窗口图表初始化已改为执行一次真实 `SolarWingSimulator` 仿真，并从 `SimulationResult` 提取 `time/tension/error/targetDisplacement/actualDisplacement` 序列，分别接入拉力图、误差图与位移双曲线图；保留 `PlotWidget` 通用接口不变，为后续实时刷新和交互扩展预留结构。涉及文件：`mainwindow.h`、`mainwindow.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 4 曲线鼠标交互基础能力。`PlotWidget` 内部新增统一鼠标事件处理：悬停显示十字光标与十字参考线，左键单击时按当前图表数据坐标记录并显示标记点与坐标标签，左键双击时仅清除当前图中的所有标记点；交互逻辑未外溢到主窗口，三幅图共用同一套实现。涉及文件：`widgets/plotwidget.h`、`widgets/plotwidget.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 4 图表浏览体验增强。为 `PlotWidget` 启用 `QCustomPlot` 的滚轮缩放与拖拽平移能力，作用范围限定在当前图表内部；同时将坐标标记触发时机调整为“左键释放且未发生拖拽”时添加，避免与平移操作冲突，并保持十字光标、单击标点、双击清空功能继续可用。涉及文件：`widgets/plotwidget.h`、`widgets/plotwidget.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 4 参数驱动图表重绘。主窗口新增统一的当前仿真参数状态，图表绘制不再依赖初始化时的硬编码默认值；“基本设置/参数设置”修改后会同步回主窗口并重新执行一次 `SolarWingSimulator` 仿真，再将最新结果刷新到拉力、误差与位移三张图，同时保留现有缩放、拖拽与坐标交互。顺手为 `ParamSettingsDialog` 增加 UI 同步保护，避免打开对话框时因控件回写触发无意义重复刷新。涉及文件：`mainwindow.h`、`mainwindow.cpp`、`dialogs/paramsettingsdialog.h`、`dialogs/paramsettingsdialog.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 5 动画骨架静态绘制。新增 `widgets/animationwidget.*` 自绘控件，在右侧动画区域替换原有空白占位，静态绘制卷扬机、钢丝绳与太阳翼示意图；本轮不接入仿真数据、不引入 `QTimer`，仅完成右侧动画区可见化，为后续位移联动与播放控制打底。涉及文件：`SolarWing.pro`、`mainwindow.h`、`mainwindow.cpp`、`widgets/animationwidget.h`、`widgets/animationwidget.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 5 接入 actualDisplacement。为 `AnimationWidget` 增加 `setActualDisplacement(double)` 接口和内部位移状态，在绘制时根据实际位移映射太阳翼垂直位置并同步调整钢丝绳长度；主窗口在每次仿真刷新曲线后，取结果末样本的 `actualDisplacement` 更新右侧静态动画，空结果时回退到 0 位移。当前仍不引入 `QTimer`，仅实现“重算一次、静态更新一次”的结果展示。涉及文件：`mainwindow.cpp`、`widgets/animationwidget.h`、`widgets/animationwidget.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 5 接入 targetDisplacement 参考线。为 `AnimationWidget` 增加 `setTargetDisplacement(double)` 接口和目标位移状态，抽离统一的位移到高度映射逻辑，并在右侧动画区绘制红色虚线“目标位置”参考线；主窗口在每次仿真刷新曲线后，同步将末样本的 `targetDisplacement` 传入动画区，空结果时与实际位移一起复位。当前右侧可同屏区分绿色太阳翼实际位置与红色目标位置参考，仍保持静态结果展示。涉及文件：`mainwindow.cpp`、`widgets/animationwidget.h`、`widgets/animationwidget.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 5 上限报警与停止钩子。为 `AnimationWidget` 增加位移上限阈值、超限状态查询和 `stopRequested()` 信号，右侧动画区新增“上限位置”参考线、超限红色高亮和底部报警提示；主窗口新增可复用的停止钩子状态 `m_stopHookTriggered`，根据当前目标参数计算上限并同步给动画控件，在超限时通过状态栏提示“已触发停止钩子”，同时阻止继续开始播放。当前实现仍不引入 `QTimer`，但已为 Phase 6 的样本播放控制预留可直接复用的停止接口。涉及文件：`mainwindow.h`、`mainwindow.cpp`、`widgets/animationwidget.h`、`widgets/animationwidget.cpp`、`task/current_task.md`、`task/task_history.md`。
- 2026-06-10：Phase 5 上限报警修正。依据课程要求“最高值可自定义、到达后弹出报警消息对话框并停止仿真”，将上限值从目标位移中解耦：在 `参数设置` 中新增独立的 `最高位移` 参数并持久化保存，主窗口改为使用该自定义上限驱动动画区 `Limit` 参考线；超限时弹出 `QMessageBox` 报警并调用停止逻辑，不再把 `Limit` 与 `Target` 绑定为同一位置；同时对两者标签位置做错位绘制，避免视觉重叠。涉及文件：`dialogs/paramsettingsdialog.h`、`dialogs/paramsettingsdialog.cpp`、`dialogs/paramsettingsdialog.ui`、`mainwindow.cpp`、`widgets/animationwidget.cpp`、`task/task_history.md`。
- 2026-06-10：Phase 6 播放控制与交互（子任务 1 + 2）验收通过。主窗口新增 `QTimer` 样本序列播放骨架与缓存状态：`m_playbackTimer`、`m_lastResult`、`m_currentSampleIndex`，`loadSimulationPlots()` 在每次重算后缓存最近一次 `SimulationResult`，右侧动画区不再只显示末样本，而是可按样本顺序推进 `actualDisplacement` / `targetDisplacement`；同时完成开始 / 暂停 / 停止基础流转，暂停可停止播放，停止可中断当前播放流程，并与既有上限报警停止钩子兼容。该阶段已通过人工测试，当前可进入 Phase 6 后续交互细化或整体缺口盘点。涉及文件：`mainwindow.h`、`mainwindow.cpp`、`task/task_history.md`、`task/current_task.md`。
- 2026-06-10：阶段缺口盘点（参考外部 GPT 建议）。当前最大缺口确认为 Phase 7 的“参数保存 / 读取”能力：虽然早期已有保存读取占位与部分参数持久化，但尚未形成课程要求的完整“保存配置文件 / 打开配置文件 / 复现实验场景”闭环，尤其缺少可直接提交的 `StepTest` / `SinTest` 场景文件产出流程。除该必做项外，还记录了若干建议增强项：1）状态栏补充当前时间、位移、误差等实时信息；2）动画区增加目标位移 / 实际位移 / 误差文本显示；3）复核播放结束后的自动停止与状态恢复；4）明确运行中参数修改保护策略。后续实现优先级先进入 Phase 7 参数保存读取，再按时间补强增强项。涉及文件：`task/task_history.md`、`task/current_task.md`。
