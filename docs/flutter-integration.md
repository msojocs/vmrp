# VMRP Flutter 对接指南

本文档说明如何在 Flutter 项目中集成 VMRP，通过 `dart:ffi` 直接调用 C API 运行 MRP 程序。当前工程已覆盖 Android、Windows、Linux 的 VMRP 共享库构建路径；音频播放通过 Flutter 侧统一消费 VMRP 导出的 PCM 流。

## 架构概览

```
┌──────────────────────────────────┐
│  Flutter UI (Dart)               │
│  ┌────────────┐ ┌─────────────┐  │
│  │ VmrpWidget │ │ VmrpEngine  │  │
│  │ (渲染屏幕) │ │ (FFI/调度)  │  │
│  └────────────┘ └──────┬──────┘  │
│                 ┌──────▼──────┐  │
│                 │ VmrpAudio   │  │
│                 │ + SoLoud    │  │
│                 └──────┬──────┘  │
│                        │ dart:ffi│
├────────────────────────┼─────────┤
│  libskyengine.so (C)        │         │
│  ┌─────────────────────▼──────┐  │
│  │ skyengine_api.c (导出API)       │  │
│  │   ↓                        │  │
│  │ skyengine core + mythroad + ARM │  │
│  └────────────────────────────┘  │
└──────────────────────────────────┘
```

**设计要点：**
- 不依赖 JNI/MethodChannel，`dart:ffi` 直接调 C
- 屏幕渲染：C 侧维护 RGB565 像素缓冲区，Dart 侧读取后绘制
- 音频播放：C 侧解码/合成 MIDI/WAV/PCM 为 44.1kHz/S16/stereo PCM，Dart 侧通过 `flutter_soloud` 播放
- 定时器：Dart 侧轮询 C 的 timer interval，自行用 `Timer` 调度
- 单线程模型：所有 skyengine C 调用必须在同一个 isolate 中

---

## 1. 集成到 Flutter 项目

推荐使用 **git submodule** 方式，Flutter 构建时自动编译 `libskyengine.so`，无需手动交叉编译。

### 方式一：Git Submodule（推荐）

#### 1.1 添加子模块

```bash
cd your_flutter_project
git submodule add https://github.com/msojocs/skyengine.git skyengine
git submodule update --init --recursive  # 初始化 skyengine 内部的 unicorn 子模块
```

目录结构：
```
your_flutter_project/
  skyengine/                          ← git submodule
    CMakeLists.txt
    src/
    third_party/unicorn/         ← 嵌套子模块，--recursive 会自动拉取
  android/
    app/
      build.gradle.kts           ← 配置 CMake
  lib/
    skyengine_bindings.dart
    skyengine_engine.dart
```

#### 1.2 配置 android/app/build.gradle.kts

```kotlin
android {
    // ...
    defaultConfig {
        // ...
        ndk {
            abiFilters += listOf("arm64-v8a", "armeabi-v7a")
        }
    }

    externalNativeBuild {
        cmake {
            path = file("../../skyengine/CMakeLists.txt")
            version = "3.18.1+"
        }
    }
}
```

如果使用 Groovy 格式的 `build.gradle`：

```groovy
android {
    defaultConfig {
        ndk {
            abiFilters 'arm64-v8a', 'armeabi-v7a'
        }
        externalNativeBuild {
            cmake {
                arguments '-DSKYENGINE_BUILD_SHARED_ONLY=ON'
            }
        }
    }

    externalNativeBuild {
        cmake {
            path "../../skyengine/CMakeLists.txt"
        }
    }
}
```

关键参数 `-DSKYENGINE_BUILD_SHARED_ONLY=ON`：只构建 `libskyengine.so`，跳过 SDL 可执行文件和测试（Android 环境没有 SDL）。

#### 1.3 构建

```bash
flutter build apk  # 自动触发 CMake 编译 libskyengine.so
```

Flutter 的 Gradle 插件会用 Android NDK 的 CMake toolchain 自动编译，产物直接打包进 APK。

---

### 方式二：预编译 .so（手动）

如果不想用 submodule，也可以手动交叉编译后放入 `jniLibs`：

```bash
export NDK=$HOME/Android/Sdk/ndk/<version>

# arm64-v8a
cmake -B build-android-arm64 \
  -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-21 \
  -DSKYENGINE_BUILD_SHARED_ONLY=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-android-arm64
```

放入 Flutter 项目：
```
your_flutter_project/
  android/
    app/
      src/main/
        jniLibs/
          arm64-v8a/
            libskyengine.so
          armeabi-v7a/
            libskyengine.so
```

---

## 2. Dart FFI 绑定

### skyengine_bindings.dart

```dart
import 'dart:ffi';
import 'dart:io';

// C 函数签名
typedef _skyengine_api_init_C = Int32 Function(Int32, Int32);
typedef _skyengine_api_init_Dart = int Function(int, int);

typedef _skyengine_api_start_C = Int32 Function(Pointer<Utf8>, Pointer<Utf8>, Pointer<Utf8>);
typedef _skyengine_api_start_Dart = int Function(Pointer<Utf8>, Pointer<Utf8>, Pointer<Utf8>);

typedef _skyengine_api_destroy_C = Void Function();
typedef _skyengine_api_destroy_Dart = void Function();

typedef _skyengine_api_event_C = Int32 Function(Int32, Int32, Int32);
typedef _skyengine_api_event_Dart = int Function(int, int, int);

typedef _skyengine_api_timer_C = Int32 Function();
typedef _skyengine_api_timer_Dart = int Function();

typedef _skyengine_api_get_timer_interval_C = Int32 Function();
typedef _skyengine_api_get_timer_interval_Dart = int Function();

typedef _skyengine_api_get_screen_buffer_C = Pointer<Uint16> Function();
typedef _skyengine_api_get_screen_buffer_Dart = Pointer<Uint16> Function();

typedef _skyengine_api_get_screen_dirty_C = Int32 Function();
typedef _skyengine_api_get_screen_dirty_Dart = int Function();

typedef _skyengine_api_get_screen_width_C = Int32 Function();
typedef _skyengine_api_get_screen_width_Dart = int Function();

typedef _skyengine_api_get_screen_height_C = Int32 Function();
typedef _skyengine_api_get_screen_height_Dart = int Function();

typedef _skyengine_api_get_screen_rotation_C = Int32 Function();
typedef _skyengine_api_get_screen_rotation_Dart = int Function();

typedef _skyengine_api_motion_C = Int32 Function(Int32, Int32, Int32);
typedef _skyengine_api_motion_Dart = int Function(int, int, int);

typedef _skyengine_api_motion_active_C = Int32 Function();
typedef _skyengine_api_motion_active_Dart = int Function();

typedef _skyengine_api_is_edit_active_C = Int32 Function();
typedef _skyengine_api_is_edit_active_Dart = int Function();

typedef _skyengine_api_set_edit_text_C = Int32 Function(Pointer<Utf8>);
typedef _skyengine_api_set_edit_text_Dart = int Function(Pointer<Utf8>);

typedef _skyengine_api_cancel_edit_C = Int32 Function();
typedef _skyengine_api_cancel_edit_Dart = int Function();

class VmrpBindings {
  late final DynamicLibrary _lib;

  late final _skyengine_api_init_Dart init;
  late final _skyengine_api_start_Dart start;
  late final _skyengine_api_destroy_Dart destroy;
  late final _skyengine_api_event_Dart event;
  late final _skyengine_api_timer_Dart timer;
  late final _skyengine_api_get_timer_interval_Dart getTimerInterval;
  late final _skyengine_api_get_screen_buffer_Dart getScreenBuffer;
  late final _skyengine_api_get_screen_dirty_Dart getScreenDirty;
  late final _skyengine_api_get_screen_width_Dart getScreenWidth;
  late final _skyengine_api_get_screen_height_Dart getScreenHeight;
  late final _skyengine_api_get_screen_rotation_Dart getScreenRotation;
  late final _skyengine_api_motion_Dart motion;
  late final _skyengine_api_motion_active_Dart motionActive;
  late final _skyengine_api_is_edit_active_Dart isEditActive;
  late final _skyengine_api_set_edit_text_Dart setEditText;
  late final _skyengine_api_cancel_edit_Dart cancelEdit;

  VmrpBindings() {
    _lib = Platform.isAndroid
        ? DynamicLibrary.open('libskyengine.so')
        : DynamicLibrary.process();

    init = _lib.lookupFunction<_skyengine_api_init_C, _skyengine_api_init_Dart>('skyengine_api_init');
    start = _lib.lookupFunction<_skyengine_api_start_C, _skyengine_api_start_Dart>('skyengine_api_start');
    destroy = _lib.lookupFunction<_skyengine_api_destroy_C, _skyengine_api_destroy_Dart>('skyengine_api_destroy');
    event = _lib.lookupFunction<_skyengine_api_event_C, _skyengine_api_event_Dart>('skyengine_api_event');
    timer = _lib.lookupFunction<_skyengine_api_timer_C, _skyengine_api_timer_Dart>('skyengine_api_timer');
    getTimerInterval = _lib.lookupFunction<_skyengine_api_get_timer_interval_C, _skyengine_api_get_timer_interval_Dart>('skyengine_api_get_timer_interval');
    getScreenBuffer = _lib.lookupFunction<_skyengine_api_get_screen_buffer_C, _skyengine_api_get_screen_buffer_Dart>('skyengine_api_get_screen_buffer');
    getScreenDirty = _lib.lookupFunction<_skyengine_api_get_screen_dirty_C, _skyengine_api_get_screen_dirty_Dart>('skyengine_api_get_screen_dirty');
    getScreenWidth = _lib.lookupFunction<_skyengine_api_get_screen_width_C, _skyengine_api_get_screen_width_Dart>('skyengine_api_get_screen_width');
    getScreenHeight = _lib.lookupFunction<_skyengine_api_get_screen_height_C, _skyengine_api_get_screen_height_Dart>('skyengine_api_get_screen_height');
    getScreenRotation = _lib.lookupFunction<_skyengine_api_get_screen_rotation_C, _skyengine_api_get_screen_rotation_Dart>('skyengine_api_get_screen_rotation');
    motion = _lib.lookupFunction<_skyengine_api_motion_C, _skyengine_api_motion_Dart>('skyengine_api_motion');
    motionActive = _lib.lookupFunction<_skyengine_api_motion_active_C, _skyengine_api_motion_active_Dart>('skyengine_api_motion_active');
    isEditActive = _lib.lookupFunction<_skyengine_api_is_edit_active_C, _skyengine_api_is_edit_active_Dart>('skyengine_api_is_edit_active');
    setEditText = _lib.lookupFunction<_skyengine_api_set_edit_text_C, _skyengine_api_set_edit_text_Dart>('skyengine_api_set_edit_text');
    cancelEdit = _lib.lookupFunction<_skyengine_api_cancel_edit_C, _skyengine_api_cancel_edit_Dart>('skyengine_api_cancel_edit');
  }
}
```

---

## 3. VmrpEngine 封装

```dart
import 'dart:async';
import 'dart:ffi';
import 'dart:typed_data';
import 'dart:ui' as ui;
import 'package:ffi/package:ffi.dart';
import 'skyengine_bindings.dart';

/// 事件码（对应 skyengine_api.h 中的 VMRP_* 常量）
class VmrpEvent {
  static const int keyPress = 0;
  static const int keyRelease = 1;
  static const int mouseDown = 2;
  static const int mouseUp = 3;
  static const int mouseMove = 12;
}

/// 按键码
class VmrpKey {
  static const int key0 = 0;
  static const int key1 = 1;
  static const int key2 = 2;
  static const int key3 = 3;
  static const int key4 = 4;
  static const int key5 = 5;
  static const int key6 = 6;
  static const int key7 = 7;
  static const int key8 = 8;
  static const int key9 = 9;
  static const int star = 10;
  static const int pound = 11;
  static const int up = 12;
  static const int down = 13;
  static const int left = 14;
  static const int right = 15;
  static const int power = 16;
  static const int softLeft = 17;
  static const int softRight = 18;
  static const int send = 19;
  static const int select = 20;
}

class VmrpEngine {
  final VmrpBindings _bindings = VmrpBindings();
  final int screenWidth;
  final int screenHeight;

  Timer? _timer;
  bool _running = false;

  /// 屏幕更新时触发，监听者应重新渲染
  final StreamController<void> _onScreenUpdate = StreamController.broadcast();
  Stream<void> get onScreenUpdate => _onScreenUpdate.stream;

  /// 文本编辑请求
  final StreamController<void> _onEditRequest = StreamController.broadcast();
  Stream<void> get onEditRequest => _onEditRequest.stream;

  VmrpEngine({this.screenWidth = 240, this.screenHeight = 320});

  /// 初始化 VMRP 引擎
  int init() {
    return _bindings.init(screenWidth, screenHeight);
  }

  /// 启动 MRP 程序
  /// [mrpPath] 必须是设备上的绝对路径
  int start(String mrpPath, {String ext = 'start.mr', String? entry}) {
    final pPath = mrpPath.toNativeUtf8();
    final pExt = ext.toNativeUtf8();
    final pEntry = entry?.toNativeUtf8() ?? nullptr;

    final ret = _bindings.start(pPath.cast(), pExt.cast(), pEntry.cast());

    malloc.free(pPath);
    malloc.free(pExt);
    if (pEntry != nullptr) malloc.free(pEntry);

    if (ret == 0) {
      _running = true;
      _scheduleTimer();
    }
    return ret;
  }

  /// 发送触摸事件
  void sendTouchDown(int x, int y) {
    _bindings.event(VmrpEvent.mouseDown, x, y);
    _checkState();
  }

  void sendTouchUp(int x, int y) {
    _bindings.event(VmrpEvent.mouseUp, x, y);
    _checkState();
  }

  void sendTouchMove(int x, int y) {
    _bindings.event(VmrpEvent.mouseMove, x, y);
    _checkState();
  }

  /// 发送按键事件
  void sendKeyDown(int keyCode) {
    _bindings.event(VmrpEvent.keyPress, keyCode, 0);
    _checkState();
  }

  void sendKeyUp(int keyCode) {
    _bindings.event(VmrpEvent.keyRelease, keyCode, 0);
    _checkState();
  }

  /// 获取屏幕像素数据（RGBA8888），可直接用于 Canvas 绘制
  Uint8List? getScreenRGBA() {
    final ptr = _bindings.getScreenBuffer();
    if (ptr == nullptr) return null;

    final pixelCount = screenWidth * screenHeight;
    final rgb565 = ptr.asTypedList(pixelCount);
    final rgba = Uint8List(pixelCount * 4);

    for (int i = 0; i < pixelCount; i++) {
      final c = rgb565[i];
      final r = ((c >> 11) & 0x1F) * 255 ~/ 31;
      final g = ((c >> 5) & 0x3F) * 255 ~/ 63;
      final b = (c & 0x1F) * 255 ~/ 31;
      rgba[i * 4 + 0] = r;
      rgba[i * 4 + 1] = g;
      rgba[i * 4 + 2] = b;
      rgba[i * 4 + 3] = 255;
    }
    return rgba;
  }

  /// 确认文本编辑
  void confirmEdit(String text) {
    final pText = text.toNativeUtf8();
    _bindings.setEditText(pText.cast());
    malloc.free(pText);
    _checkState();
  }

  /// 取消文本编辑
  void cancelEdit() {
    _bindings.cancelEdit();
    _checkState();
  }

  /// 销毁引擎
  void dispose() {
    _timer?.cancel();
    _timer = null;
    _running = false;
    _bindings.destroy();
    _onScreenUpdate.close();
    _onEditRequest.close();
  }

  // --- 内部方法 ---

  void _checkState() {
    if (_bindings.getScreenDirty() != 0) {
      _onScreenUpdate.add(null);
    }
    if (_bindings.isEditActive() != 0) {
      _onEditRequest.add(null);
    }
    _scheduleTimer();
  }

  void _scheduleTimer() {
    _timer?.cancel();
    _timer = null;
    if (!_running) return;

    final ms = _bindings.getTimerInterval();
    if (ms > 0) {
      _timer = Timer(Duration(milliseconds: ms), () {
        if (!_running) return;
        _bindings.timer();
        _checkState();
      });
    }
  }
}
```

---

## 4. VmrpWidget 渲染组件

```dart
import 'dart:async';
import 'dart:typed_data';
import 'dart:ui' as ui;
import 'package:flutter/material.dart';
import 'skyengine_engine.dart';

class VmrpWidget extends StatefulWidget {
  final VmrpEngine engine;
  final double scale;

  const VmrpWidget({
    super.key,
    required this.engine,
    this.scale = 2.0,
  });

  @override
  State<VmrpWidget> createState() => _VmrpWidgetState();
}

class _VmrpWidgetState extends State<VmrpWidget> {
  StreamSubscription? _sub;
  ui.Image? _screenImage;

  VmrpEngine get engine => widget.engine;

  @override
  void initState() {
    super.initState();
    _sub = engine.onScreenUpdate.listen((_) => _updateScreen());
  }

  @override
  void dispose() {
    _sub?.cancel();
    _screenImage?.dispose();
    super.dispose();
  }

  Future<void> _updateScreen() async {
    final rgba = engine.getScreenRGBA();
    if (rgba == null) return;

    final completer = Completer<ui.Image>();
    ui.decodeImageFromPixels(
      rgba,
      engine.screenWidth,
      engine.screenHeight,
      ui.PixelFormat.rgba8888,
      (img) => completer.complete(img),
    );

    final img = await completer.future;
    if (mounted) {
      setState(() {
        _screenImage?.dispose();
        _screenImage = img;
      });
    }
  }

  Offset _toMrpCoords(Offset localPos) {
    return Offset(
      localPos.dx / widget.scale,
      localPos.dy / widget.scale,
    );
  }

  @override
  Widget build(BuildContext context) {
    final w = engine.screenWidth * widget.scale;
    final h = engine.screenHeight * widget.scale;

    return GestureDetector(
      onPanStart: (d) {
        final p = _toMrpCoords(d.localPosition);
        engine.sendTouchDown(p.dx.toInt(), p.dy.toInt());
      },
      onPanUpdate: (d) {
        final p = _toMrpCoords(d.localPosition);
        engine.sendTouchMove(p.dx.toInt(), p.dy.toInt());
      },
      onPanEnd: (d) {
        // GestureDetector 不提供 end 坐标，使用最后的 update 坐标
        engine.sendTouchUp(0, 0);
      },
      onTapDown: (d) {
        final p = _toMrpCoords(d.localPosition);
        engine.sendTouchDown(p.dx.toInt(), p.dy.toInt());
      },
      onTapUp: (d) {
        final p = _toMrpCoords(d.localPosition);
        engine.sendTouchUp(p.dx.toInt(), p.dy.toInt());
      },
      child: SizedBox(
        width: w,
        height: h,
        child: CustomPaint(
          painter: _VmrpPainter(_screenImage, widget.scale),
          size: Size(w, h),
        ),
      ),
    );
  }
}

class _VmrpPainter extends CustomPainter {
  final ui.Image? image;
  final double scale;

  _VmrpPainter(this.image, this.scale);

  @override
  void paint(Canvas canvas, Size size) {
    if (image == null) {
      canvas.drawRect(Rect.fromLTWH(0, 0, size.width, size.height),
          Paint()..color = Colors.black);
      return;
    }
    canvas.save();
    canvas.scale(scale, scale);
    canvas.drawImage(image!, Offset.zero, Paint()..filterQuality = FilterQuality.none);
    canvas.restore();
  }

  @override
  bool shouldRepaint(_VmrpPainter old) => image != old.image;
}
```

---

## 5. 使用示例

```dart
import 'package:flutter/material.dart';
import 'skyengine_engine.dart';
import 'skyengine_widget.dart';

class MrpPlayerPage extends StatefulWidget {
  final String mrpPath;
  final int screenWidth;
  final int screenHeight;

  const MrpPlayerPage({
    super.key,
    required this.mrpPath,
    this.screenWidth = 240,
    this.screenHeight = 320,
  });

  @override
  State<MrpPlayerPage> createState() => _MrpPlayerPageState();
}

class _MrpPlayerPageState extends State<MrpPlayerPage> {
  late final VmrpEngine _engine;

  @override
  void initState() {
    super.initState();
    _engine = VmrpEngine(
      screenWidth: widget.screenWidth,
      screenHeight: widget.screenHeight,
    );
    _engine.init();
    _engine.start(widget.mrpPath);

    // 监听文本编辑请求
    _engine.onEditRequest.listen((_) => _showEditDialog());
  }

  @override
  void dispose() {
    _engine.dispose();
    super.dispose();
  }

  Future<void> _showEditDialog() async {
    final result = await showDialog<String>(
      context: context,
      builder: (ctx) {
        final controller = TextEditingController();
        return AlertDialog(
          title: const Text('输入'),
          content: TextField(controller: controller, autofocus: true),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(ctx),
              child: const Text('取消'),
            ),
            TextButton(
              onPressed: () => Navigator.pop(ctx, controller.text),
              child: const Text('确定'),
            ),
          ],
        );
      },
    );

    if (result != null) {
      _engine.confirmEdit(result);
    } else {
      _engine.cancelEdit();
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('VMRP')),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            VmrpWidget(engine: _engine, scale: 2.0),
            const SizedBox(height: 16),
            // 虚拟按键（可选）
            _buildKeypad(),
          ],
        ),
      ),
    );
  }

  Widget _buildKeypad() {
    return Row(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        _keyButton('左软键', VmrpKey.softLeft),
        _keyButton('上', VmrpKey.up),
        _keyButton('右软键', VmrpKey.softRight),
      ],
    );
  }

  Widget _keyButton(String label, int keyCode) {
    return GestureDetector(
      onTapDown: (_) => _engine.sendKeyDown(keyCode),
      onTapUp: (_) => _engine.sendKeyUp(keyCode),
      child: Container(
        margin: const EdgeInsets.all(4),
        padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
        decoration: BoxDecoration(
          color: Colors.grey[800],
          borderRadius: BorderRadius.circular(4),
        ),
        child: Text(label, style: const TextStyle(color: Colors.white)),
      ),
    );
  }
}
```

---

## 6. MRP 文件存放

MRP 文件需要放在设备可访问的路径。推荐方式：

```dart
import 'package:path_provider/path_provider.dart';

// 将 MRP 文件从 assets 复制到应用目录
Future<String> prepareMrpFile(String assetName) async {
  final dir = await getApplicationDocumentsDirectory();
  final mrpDir = Directory('${dir.path}/mythroad');
  if (!await mrpDir.exists()) {
    await mrpDir.create(recursive: true);
  }
  final file = File('${mrpDir.path}/$assetName');
  if (!await file.exists()) {
    final data = await rootBundle.load('assets/mrp/$assetName');
    await file.writeAsBytes(data.buffer.asUint8List());
  }
  return file.path;
}
```

VMRP 的工作目录（文件 I/O 根目录）是 MRP 文件所在的目录。

---

## 7. 注意事项

### 线程安全
所有 `skyengine_api_*` 调用必须在**同一线程**中执行。VMRP 内部的 Unicorn ARM 引擎不支持并发访问。在 Flutter 中，确保所有调用都在主 isolate 的同步代码中（`dart:ffi` 调用本身是同步的）。

### 分辨率
常见 MRP 分辨率：
- 240x320（主流）
- 176x220
- 128x160

通过 `skyengine_api_init(width, height)` 设置。分辨率必须在 `start()` 之前设定。

### 屏幕旋转（横屏游戏）
部分游戏（如 gtcm/贪吃猫，面向 320x480 竖屏真机）运行期经
`mr_plat(101, param)` 请求 LCD 旋转（`MR_LCD_ROTATE_*`：0=正常，1=90°，
2=180°，3=270°），并可在进入付费/下载等插件界面前撤销旋转再恢复。
旋转是**运行期状态**，宿主需在每次 dirty 帧后复查：

- `skyengine_api_get_screen_rotation()`：返回当前旋转（0..3）
- `skyengine_api_get_screen_width()` / `skyengine_api_get_screen_height()`：**旋转感知**，
  奇数旋转（90°/270°）时返回 `init` 面板尺寸的转置（如 320x480 → 480x320），
  屏幕缓冲区行宽同步按该宽度解释
- 总像素数在转置下不变，`getScreenBuffer()` 指针保持有效，无需重新 `init`

Dart FFI 绑定：

```dart
typedef _skyengine_api_get_screen_rotation_C = Int32 Function();
typedef _skyengine_api_get_screen_rotation_Dart = int Function();
// ...
getScreenRotation = _lib.lookupFunction<_skyengine_api_get_screen_rotation_C,
    _skyengine_api_get_screen_rotation_Dart>('skyengine_api_get_screen_rotation');
```

VmrpEngine 轮询处（每次 dirty 帧后）：

```dart
if (_bindings.getScreenDirty() != 0) {
  final w = _bindings.getScreenWidth();
  final h = _bindings.getScreenHeight();
  if (w != _lastW || h != _lastH) {
    // 旋转发生:按新尺寸重建 Image/纹理与触摸坐标映射,
    // 触摸坐标始终以当前 w x h 画布为准(1:1,无需变换)。
    _lastW = w; _lastH = h;
    _rebuildTexture(w, h);
  }
  _presentFrame(w, h);
}
```

### 动感芯片（加速度传感器）
部分游戏（如 gtdgdq）经 `mr_plat(4001~4006)` 使用动感芯片（SKYENGINE
《动感芯片接口》），监听开启后由 `MR_MOTION_EVENT` 上送加速度样本：

- `skyengine_api_motion_active()`：guest 监听状态，-1=未监听（**应关闭平台传感
  器省电**），0=晃动模式，1=倾斜模式；轮询风格同 `getScreenDirty()`
- `skyengine_api_motion(x, y, z)`：注入重力加速度分量，取值 **±1000**
  （plat(4006) 向游戏通告的量程）；guest 未监听时样本被忽略

坐标系（设备坐标，见《动感芯片接口》）：手机平放屏幕朝上 → +Z 最大；屏幕
向左横立 → +X 最大；屏幕背向自己竖立 → +Y 最大。Android/iOS 传感器数据换
算：`x = accel_x / 9.8 * 1000` 依次类推（注意平台轴向差异需按上述定义映射）。

```dart
// 传感器推送(示例:sensors_plus 包)
StreamSubscription? _accelSub;
void _pollMotionState() {
  final mode = _bindings.motionActive();
  if (mode >= 0 && _accelSub == null) {
    _accelSub = accelerometerEventStream().listen((e) {
      // 按《动感芯片接口》轴向定义换算到 ±1000
      _bindings.motion(
        (e.x / 9.8 * 1000).round(),
        (e.y / 9.8 * 1000).round(),
        (e.z / 9.8 * 1000).round(),
      );
    });
  } else if (mode < 0 && _accelSub != null) {
    _accelSub!.cancel();
    _accelSub = null;
  }
}
```

### 震动马达
游戏经 `mr_startShake(ms)` / `mr_stopShake()` 控制振动器（SKYENGINE 手册
mr_startShake.md）。C 侧只记录最新请求，嵌入端轮询取走后调平台振动器：

- `skyengine_api_take_shake()`：取走并清除请求（轮询风格同 `getScreenDirty()`，
  建议随 dirty 帧/timer 后复查）
  - `0`：无新请求
  - `>0`：开始震动 N 毫秒
  - `-1`：停止震动
- 连续 start/stop 只保留最后一次（与真机马达"后到指令决定状态"一致）

```dart
// FFI 绑定
typedef _skyengine_api_take_shake_C = Int32 Function();
typedef _skyengine_api_take_shake_Dart = int Function();
// takeShake = _lib.lookupFunction<...>('skyengine_api_take_shake');

// 轮询处(示例:vibration 包;HapticFeedback 仅短促反馈不支持时长)
void _pollShake() {
  final req = _bindings.takeShake();
  if (req > 0) {
    Vibration.vibrate(duration: req);
  } else if (req < 0) {
    Vibration.cancel();
  }
}
```

### 屏幕缓冲区格式
- RGB565，16-bit per pixel，little-endian
- 行优先存储，大小 = `width * height * 2` 字节（width/height 为旋转感知
  getter 的当前返回值）
- 上面的 `getScreenRGBA()` 示例中已包含 RGB565 → RGBA8888 转换

### 定时器模型
MRP 程序通过 `timerStart(ms)` 请求定时回调。C API 不自行创建线程，而是暴露 `skyengine_api_get_timer_interval()` 让宿主调度。典型流程：

```
start() → check getTimerInterval() → 非0 → Timer(ms) → timer()
                                                          ↓
                                            check getTimerInterval() → 循环
```

每次 `event()` / `timer()` 调用后都应检查 `getTimerInterval()` 和 `getScreenDirty()`。

### 音频模型
MRP 的 `mr_playSound()` 在 C 侧接收完整内存音频数据。Flutter 共享库不直接打开 SDL 音频设备，而是把支持的 MIDI/WAV/PCM 转成固定格式 PCM，由宿主拉流播放：

- `skyengine_api_audio_sample_rate()`：当前固定返回 `44100`
- `skyengine_api_audio_channels()`：当前固定返回 `2`
- `skyengine_api_audio_is_active()`：有正在播放的声音时返回 `1`
- `skyengine_api_audio_render_s16le(buffer, frames)`：向 `buffer` 写入 signed 16-bit little-endian stereo PCM，返回实际帧数
- `skyengine_api_audio_stop()`：停止当前声音并清空音频状态

Dart 侧的 `VmrpAudioPlayer` 会在 `start()`、输入事件和 timer 回调之后检查 `audio_is_active`，再通过 `flutter_soloud` 的 `BufferStream` 持续写入 PCM 数据。这样 Android、Windows、Linux 等 Flutter 平台共用同一套 VMRP 解码/合成逻辑，平台差异留给 Flutter 音频插件处理。

当前支持：
- `MR_SOUND_MIDI`：标准 SMF MIDI，使用 VMRP 内置简化合成器
- `MR_SOUND_WAV`：RIFF/WAVE PCM 或 float PCM
- `MR_SOUND_PCM`：按 Mythroad ABI 处理为 8kHz/16-bit/mono PCM

当前不支持 MP3、M4A、AMR、AMR-WB。遇到这些压缩格式时 C API 返回失败并打印日志，避免无声成功造成兼容性误判。

### ProGuard / R8
如果使用了代码混淆，确保 JNI 库加载不受影响（`dart:ffi` 的 `DynamicLibrary.open` 不经过 JNI，一般不受影响）。
