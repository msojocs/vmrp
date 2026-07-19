# SkyEngine

SkyEngine 是一个面向 Mythroad/MRP 应用的开源运行时与模拟器。项目使用本机 C 代码实现外层 Mythroad/DSM 运行环境，并通过 [Unicorn Engine](https://www.unicorn-engine.org/) 执行 MRP 包内的 ARM EXT 代码，因此不要求宿主 CPU 本身是 ARM 架构。

项目当前以 Linux 桌面版和真实 MRP 样本的自动化回归为主要开发基线，同时保留 Windows 桌面、WebAssembly，以及供 Android/Flutter 通过 FFI 集成的共享库构建路径。

> SkyEngine 仍是兼容性研究项目，不是完整复刻所有型号手机和厂商服务的通用 MRP 平台。不同 MRP 对屏幕尺寸、内存、字体、插件、网络服务和平台私有接口的依赖差异很大；仓库内回归样本覆盖的流程不代表所有 MRP 都能正常运行。

## 项目来源

SkyEngine 基于 [VMRP](https://github.com/vmrp/vmrp) 二次开发。感谢 VMRP 原作者及后续贡献者完成的 MRP 格式研究、Mythroad 运行时移植、ARM EXT 执行和跨平台模拟器基础工作。

当前代码仍保留上游的兼容标识：CMake 目标与桌面可执行文件名为 `skyengine`，共享库名为 `libskyengine`，公共 C API 使用 `skyengine_api_*` 前缀，环境变量使用 `SKYENGINE_*` 前缀。这些名称属于现有构建和 ABI 接口，不代表对外项目名称；在完成兼容迁移前不会仅为品牌改名而破坏已有集成。

## 当前能力

- 使用本机版 Mythroad/DSM 层加载 MRP、运行 MR/Lua 代码并管理平台生命周期。
- 使用 vendored Unicorn 模拟 ARM，执行 MRP 内的 `cfunction.ext`、`game.ext` 及嵌套插件 EXT。
- 通过 SDL2 提供桌面窗口、RGB565 画面、鼠标/触摸、功能键、文本输入和音频输出。
- 支持 MIDI 合成，以及 WAV、PCM、MP3 的解码或转换播放。
- 提供文件系统、TCP socket、DNS 映射和部分 CMWAP 兼容行为；浏览器、下载器等样本可以配合 `tool/` 下的本地服务调试。
- 支持 `240x320` 之外的自定义屏幕尺寸，以及 `1/2/4/6/8/16 MB` 应用可见内存档位。
- 提供无 SDL 的 `libskyengine` 共享库，导出生命周期、输入、RGB565/RGBA 画面、文本编辑和 PCM 音频接口。
- 提供 Emscripten WebAssembly 目标和浏览器运行资源。
- 使用 Vitest 驱动真实 MRP 样本，支持输入注入、逐帧等待、PPM 截图和像素断言。

## 架构

```text
MRP 包
  |
  +-- MR/Lua、资源 ----------> 原生 Mythroad/DSM 运行时
  |
  +-- ARM EXT ---------------> ARM EXT 执行器 ---> Unicorn ARM
                                      |
                                      +-- Mythroad/DSM 函数表桥接
                                                   |
                         +-------------------------+-------------------------+
                         |                         |                         |
                      文件/网络                 图形/输入                  音频
                         |                         |                         |
                    宿主操作系统             SDL2 或 FFI             SDL2 或 PCM FFI
```

外层 Mythroad 已直接从 `src/mythroad/` 编译为本机代码；Unicorn 只负责运行 MRP 包内仍为 ARM 机器码的 EXT。ARM EXT 的 R9/R10、GOT、guest 内存、函数表和嵌套模块状态由 `src/arm_ext_executor.c` 与 `src/arm_ext/` 共同维护。

## 平台与构建目标

| 使用场景 | CMake 目标 | 当前说明 |
| --- | --- | --- |
| Linux 桌面 | `skyengine` | 主要开发与 CI 回归环境，使用 SDL2 |
| Windows 桌面 | `skyengine` | 支持 MSVC 构建，CMake 获取 zlib 和 SDL2 |
| Android/Flutter | `skyengine-shared` | 无 SDL 共享库，通过 C API/FFI 集成 |
| Web 浏览器 | `skyengine-wasm` | 检测到 `emcc` 时才创建该目标 |

## 快速开始

### 1. 获取源码

Unicorn 是仓库子模块，必须一并初始化：

```bash
git clone --recurse-submodules https://github.com/open-skyengine/skyengine.git skyengine
cd skyengine
```

已经克隆过仓库时执行：

```bash
git submodule update --init --recursive
```

### 2. Linux 构建

Ubuntu/Debian 可安装以下依赖：

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libsdl2-dev pkg-config zlib1g-dev
```

配置并构建：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target skyengine --parallel
```

从源码目录运行一个 MRP：

```bash
./build/skyengine --work-dir . /path/to/app.mrp
```

不传 MRP 路径时，模拟器默认启动 `dsm_gm.mrp`：

```bash
./build/skyengine --work-dir .
```

### 3. Windows 构建

使用 Visual Studio 的 x64 Native Tools 终端或已配置 MSVC 的 PowerShell：

```powershell
git submodule update --init --recursive
cmake -S . -B build -A x64
cmake --build build --config Release --target skyengine --parallel
```

CMake 配置阶段会获取 zlib 和 SDL2，构建后会把 `SDL2.dll` 复制到可执行文件目录。Visual Studio 生成器的程序通常位于 `build\Release\skyengine.exe`：

```powershell
.\build\Release\skyengine.exe --work-dir . C:\path\to\app.mrp
```

## 运行目录与资源

`--work-dir` 不只是进程的当前目录，也是模拟器映射 MRP 文件系统时使用的根目录。桌面程序默认使用可执行文件所在目录；从源码树运行时建议显式传入 `--work-dir .`。

仓库根目录的 `mythroad` 符号链接指向 `wasm/dist/fs/mythroad`。其中包含默认入口、字体和部分系统插件，例如：

```text
mythroad/
├── dsm_gm.mrp
├── plugins/
│   ├── netpay.mrp
│   ├── flaengine.mrp
│   └── ose/brwcore.mrp
└── system/
    ├── gb12.uc2
    └── gb16.uc2
```

MRP 自己下载或生成的文件也会写入该工作目录下的 `mythroad/`。若应用提示字体、插件或资源不存在，首先检查工作目录是否正确，以及所需文件是否确实存在。项目不会随源码提供所有历史 MRP 应用和厂商在线服务。

构建桌面版 `skyengine` 时，基础字库会自动复制到可执行文件目录下的 `mythroad/system/`，因此直接运行 `build/skyengine /path/to/app.mrp` 也能在默认工作目录中渲染中文文本。

命令行传入的 MRP 会先解析为绝对路径，再切换到工作目录。受旧 Mythroad ABI 限制，最终 MRP 路径必须短于 128 字节；路径过长时请把文件移到更短的位置。

## 命令行

```text
skyengine [OPTIONS] [MRP_PATH] [EXT_NAME] [ENTRY]
```

位置参数：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `MRP_PATH` | `VMRP_MRP` 或 `dsm_gm.mrp` | 要启动的 MRP 文件 |
| `EXT_NAME` | `VMRP_EXT` 或 `start.mr` | 包内启动文件/EXT 名称 |
| `ENTRY` | `VMRP_ENTRY` 或空 | 可选入口名，例如 `_dsm` |

选项：

| 选项 | 说明 |
| --- | --- |
| `--screen WxH` | 设置屏幕分辨率，默认 `240x320` |
| `--memory SIZE` | 设置应用可见内存，只接受 `1M`、`2M`、`4M`、`6M`、`8M`、`16M` |
| `--device-date DATE` | 设置应用可见设备日期，接受 `YYYY-MM-DD` 或 `host`；默认 `2011-01-01` |
| `--work-dir DIR` | 设置运行和 MRP 文件系统的工作目录 |
| `--dns-map MAP` | 设置域名替换规则 |
| `-h`, `--help` | 显示帮助 |

示例：

```bash
# 普通启动
./build/skyengine --work-dir . /path/to/app.mrp

# 横屏应用及更大的应用内存
./build/skyengine --work-dir . --screen 480x320 --memory 4M /path/to/app.mrp

# 需要真实墙钟日期的应用显式使用宿主日期
./build/skyengine --device-date host /path/to/app.mrp

# 显式指定包内启动文件和入口
./build/skyengine --work-dir . /path/to/app.mrp start.mr _dsm
```

等价的常用环境变量如下。命令行选项优先级高于环境变量：

| 环境变量 | 说明 |
| --- | --- |
| `VMRP_MRP` | 默认 MRP 路径 |
| `VMRP_EXT` | 默认包内启动文件，通常为 `start.mr` |
| `VMRP_ENTRY` | 默认入口名 |
| `SKYENGINE_SCREEN_WIDTH` / `SKYENGINE_SCREEN_HEIGHT` | 屏幕宽高 |
| `SKYENGINE_MEMORY` | 应用可见内存档位 |
| `SKYENGINE_WORK_DIR` | 工作目录 |
| `SKYENGINE_DNS_MAP` | DNS 替换规则 |
| `SKYENGINE_LOG` | 启用较详细的运行日志 |
| `SKYENGINE_PPM_PATH` | PPM 截图输出路径，默认 `/tmp/skyengine_screen.ppm` |
| `SKYENGINE_DEVICE_DATE` | 应用可见设备日期：`YYYY-MM-DD` 或 `host`；默认 `2011-01-01` |

### DNS 映射

域名映射让应用请求解析旧域名时，实际解析另一个域名或 IPv4 地址，并把结果返回给应用：

```bash
./build/skyengine --work-dir . \
  --dns-map "old.example->new.example;api.old.example=192.0.2.10" \
  /path/to/app.mrp
```

多条规则可用逗号、分号或换行分隔，`->` 和 `=` 都可作为映射符。当前运行时带有一组面向历史服务的默认映射；需要完全禁用时可显式传入空环境变量：

```bash
SKYENGINE_DNS_MAP="" ./build/skyengine --work-dir . /path/to/app.mrp
```

## 桌面版操作

| MRP 按键 | 键盘 |
| --- | --- |
| 方向键 | 方向键或 `W/A/S/D` |
| 确认/选择 | `Enter` |
| 左功能键 | `Q`、`[` 或 `=` |
| 右功能键 | `E`、`]` 或 `-` |
| 数字键 | `0` 到 `9` 或数字小键盘 |
| `*` / `#` | `*` / `#` |
| 接听键 | `Tab` |
| 挂机/返回键 | `Esc` |

SDL 窗口中的鼠标按下、移动和抬起会转换为 MRP 触摸事件。应用打开文本编辑框后，使用 `Ctrl+V` 粘贴并确认当前剪贴板内容，使用 `Ctrl+Z` 取消输入。

## 共享库与 Flutter

同时构建桌面程序和共享库：

```bash
cmake -S . -B build-shared \
  -DCMAKE_BUILD_TYPE=Release \
  -DSKYENGINE_BUILD_SHARED=ON
cmake --build build-shared --target skyengine-shared --parallel
```

只构建共享库，不配置 SDL 桌面目标和 Wasm 目标：

```bash
cmake -S . -B build-shared-only \
  -DCMAKE_BUILD_TYPE=Release \
  -DSKYENGINE_BUILD_SHARED_ONLY=ON
cmake --build build-shared-only --target skyengine-shared --parallel
```

产物名称为 `libskyengine.so` 或 `skyengine.dll`。公共 ABI 位于 [`src/include/skyengine_api.h`](src/include/skyengine_api.h)，主要包括：

- 初始化、内存/工作目录/DNS/设备日期配置、启动与销毁；
- 输入事件和运行状态；
- RGB565 原始屏幕与 RGBA8888 转换缓冲；
- 文本编辑状态和提交/取消；
- 44.1 kHz、S16LE、双声道 PCM 音频输出。

Android NDK 交叉编译、Gradle 配置和 Dart FFI 示例见 [`docs/flutter-integration.md`](docs/flutter-integration.md)。共享库内部串行调度 VM 事件和定时器；宿主仍应把一次运行实例视为不可并发访问的状态机，并完整管理 `init -> start -> destroy` 生命周期。

共享库同样默认使用 `2011-01-01` 的设备日期。需要真实墙钟日期或其它合法日期时，在 `skyengine_api_start()` 前分别调用 `skyengine_api_set_device_date("host")` 或 `skyengine_api_set_device_date("YYYY-MM-DD")`；非法日期和活动运行期间的调用返回 `-1`，不会改变上一次有效配置。

## WebAssembly

`emcc` 在 `PATH` 中时，CMake 会创建 `skyengine-wasm` 目标：

```bash
source /path/to/emsdk/emsdk_env.sh
cmake -S . -B build-wasm
cmake --build build-wasm --target skyengine-wasm --parallel
```

生成文件位于 `build-wasm/wasm/skyengine.js` 和同目录的 `skyengine.wasm`。`wasm/dist/main.html` 使用 `skyengine.js`、`skyengine.wasm`、`fs.js` 以及 `fs/` 下的运行资源，可按下面的方式组织本地预览：

```bash
cp build-wasm/wasm/skyengine.js build-wasm/wasm/skyengine.wasm wasm/dist/
python3 -m http.server 8000 --directory wasm/dist
```

然后访问 `http://127.0.0.1:8000/main.html`。Wasm 和预加载资源需要通过 HTTP 提供，不能只用 `file://` 打开 HTML。当前 Wasm 路径使用 `wasm/unicorn/libunicorn.a` 中预构建的 unicorn.js 静态库。

## 测试

CI 使用 Node.js 24、pnpm 10 和 Vitest。先构建桌面程序，再安装并运行测试：

```bash
cmake -S . -B build
cmake --build build --target skyengine --parallel

corepack enable
pnpm install --frozen-lockfile
pnpm test:e2e
```

运行单个场景：

```bash
pnpm vitest run test/e2e/gxdzc/gxdzc-pixel.test.ts
pnpm vitest run test/e2e/gghjt/game-start.test.ts
```

E2E 驱动默认使用 SDL 的 `dummy` 视频和音频驱动，不需要 Xvfb。每个测试用例创建独立临时工作目录，通过 Unix socket 向 SDL 主线程发送点击、按键、粘贴、截屏和退出命令，并对 PPM 像素或运行日志做断言。

常用测试环境变量：

| 环境变量 | 说明 |
| --- | --- |
| `VMRP_BIN` | 被测程序路径，默认 `build/skyengine` |
| `VMRP_TIMEOUT_MS` | E2E socket 启动和命令超时 |
| `VMRP_E2E_KEEP_TMP=1` | 测试结束后保留临时目录、截图和日志 |

测试框架与 socket 命令详见 [`docs/TESTING.md`](docs/TESTING.md)。

### Sanitizer 与严格警告

```bash
cmake -S . -B build-san -DSANITIZE=address,undefined
cmake --build build-san --target skyengine --parallel
```

第一方源码默认启用额外警告；可以使用 `-DVMRP_WERROR=ON` 将其提升为错误，或用 `-DSKYENGINE_STRICT_WARNINGS=OFF` 临时关闭。vendored Unicorn 和独立维护的 Mythroad 子项目不受这组严格警告选项影响。

## 已知边界

- 当前兼容性由仓库内真实样本和回归流程逐步扩展，不保证任意 MRP、任意 EXT 编译器产物或任意设备私有 ABI 都可运行。
- 网络 socket 和 DNS 已实现，但依赖已经下线的厂商服务器、WAP 网关、支付平台或特定代理协议的应用仍需要本地替代服务或额外适配。
- 短信、通话、震动、计费和部分原生菜单/对话框接口只做模拟或兼容返回，不会触发真实手机服务。
- 默认屏幕为 `240x320`、应用可见内存为 `1 MB`。一些应用必须使用其原始设备对应的分辨率和内存档位。
- WebAssembly 仍沿用预构建的 unicorn.js 静态库，其更新方式不同于桌面版的 `third_party/unicorn` 子模块。

## 目录结构

| 路径 | 内容 |
| --- | --- |
| `src/` | 模拟器核心、SDL 平台、网络/文件/音频、共享库 API |
| `src/arm_ext/` | 从 ARM EXT 执行器拆出的内存、模块、函数表、屏幕、定时器和诊断单元 |
| `src/mythroad/` | 独立维护的 Mythroad/DSM 运行时源码 |
| `third_party/unicorn/` | Unicorn/QEMU 子模块，桌面版和共享库只构建 ARM 后端 |
| `third_party/minimp3/` | MP3 解码器 |
| `test/e2e/` | Vitest 真实应用回归测试 |
| `test/fixtures/` | 测试 MRP、字体、插件与黄金 PPM 资源 |
| `wasm/` | Emscripten 运行页面、预加载文件和 unicorn.js 静态库 |
| `tool/` | MRP 分析、编辑和本地 HTTP/socket/代理服务工具 |
| `mrc/` | MRP/EXT 格式、函数表和汇编研究资料，不参与主程序构建 |
| `docs/` | 架构、ABI、测试、Flutter 和兼容性分析文档 |

`src/mythroad/` 按可独立移植的子项目维护，不应直接混入外层模拟器的 SDL 或宿主实现。除非在修复明确的上游问题，也应避免直接修改 `third_party/unicorn/`。

## 进一步阅读

- [`docs/arm-ext-executor/`](docs/arm-ext-executor/)：ARM EXT 执行器架构、内存、模块、桥接与调试手册。
- [`docs/arm-ext-abi.md`](docs/arm-ext-abi.md)：ARM EXT ABI、guest 内存和模块状态参考。
- [`docs/mythroad-platform-reference.md`](docs/mythroad-platform-reference.md)：真机 Mythroad 平台行为与模拟器设计依据。
- [`docs/flutter-integration.md`](docs/flutter-integration.md)：共享库和 Flutter FFI 集成。
- [`docs/TESTING.md`](docs/TESTING.md)：E2E 驱动、截图和像素测试。
- [`CHANGELOG.MD`](CHANGELOG.MD)：版本变更记录。

## 相关项目与工具

- [mrpoid2018](https://github.com/Yichou/mrpoid2018)：Android/ARM 上的 MRP 运行实现参考。
- [VMRP](https://github.com/vmrp/vmrp)：SkyEngine 二次开发所基于的上游项目。
- [Unicorn Engine](https://github.com/unicorn-engine/unicorn)：SkyEngine 使用的 CPU 模拟引擎。
- [Ghidra](https://github.com/NationalSecurityAgency/ghidra)：分析 ARM EXT 的常用逆向工具。
- [`tool/Mrpeditor.exe`](tool/Mrpeditor.exe)：仓库内保留的 MRP 编辑器。
- [`tool/de.c`](tool/de.c)：项目早期使用的反汇编辅助工具。

## 许可证

SkyEngine 延续上游许可，使用 [GNU General Public License v3.0](LICENSE) 发布。`third_party/` 中的组件分别遵循其上游许可证；分发二进制时请同时检查对应组件的许可要求。
