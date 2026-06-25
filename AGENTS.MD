# AGENTS.MD

本文件约束仓库根目录及其所有子目录。更深层目录若新增自己的 `AGENTS.md` / `AGENTS.MD`，以更深层文件为准。

## 项目概况

- 本项目是 VMRP/MRP 模拟器，主体为 C/CMake 工程。核心目标是模拟 Mythroad/MRP 运行环境，并通过 Unicorn 执行 MRP 包内的 ARM EXT 代码。
- 主要目录：
  - `src/`：模拟器核心、SDL 平台入口、文件/网络/调试桥接、ARM EXT 执行器。
  - `src/mythroad/`：从 `vmrp_arm` 复制过来的 Mythroad/DSM 层，按独立子项目维护。
  - `mrc/`：研究 MRP 编译和格式的辅助资料，不直接参与 VMRP 主程序。
  - `test/`：Vitest e2e 测试、测试夹具和可复现的模拟器回归资源。
  - `wasm/`：WebAssembly 运行资源与构建产物。
  - `third_party/unicorn/`：vendored Unicorn/QEMU 源码，通常只作为上游依赖使用。

## 构建与测试

- 常规 Linux 构建：
  ```bash
  cmake -S . -B build
  cmake --build build
  ```
- 运行 Vitest e2e 测试：
  ```bash
  pnpm test:e2e
  ```
- 共享库构建：
  ```bash
  cmake -S . -B build-shared -DVMRP_BUILD_SHARED=ON
  cmake --build build-shared
  ```
- 仅构建共享库：
  ```bash
  cmake -S . -B build-shared-only -DVMRP_BUILD_SHARED_ONLY=ON
  cmake --build build-shared-only
  ```
- WebAssembly 目标仅在 `emcc` 可用时启用：
  ```bash
  cmake --build build --target vmrp-wasm
  ```
- `test/e2e/` 是面向真实 MRP 样本的 Vitest 回归用例，通常假定可执行文件位于 `build/vmrp`，并依赖 `test/fixtures/*.mrp` 等运行资源。

## 开发要求

- 优先保持现有 C 风格和目录边界，不做无关重构。
- 修改公共 ABI、函数表、结构体布局、事件码、文件句柄、网络回调、定时器、屏幕缓冲或音频接口时，必须明确兼容性影响。
- 避免改动 `third_party/unicorn/`。确实需要修改时，应说明这是上游依赖补丁，并尽量把补丁保持最小。
- `src/mythroad/` 必须按独立 Mythroad/DSM 子项目处理：
  - 不要把 `src/` 外层模拟器实现直接混入该目录。
  - 不要依赖外层 `VMRP` 宏才能让 Mythroad 层正常工作；当前 CMake 会对 Mythroad 源码使用 `-UVMRP`。
  - 保持该目录代码便于将来单独移植到嵌入式或其它平台。
- `mrc/` 主要用于研究和对照，除非任务明确要求，不要把其中实验性代码纳入主构建。
- 不要随意修改或删除 `.mrp`、`.ext`、`.dll`、`.a`、`.exe` 等二进制资源。若必须更新，说明来源和验证方式。
- 代码要有注释，注释要指明修改的原因，兼容性影响和验证步骤。不要只写“修复兼容性问题”或“调整行为”，而不说明具体改动内容和预期效果。要避免修好B问题导致A问题又复现的情况。
- 禁止编写应用特定代码，比如 if(is_xxxx_app()){...}
- 注意trace日志量会特别大。
- 不要使用xvfb。
- 修改代码要加注释，不要有兜底代码逻辑。
- 深入反汇编检测BUG所在位置。
- 你可以检测PPM，来验证修复是否成功。
- 在分析与解决过程中，把进度定期保存至文档中。
- 充分利用子Agent来分析与解决问题。

## 模拟器开发注意事项

- ARM EXT 执行路径对寄存器和内存布局高度敏感。改动 `src/arm_ext_executor.c` 时，要特别关注 R9/R10、GOT 快照、helper 地址、嵌套 EXT、wrapper 栈、低地址表和 Unicorn hook 的一致性。
- Mythroad/MRP 行为优先按真实设备或既有样本兼容，而不是按主机平台“更合理”的语义改写。
- 所有进入 Unicorn 或 Mythroad VM 的调用应保持在主线程串行执行。SDL 定时器线程和自动点击线程只能通过 SDL 事件队列投递事件，不要直接调用模拟器核心。
- 定时器、事件和输入时序属于兼容性的一部分。自动点击脚本中的 delay、按下/释放间隔、软键映射和退出路径不要随意调整。
- 文件系统路径、MRP 包内文件查找、gzip/zip 解压、虚拟文件描述符和大小端解析都要按 MRP 格式处理，不要用主机文件系统假设替代。
- 屏幕默认分辨率为 240x320，但代码应继续支持 `--screen WxH`、`VMRP_SCREEN_WIDTH` 和 `VMRP_SCREEN_HEIGHT`。
- 图形缓冲通常是 RGB565/16 位像素，跨 SDL、PPM dump、Wasm 和 Mythroad 层传递时不要隐式改格式。
- 网络、支付、下载、短信、电话、震动、音频等接口很多是模拟或兼容实现。改动时优先保证已有样本可继续运行，并避免引入阻塞主线程的长时间等待。
- 不要用真实外部服务作为默认测试依赖。需要网络验证时，优先使用仓库内 `tool/` 的本地服务或可控 mock。
- 遇到崩溃或挂起时，先保留可复现输入：MRP 文件、启动参数、环境变量、自动点击序列和关键日志；不要只提交经验性 workaround。

## 验证要求

- 修改构建系统、核心运行时、Mythroad 层、ARM EXT 执行器或公共 API 后，至少运行 `cmake --build build --target vmrp` 和相关 Vitest e2e 用例。
- 修改事件、定时器、UI、支付/下载/退出流程后，优先运行相关 Vitest 用例，例如：
  ```bash
  pnpm vitest run test/e2e/gxdzc-pixel.test.ts
  pnpm vitest run test/e2e/gghjt/game-start.test.ts
  pnpm vitest run test/e2e/dota/download-plugin.test.ts
  ```
- 修改 Wasm 路径时，在有 `emcc` 的环境下构建 `vmrp-wasm`，并确认 `wasm/dist` 资源路径没有被破坏。
- 无法运行某项验证时，在最终说明中写清楚原因和剩余风险。

## 文档与沟通

- 文档可使用中文；代码注释保持简洁，只有在解释兼容性、ABI、寄存器、内存布局或历史 workaround 时才添加。
- 新增或调整模拟器行为时，同步更新相关文档或测试脚本，尤其是环境变量、启动参数、自动点击格式和回归步骤。
- 报告问题时优先给出文件路径、函数名、运行命令、环境变量和复现样本，避免只描述现象。
