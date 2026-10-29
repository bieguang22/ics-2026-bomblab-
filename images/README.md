# BombLab 图片索引

本目录保存真实汇编、分析和测试记录，以及对应的图片。
图片是日志排版渲染图，不是直接截取桌面终端窗口的原生截图；
新增图片均注明 `Rendered real command log`，原始 `.txt` 同名保留。
图片只有在对应关卡确实完成并通过测试后，才能标为通过证据。

## 命名约定

```text
phaseNN-01-entry-analysis.png    入口、输入数量和最终比较
phaseNN-02-compute-analysis.png  核心函数或数据流
phaseNN-03-helpers-analysis.png  helper、递归、跳转表等关键实现
phaseNN-04-public-tests.png      公开 seed 测试结果
phaseNN-05-extra-seed.png        公开列表之外的 seed 验证
```

`analysis` 图片只证明观察和推理过程，不代表关卡已通过。
`public-tests` 和 `extra-seed` 图片必须来自真实成功运行。

## Phase 1

| 图片 | 内容 | 报告用途 | 当前状态 |
| --- | --- | --- | --- |
| `phase01-01-entry-analysis.png` | `phase_01` 的字段检查、解析、helper 调用和最终比较 | 关键汇编 | 可用 |
| `phase01-02-compute-analysis.png` | `p1_compute` 的 challenge 拆分和 helper 调用顺序 | 推理过程 | 可用 |
| `phase01-03-helpers-analysis.png` | `p1_step0/1/2` 的关键汇编 | 计算关系 | 可用 |
| `phase01-04-seed42-observation.png` | seed 42 的 challenge，以及没有正确输入时的 `BOOM!!!` | 调试过程，不能作为通过结果 | 公开 seed，未通过 |
| `phase01-05-seed42-pass.png` | 手算结果输入炸弹后的 `Phase 1 defused.` | 单个示例验证，可作为辅助证据 | 公开 seed，已通过 |
| `phase01-06-public-tests.png` | `public_test.py --phase 1` 的十个公开 seed 输出 | 公开测试结果 | 10/10 通过 |
| `phase01-07-extra-seed.png` | seed `314159` 的 solver 输出与炸弹通过结果 | 公开列表外 seed 验证 | 已通过 |

Phase 1 的关键汇编、计算关系、公开测试和额外 seed 证据已齐全。

## Phase 2

| 图片 | 内容 | 报告用途 | 当前状态 |
| --- | --- | --- | --- |
| `phase02-01-loop-analysis.png` | 六字段解析、首项检查和五次递推循环 | 关键汇编 | 可用 |
| `phase02-02-seed42-calculation.png` | seed 42 的逐项递推计算 | 计算关系与推理过程 | 可用 |
| `phase02-03-seed42-pass.png` | seed 42 的六个输入与炸弹通过结果 | 单个示例验证 | 已通过 |
| `phase02-04-public-tests.png` | 十个公开 seed 的测试输出 | 公开测试结果 | 10/10 通过 |
| `phase02-05-extra-seed.png` | seed `314159` 的 solver 输出与炸弹结果 | 公开列表外 seed 验证 | 已通过 |

Phase 2 的关键汇编、递推关系、公开测试和额外 seed 证据已齐全。

## Phase 3

| 图片 | 内容 | 报告用途 | 当前状态 |
| --- | --- | --- | --- |
| `phase03-01-entry-analysis.png` | 两字段解析、selector 检查和 dispatch 参数生成 | 关键汇编 | 可用 |
| `phase03-02-jump-table-analysis.png` | 八个真实表项及 selector 到 case 的映射 | 跳转表分析 | 可用 |
| `phase03-03-cases-analysis.png` | 八个 case 的运算公式 | 计算关系 | 可用 |
| `phase03-04-seed42-gdb.png` | seed 42 的 selector、dispatch 参数、实际 case 和返回值 | GDB 动态验证 | 可用，末尾占位输入导致爆炸 |

Phase 3 已补齐实现并通过测试。旧 `phase03-04-seed42-gdb.png` 保留为占位输入的历史调试记录，不能作为通过图片。

新增通过证据：`phase03-seed42-pass-page01.png` 和 `phase03-seed314159-pass-page01.png`。

## Phase 4–6 和 Honor

同一份日志较长时分为 `page01`、`page02` 等多张。每张都有页码，原始全文在同名前缀的 `.txt` 中。

| 编号 | 图片 | 内容与用途 |
| --- | --- | --- |
| P4-A | `phase04-full-disassembly-page*.png` | 入口参数、递归三种分支及返回计算 |
| P4-B | `phase04-seed42-gdb-page*.png` | target、区间、key、最终 eax 与通过输出 |
| P4-C | `phase04-seed42-pass-page01.png` | 公开 seed 42 的求解器答案和真实通过结果 |
| P4-D | `phase04-seed314159-pass-page01.png` | 列表外 seed 验证 |
| P5-A | `phase05-full-disassembly-page*.png` | 链表遍历、结构体字段偏移、真实节点及起点表 |
| P5-B | `phase05-seed42-gdb-page*.png` | challenge、返回校验值与通过输出 |
| P5-C | `phase05-seed42-pass-page01.png` | 两个答案字段及真实通过结果 |
| P5-D | `phase05-seed314159-pass-page01.png` | 列表外 seed 验证 |
| P6-A | `phase06-full-disassembly-page*.png` | 两次间接调用、六种运算、八项指针表 |
| P6-B | `phase06-seed42-gdb-page*.png` | challenge、最终 eax 与通过输出 |
| P6-C | `phase06-seed42-pass-page01.png` | 答案及真实通过结果 |
| P6-D | `phase06-seed314159-pass-page01.png` | 列表外 seed 验证 |
| P7-A | `phase07-full-disassembly-page*.png` | gate、copy、上下文参与计算和最终校验 |
| P7-B | `phase07-seed42-gdb-page*.png` | 偏移 32 的 OPEN、偏移 36 的数字、context、al 成功标志 |
| P7-C | `phase07-seed42-pass-page*.png` | 六关加隐藏关的完整通过过程 |
| P7-D | `phase07-seed314159-pass-page*.png` | 列表外 seed 的完整通过过程 |
| ALL-A | `all-public-tests-page*.png` | 常规六关 60/60，Honor 10/10 |
| ALL-B | `all-additional-tests-page01.png` | 321 个 seed 的完整流程全部通过，其中 315 个在公开列表外 |
| ALL-C | `all-selector-edge-tests-page01.png` | 696 个定向 challenge 测试全部通过 |

`*` 在此表示多页图片的文件名模式，报告插图时要换成具体页码，不能把星号直接写入图片路径。

推荐对应说明见根目录 `phase3-7-notes.md`。例如：

```markdown
![P4 递归调试验证](images/phase04-seed42-gdb-page01.png)
![P5 列表外 seed 验证](images/phase05-seed314159-pass-page01.png)
![Honor 完整通过](images/phase07-seed314159-pass-page01.png)
```

所有新增 GDB 日志和通过记录来自实际运行。批量测试图片为节省空间，省略了中间逐条记录并明确标注；完整列表仍在对应 `.txt` 日志中。公开测试不能等同于助教隐藏测试保证。

## 公开 Seed

`tests/public.json` 中的公开列表是：

```text
0
1
7
42
99
123
2026
65535
987654321
18446744073709551615
```

这些 seed 可以用于公开测试截图，但不能满足报告要求中的
“至少一次公开列表之外的 seed 验证”。
