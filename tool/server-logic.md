# vmrp 本地 TCP 服务器逻辑说明

本文档说明 `tool/applist-server.js` 和 `tool/download-server.js` 两个服务器的协议逻辑与实现细节。

---

## 概览

| 文件 | 默认端口 | 对应 dltype | 主要功能 |
|---|---|---|---|
| `applist-server.js` | 13230 | 3 | 向客户端推送应用列表（applist.mrp） |
| `download-server.js` | 13229 | 2 | 向客户端推送单个 MRP 文件下载 |

两台服务器共用相同的连接握手协议与帧格式，均通过环境变量控制行为。

---

## 连接状态机

每个客户端连接经历三个状态：

```
detect  →  handshake  →  data
```

### 1. `detect` — 协议类型识别

收到数据后检查首包内容：

- 以 `CONNECT ` 开头 → CMWAP 隧道模式：等待 `\r\n\r\n` 结束 HTTP CONNECT 头部，回复 `HTTP/1.1 200 Connection established\r\n\r\n`，然后进入 `handshake`。
- 以 `set ` 开头 → 直连模式：直接进入 `handshake`。
- 其他情况 → 报错断开。

### 2. `handshake` — 握手解析

等待以 `\n` 结尾的一行文本，格式为：

```
set dltype=3,listver=0,scrsize=128x160,IMEI=123456789012345,IMSI=460001234567890
```

解析所有 `key=value` 键值对，校验必填字段：`dltype`、`listver`、`scrsize`、`IMEI`、`IMSI`。校验通过后进入 `data` 状态并发送响应。

### 3. `data` — 后续数据记录

握手后客户端发来的额外数据仅打印日志，不做处理。

---

## 帧格式

所有业务数据以帧形式传输，格式如下：

```
[2字节 uint16-BE: 负载总长度] [命令名称(ASCII)] [空格] [数据内容]
```

即：`total = 命令名.length + 1(空格) + 数据.length`，最大帧负载 65535 字节。

### 服务端发出的命令帧

| 命令 | 数据内容 | 说明 |
|---|---|---|
| `code` | Lua 源码字符串 | 客户端执行此代码，注册后续命令处理器 |
| `progress` | 进度数字字符串（如 `"50"`） | 更新客户端进度条显示（0–100） |
| `file` | 二进制文件块 | 下载服务器专用，传输 MRP 文件分块 |
| `done` | `"1"` | 下载服务器专用，通知文件传输完成 |

---

## applist-server.js 详细逻辑

### 握手后响应流程

延迟 `FRAME_DELAY_MS`（默认 300ms）后发送首个 `code` 帧，然后按 `PROGRESS_DELAY_MS`（默认 700ms）间隔依次发送：

```
code(初始 payload)
→ progress(10) → progress(30) → progress(60) → progress(90) → progress(100)
→ code(结束 payload)
```

### code 帧内容

- **初始 payload**（`getCodePayload`）：
  - 优先读取环境变量 `CODE_PAYLOAD_FILE` / `CODE_PAYLOAD_HEX` / `CODE_PAYLOAD`。
  - 否则生成默认调试代码，注册 `cmd.progress` 处理器，打印 dltype/listver。

- **结束 payload**（`getFinishPayload`）：
  - `dltype=3` 且 `DEMO_APPLIST != "0"` → 生成 applist 安装代码（见下文）。
  - 其他情况 → 发送 `net.successexit()` 调用代码。

### Applist 安装 payload（`getApplistInstallPayload`）

生成 Lua 代码，在客户端执行以下操作：
1. 删除旧的 `applist.mrp`。
2. 打开新文件，以 `\DDD`（十进制转义）格式按 512 字节分块写入 MRP 二进制内容。
3. 写完后调用 `recreateApplist(1)` 或 `net.successexit()`。

### MRP 包构建（`makeMrpPackage`）

生成符合 MRPG 格式的 MRP 文件，包含：

```
[240字节头部: 魔数MRPG + 元数据(appname/appid/version/vendor/description)]
[索引区: 每个文件的 名称长度+名称+偏移+大小]
[数据区: 每个文件的 名称长度+名称+数据长度+数据]
```

默认 applist 包内含两个文件：
- `start.mr` — Lua 格式的应用列表源码（`listver`、`list` 数组）
- `icons.bmp` — 图标文件（从 `temp/dsm_gm/lstic.bmp` 读取）

---

## download-server.js 详细逻辑

### 握手后响应流程

延迟 `FRAME_DELAY_MS`（默认 300ms）后：

1. 发送 `code` 帧（下载初始化 Lua 代码）。
2. 延迟 `DOWNLOAD_START_DELAY_MS`（默认 300ms）后开始分块发送文件。

### code 帧内容（`getDownloadInitPayload`）

注入以下 Lua 全局变量和函数，并注册命令处理器：

| 变量/函数 | 说明 |
|---|---|
| `dl_name` | 目标文件名（由 `dlname` 字段或 `DOWNLOAD_TARGET` 环境变量决定） |
| `dl_total` | 文件总字节数 |
| `dl_written` | 已写入字节数，用于最终校验 |
| `cmd.progress` | 更新进度条 |
| `cmd.file` | 接收文件块并写入本地文件 |
| `cmd.done` | 校验写入完整性，调用 `recreateApplist` 或 `net.successexit` |

### 文件分块传输（`sendDownloadFrames`）

- 按 `DOWNLOAD_CHUNK_SIZE`（默认 8192 字节）分块。
- 每块间隔 `DOWNLOAD_CHUNK_DELAY_MS`（默认 40ms）。
- 每块发送 `file` 帧，并同步发送当前进度的 `progress` 帧。
- 全部块发完后发送 `progress(100)` 和 `done(1)`。

### 文件名清理（`cleanMrpName`）

对环境变量或握手字段中的 `dlname` 做安全处理：
- 取路径最后一段（去掉目录部分）。
- 只保留字母、数字、`_`、`.`、`-` 字符，防止路径遍历。
- 自动补全 `.mrp` 后缀。

---

## 共用工具函数

| 函数 | 说明 |
|---|---|
| `u32le(value)` | 将数值编码为 4 字节小端序 Buffer |
| `fixedString(value, len)` | 将字符串填充/截断为定长 ASCII Buffer |
| `parseHandshake(line)` | 解析 `key=value,key=value` 格式的握手行 |
| `escapeMrString(buf)` | 将二进制 Buffer 转为 `\DDD` 十进制转义字符串（用于 Lua 字面量） |
| `chunkBuffer(buf, size)` | 将 Buffer 按固定大小切分为数组 |
| `makeFrame(command, data)` | 构建协议帧 |
| `sendFrame(socket, command, data)` | 发送协议帧并打印日志 |
| `preview(buf, max)` | 生成 Buffer 的 hex + 文本预览字符串（调试用） |

---

## 环境变量参考

### 通用

| 变量 | 默认值 | 说明 |
|---|---|---|
| `PORT` | 13230 / 13229 | 监听端口 |
| `FRAME_DELAY_MS` | 300 | 握手后首帧延迟（毫秒） |
| `CODE_PAYLOAD_FILE` | — | 替换 code 帧内容，读取指定文件 |
| `CODE_PAYLOAD_HEX` | — | 替换 code 帧内容，十六进制字符串 |
| `CODE_PAYLOAD` | — | 替换 code 帧内容，UTF-8 字符串 |

### applist-server.js 专用

| 变量 | 默认值 | 说明 |
|---|---|---|
| `PROGRESS_DELAY_MS` | 700 | 进度帧间隔（毫秒） |
| `DEMO_APPLIST` | — | 设为 `"0"` 禁用 applist 安装流程 |
| `DEMO_PROGRESS` | — | 设为 `"0"` 禁用进度帧 |
| `APPLIST_MRP_FILE` | — | 直接使用指定的 MRP 文件作为 applist |
| `APPLIST_SOURCE_FILE` | — | 替换 applist 的 start.mr 源码文件 |

### download-server.js 专用

| 变量 | 默认值 | 说明 |
|---|---|---|
| `DOWNLOAD_MRP_FILE` | `temp/test.mrp` | 要下载的 MRP 文件路径 |
| `DOWNLOAD_TARGET` | — | 覆盖客户端文件名（优先于握手字段 dlname） |
| `DOWNLOAD_CHUNK_SIZE` | 8192 | 每个 file 帧的字节数 |
| `DOWNLOAD_CHUNK_DELAY_MS` | 40 | 分块发送间隔（毫秒） |
| `DOWNLOAD_START_DELAY_MS` | 300 | code 帧后开始发文件的延迟（毫秒） |
