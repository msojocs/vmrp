# 冒泡浏览器 Windows DNS 映射排查

日期：2026-07-13

## 现象

Windows 版 Flutter/FFI 运行 MRP 时，怀疑服务端下发的 HOSTS/DNS 映射没有在模拟器网络层生效。受影响样本优先用 `test/fixtures/wbrw.mrp`（冒泡浏览器）验证。

## 当前修改

- `skyengine-shared` 和 Windows 原生 `skyengine` 目标显式启用 `NETWORK_SUPPORT`。此前 MSVC 构建没有该宏，`network.c` 的 socket/DNS API 会直接返回失败，导致 DNS 映射无机会生效。
- `network.c` 增加 Windows `CreateThread` 异步 worker 封装，保持 `mr_connect` / `mr_getHostByName` 的 `MR_WAITING` 回调语义，不把异步路径改成同步路径。
- 新增 `VMRP_NETWORK_LOG=1` 或 `SKYENGINE_LOG=1` 时的关键日志：DNS map 原始字符串、解析后的表项、命中/未命中、`getaddrinfo` 目标和最终 `connect` 目标。
- Flutter 层记录远端 `/config.hosts` 生成的 `dnsMap`，以及启动 MRP 时实际传入 FFI 的 `dnsMap`。

## 服务端映射说明

2026-07-13 实测默认应用商店 `/config` 返回的是线上地址：

```text
freeads.51mrp.com->159.75.119.124
help.proxy.51mrp.com->159.75.119.124
proxy.51mrp.com->159.75.119.124
proxy2.51mrp.com->159.75.119.124
rop.skymobiapp.com->159.75.119.124
spd.skymobiapp.com->159.75.119.124
wap.skmeg.com->159.75.119.124
```

本地调试时把 `proxy2.51mrp.com` 映射到 `127.0.0.1` 只用于复用 `tool/server-http.js` 的冒泡浏览器 proxy2 e2e 服务，不代表线上配置应为 localhost。Windows 问题的关键不是 IP 值，而是 MSVC 共享库未启用 `NETWORK_SUPPORT`，导致线上或本地任何 DNS map 都不会进入 socket/DNS 实现。

## 验证

Windows CLI 冒泡浏览器 smoke 已通过。使用 `build\Debug\skyengine.exe` 启动
`test\fixtures\wbrw.mrp`，显式传入线上 DNS map，日志确认：

```text
[skyengine-net] dns_map[3]: proxy2.51mrp.com -> 159.75.119.124
[skyengine-net] dns_lookup hit: proxy2.51mrp.com -> 159.75.119.124
[skyengine-net] get_host result name='proxy2.51mrp.com' ip=159.75.119.124/0x9F4B777C
[skyengine-net] connect fd=540 target=159.75.119.124:80 ip=0x9F4B777C
[skyengine-net] connect success ip=0x9F4B777C
```

自动像素 e2e 仍是非 Windows 路径：`src/e2e_control.c` 在 Windows 下会走
stub，无法直接复用 `test/e2e/wbrw/boot-to-home.test.ts` 的 Unix socket 控制流程。

已执行的 Windows 构建：

```powershell
& 'D:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' --build build --target skyengine --config Debug
& 'D:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' --build build\windows\x64 --target skyengine-shared --config Debug
flutter build windows --debug
```

已执行的 Flutter 测试：

```powershell
flutter test test\widget_test.dart test\mrp_player_page_test.dart
flutter test test\app_store_api_test.dart
```
