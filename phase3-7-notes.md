# Phase 3–6 与 Honor：学习和证据笔记

本文由 AI 协助分析本地 `bomb` 汇编、实现求解器并运行测试后整理，不是你独立完成实验的声明，也不要直接当作自己的分析提交。报告请用自己理解后的语言说明过程并披露辅助工具。

## 先理解共用规则

- 下面 `C` 是当前关卡的 64 位 `challenge`，不是 seed。驱动负责把 seed 转成各关不同的 challenge。
- `u32(x)` 表示保留最低 32 位。所有公式中的加、减、乘按无符号 32 位计算，溢出后保留低 32 位。
- AT&T 汇编是“源在前，目的在后”：`mov %edi,%eax` 把 edi 复制到 eax。
- `shr` 是逻辑右移，`xor` 是按位异或，不是乘方。
- `lea (%rax,%rax,2),%eax` 计算 `eax = 3*eax`，这里不是读取内存。
- `call` 调用函数；普通 32 位整数结果看 eax。布尔返回值只需看 al。
- 图片是从真实命令日志生成的排版图，不是桌面终端窗口的原生截图。原始 `.txt` 日志一起保留。

## Phase 3：跳转表

入口要求两个整数。第一个必须是 `C & 7`；第二个来自一次表驱动计算。

```text
s = C & 7
x = u32(C >> 32) ^ u32(C >> 3)
```

`p3_dispatch` 通过 `jmp *表(索引,8)` 跳到对应代码。表中的顺序不等于函数名后缀，因此要读表项，再跟踪跳转目标。

| s | 实际函数 | 第二个整数 |
| --- | --- | --- |
| 0 | case_5 | `(4*x) ^ 0x2d8d` |
| 1 | case_0 | `(x ^ 0x90d2) + (x >> 1)` |
| 2 | case_3 | `5*x + 0x4c54a` |
| 3 | case_7 | `3*x + 0xe0d3` |
| 4 | case_4 | `5*x + 0xbf09` |
| 5 | case_6 | `(x ^ 0xad1c) + (x >> 1)` |
| 6 | case_1 | `3*x + 0x7808` |
| 7 | case_2 | `(4*x) ^ 0xaa22` |

seed 42 的答案为 `4 2704110282`。原来用 `4 0` 调试产生的 BOOM 只是旧的占位输入记录，不是正确解法的运行结果；新的通过日志单独保存。

## Phase 4：递归不是只看最终命中

入口 `phase_04` 在调用 `p4_walk` 前设置四个参数：

```text
target = C & 127
lo = 0
hi = 127
key = u32(C >> 7) ^ u32(C >> 32)
```

在 `p4_walk` 中，`sub/shr/add` 算出中点 `mid = lo + ((hi-lo)>>1)`。`cmp` 后的相等、无符号小于分支决定下一步：

```text
W(target,lo,hi,key):
    mid = lo + ((hi-lo)>>1)
    若 target == mid：返回 (mid + 0x56) ^ key
    若 target < mid：
        child = W(target,lo,mid-1,key)
        返回 (3*child) ^ (mid+key) ^ 0x1b
    否则：
        child = W(target,mid+1,hi,key)
        返回 (3*child) ^ (mid+key) ^ 0x06
```

关键证据：`0x402374` 是左分支的 `xor $0x1b,%eax`，`0x402356` 是右分支的 `xor $0x6,%eax`。递归结束以后，还要逐层计算返回值，不能把命中的中点直接作为答案。

seed 42：`target=114`，`key=0xbdf4c81d`。搜索中点依次为 `63→95→111→119→115→113→114`，再沿调用栈反方向计算，GDB 的最终 eax 为 `1438729157`，与正确输入相等。

## Phase 5：结构体与链表

先看内存访问偏移，而不是猜结构体：

```text
mov (%rdx),%rdx       next 指针在偏移 0，占 8 字节
mov 0x8(%rdx),%esi    value 在偏移 8，占 4 字节
xor 0xc(%rdx),%eax    tag 在偏移 12，占 4 字节
```

节点大小是 16 字节。节点数组和八个起点指针的真实内容保存在 `phase05-full-disassembly.txt`，实现中只把它们转换成节点编号，并没有写死某个 seed 的答案。

```text
node = heads[C & 7]
steps = (C >> 3) & 3
acc = u32(C >> 6) ^ u32(C >> 32) ^ 0x8a88
先处理当前节点：
    acc = (acc ^ node.tag) + node.value
然后沿 next 再走 steps 次，每到一个节点都执行同样运算。
输出 acc 和最后一个节点的 value。
```

注意处理节点总数是 `steps+1`，不是 `steps`。首节点的 tag 也参加异或；异或发生在加 value 之前。

seed 42：起点索引 6，steps 为 0，所以只处理该起点；它的 value 为 `0x43b=1083`，答案为 `917956356 1083`。

## Phase 6：两次间接调用

`call *0x405e00(,%rax,8)` 表示从表中读出函数地址再调用，不是调用表本身。第二次调用使用另一段 challenge 位选出的表项。

```text
x = u32(C >> 6) ^ u32(C >> 32)
i = C & 7
j = (C >> 3) & 7
answer = F[j](F[i](x) ^ 0x60e4) + 0xe780
```

表项顺序为：`op5,op4,op3,op1,op0,op3,op2,op1`。相同函数可以出现在不止一个位置。

| 函数 | 计算关系 |
| --- | --- |
| op0 | `rol32(x,5) ^ 0xd157` |
| op1 | `(x ^ 0xf304) + 8*x` |
| op2 | `ror32(x,5)` |
| op3 | `(4*x + 0x2c06) ^ (7*x)` |
| op4 | `(x ^ (x >> 2)) + 0xa553` |
| op5 | `(x + 0xd9a7) ^ (x >> 3)` |

`rol/ror` 是循环移位，移出去的位会回到另一端，不能用一次普通移位代替。seed 42 的 `i=5,j=3`，最终答案为 `1128525277`。

## Honor：布局门槛与上下文计算

这里只针对本地实验二进制。`secret_gate` 检查输入长度在 36 到 163 字节之间，把内容复制到局部区域后，检查偏移 32 的四个字节是否为 `0x4e45504f`。x86 小端序下对应字符是 `OPEN`。随后从偏移 36 开始解析三个整数。

所以输出布局为：

```text
[32 个填充字符]OPEN[整数 s] [整数 a] [整数 b]
```

这里采用 32 个字符 `0` 作为填充。不是让程序跳转到别的代码，而是满足现有 gate 的字段布局检查。

设 `T` 为前六关答案形成的 `context`：

```text
s = u32(T) & 3
f = u32(C) ^ u32(C >> 32)
h = u32(T) ^ u32(T >> 32)
x = f ^ h
a = ror32(x,1) ^ 0x7b38 ^ (s*0x7a47) ^ u32(T >> 16)
b = a ^ (h + s + 0x17f + (f >> 1))
```

seed 42 的 GDB 记录：

```text
challenge = 0xccf635ee9e9e2fa4
context   = 0x4cb4ff1ac438014f
gate bytes at offset 32: 'O' 'P' 'E' 'N'
payload at offset 36: "3 308913114 2745741094"
```

因此隐藏关不能只用 challenge 算；必须同时使用 context。`secret_phase` 最后的成功条件写入 `al`，不能把 eax 的未定义高位解释为完整的布尔返回值。

## 如何复现

在项目的 WSL 目录执行：

```sh
make
python3 public_test.py
./defuser --seed 314159 --phase 4 | ./bomb --seed 314159 --phase 4
{ ./defuser --seed 314159; ./defuser --seed 314159 --secret; } | ./bomb --seed 314159
```

这里花括号中的两条命令依次输出六行常规答案和一行隐藏关答案。不要只把 `--secret` 的一行输给默认模式的炸弹，它仍然先读取前六关。

完整证据收集命令为 `python3 tools/complete_evidence.py`，定向测试为 `python3 tools/complete_evidence.py --edges`。在 Windows 上执行 `python tools/complete_evidence.py --render` 可把原始日志重新排版成图片。

## 陷阱与结论边界

二进制中的提示性字符串不是可信的解题指令。本次以真实调用路径、跳转表、节点数据和比较指令为依据，并用公开测试、额外 seed 和定向输入交叉验证。没有因为字符串声称某个公式正确就照抄。

已验证通过不代表已运行助教未公开的全部测试。报告仍需你自己理解、整理并标明 AI 辅助；本次没有代替你提交报告、提交 Git commit 或推送 final 标签。
