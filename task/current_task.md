# 当前任务卡片

## 任务名称
Phase 7 子任务 1：参数保存 / 读取（实验场景复现）

## 所属阶段
Phase 7：参数保存读取与验收

## 当前任务背景
- Phase 5 动画系统与上限报警已完成并通过验收。
- Phase 6 的 QTimer 播放骨架与基础开始 / 暂停 / 停止流转已经完成，并已通过人工测试。
- 当前最大缺口是课程要求的“保存对话框中所有参数，以及读取保存参数后可复现实验结果”。
- 项目里虽已有部分参数持久化或保存读取占位，但还没有形成面向用户的完整“保存配置文件 / 打开配置文件 / 复现实验场景”闭环。

## 当前任务目标
- 在菜单中提供保存配置与读取配置入口
- 将当前仿真相关参数保存到独立配置文件
- 从配置文件恢复参数、刷新对话框与主窗口仿真结果
- 为后续生成 `StepTest` / `SinTest` 保存文件打基础

## 本轮必须完成
1. 盘点当前哪些参数已经能从对话框结构中完整拿到
2. 设计统一的配置文件格式（优先 `QSettings` 的 ini 文件或 JSON）
3. 实现“保存配置”功能
4. 实现“打开配置”功能
5. 读取成功后同步刷新对话框和仿真结果

## 本轮明确不做
- 本轮先不制作最终的 `StepTest` / `SinTest` 两个交付文件
- 先不补状态栏实时信息增强
- 先不补动画区数字文本增强

## 建议涉及文件
- `mainwindow.h`
- `mainwindow.cpp`
- `dialogs/basicsettingsdialog.h`
- `dialogs/basicsettingsdialog.cpp`
- `dialogs/paramsettingsdialog.h`
- `dialogs/paramsettingsdialog.cpp`
- `task/task_history.md`
- `task/current_task.md`

## 验收标准
- 菜单中可触发保存配置与打开配置
- 保存文件可独立存在于项目外部或指定目录
- 打开配置后，参数设置对话框中的值与主窗口仿真结果同步恢复
- 后续可以基于该能力产出 `StepTest` / `SinTest`

## 完成后回写要求
1. 在 `task/task_history.md` 末尾追加本轮完成记录
2. 在本文件中切换到 Phase 7 子任务 2（生成 StepTest / SinTest 并做最终验收准备）

## 后续接力提示
下一个 AI 开始前，请先确认：
1. 配置文件格式最终采用 `ini` 还是 `json`
2. 基本设置与参数设置中哪些字段已经具备 getter / setter
3. 读取配置后是否需要立即自动重算仿真并刷新动画
4. 课程提交所需的 `StepTest` / `SinTest` 文件命名位置
