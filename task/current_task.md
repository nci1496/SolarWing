# 当前任务卡片

## 任务名称
Phase 6 子任务 1：QTimer 样本序列播放骨架

## 所属阶段
Phase 6：播放控制与交互

## 当前任务背景
- Phase 5 已完成右侧动画区的静态绘制、`actualDisplacement` 联动、`targetDisplacement` 参考线以及上限报警与停止钩子预留。
- 当前右侧仍是“每次重算后静态显示末样本结果”，还没有逐样本播放能力。
- 下一步需要先建立最小可用的 `QTimer` 播放骨架，让动画区能按 `SimulationResult.samples` 顺序推进。

## 当前任务目标
- 在主窗口建立 `QTimer` 和播放索引等基础状态
- 使用已有 `SimulationResult.samples` 作为播放数据源
- 每次定时器触发时推进一个样本并刷新右侧动画区
- 保持左侧曲线当前行为不被破坏

## 本轮必须完成
1. 在主窗口新增 `QTimer` 播放骨架和样本索引状态
2. 保存最近一次仿真结果，供右侧动画逐样本读取
3. 定时器触发时按顺序把样本中的 `actualDisplacement` / `targetDisplacement` 推给动画区
4. 与现有上限停止钩子兼容，超限时可停止定时器推进

## 本轮明确不做
- 不完善开始/暂停/停止状态机细节
- 不改造菜单和快捷键交互文案
- 不修改左侧曲线交互逻辑
- 不扩展参数保存读取功能

## 建议涉及文件
- `mainwindow.h`
- `mainwindow.cpp`
- `widgets/animationwidget.h`
- `widgets/animationwidget.cpp`
- `task/task_history.md`
- `task/current_task.md`

## 验收标准
- 程序启动后仍可正常显示界面
- 主窗口具备最小 `QTimer` 样本推进能力
- 右侧动画区可按样本顺序更新，不再只显示末样本
- 超限时可触发已有停止钩子并停止推进
- 当前阶段不要求完整播放控制交互体验

## 完成后回写要求
1. 在 `task/task_history.md` 末尾追加本轮完成记录
2. 在本文件中切换到 Phase 6 子任务 2（开始 / 暂停 / 停止状态流转）

## 后续接力提示
下一个 AI 开始前，请先确认：
1. 最近一次仿真结果是如何缓存到主窗口中的
2. `QTimer` 的推进槽函数名称和播放索引变量名称
3. 超限停止时是如何与现有 `stopRequested()` 钩子联动的
