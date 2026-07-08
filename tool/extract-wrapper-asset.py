#!/usr/bin/env python3
"""Extract wrapper (cfunction.ext) fingerprint byte assets from MRP fixtures.

Replicates the aex_module.c scanners offline to locate match offsets, then
cuts 4-aligned windows (covering pattern span + LDR literal pool) and emits
them as C arrays for test/unit/wrapper_assets.h.

Usage:
    python3 tool/extract-wrapper-asset.py test/fixtures/*.mrp
"""

from __future__ import annotations

import gzip
import io
import struct
import sys
from pathlib import Path

EXT_NAMES = ("cfunction.ext",)


def mrp_entries(path: Path):
    raw = path.read_bytes()
    if raw[:4] != b"MRPG":
        return
    pos = 240
    size = len(raw)
    while pos + 16 <= size:
        (name_len,) = struct.unpack_from("<I", raw, pos)
        pos += 4
        if name_len == 0 or name_len > 255 or pos + name_len + 12 > size:
            break
        name = raw[pos : pos + name_len].split(b"\0", 1)[0].decode("latin1")
        pos += name_len
        off, packed_len = struct.unpack_from("<II", raw, pos)
        pos += 12
        if off >= size or packed_len > size - off:
            continue
        data = raw[off : off + packed_len]
        if packed_len >= 2 and data[:2] == b"\x1f\x8b":
            try:
                data = gzip.GzipFile(fileobj=io.BytesIO(data)).read()
            except OSError:
                continue
        yield name, data


def u16(code: bytes, off: int) -> int:
    return struct.unpack_from("<H", code, off)[0]


def ldr_literal_u32(code: bytes, insn_off: int):
    if insn_off + 2 > len(code):
        return None
    insn = u16(code, insn_off)
    if (insn & 0xF800) != 0x4800:
        return None
    lit_off = ((insn_off + 4) & ~3) + (insn & 0xFF) * 4
    if lit_off + 4 > len(code):
        return None
    return lit_off, struct.unpack_from("<I", code, lit_off)[0]


TIMER_DISPATCH_PAT = bytes(
    [
        0x00, 0x48, 0xF8, 0xB5, 0x78, 0x44, 0x80, 0x6B,
        0x80, 0x30, 0x40, 0x68, 0x80, 0x47, 0x00, 0x25,
        0x00, 0x4E, 0x4E, 0x44, 0x71, 0x69, 0x75, 0x61,
        0x41, 0x1A, 0xF0, 0x68, 0x0B, 0x1C, 0x00, 0x28,
    ]
)
CHAIN_THUNK_PAT = bytes(
    [0x08, 0xB5, 0xFE, 0xF7, 0x63, 0xFD, 0x00, 0x20, 0x08, 0xBD]
)

FREE_RETURN_FIXED = {
    0x02: 0xB4F0, 0x04: 0x444A, 0x06: 0x6813, 0x08: 0x1C02,
    0x0A: 0x3107, 0x0C: 0x08CC, 0x0E: 0x00E4, 0x16: 0x689D,
    0x1C: 0x691E, 0x2E: 0x3118, 0x30: 0x6998, 0x64: 0x600D,
    0x6C: 0xC114, 0x86: 0x68D8, 0x88: 0x1900, 0x8A: 0x60D8,
    0x8C: 0xBCF0, 0x8E: 0x4770,
}


def scan_timer_dispatch(code: bytes):
    n = len(code)
    for off in range(0, n - len(TIMER_DISPATCH_PAT) + 1, 2):
        if all(
            i in (0, 16) or code[off + i] == TIMER_DISPATCH_PAT[i]
            for i in range(len(TIMER_DISPATCH_PAT))
        ):
            lit = ldr_literal_u32(code, off + 16)
            return off, lit
    return None


def scan_chain_thunk(code: bytes):
    idx = code.find(CHAIN_THUNK_PAT)
    while idx != -1 and idx % 2 != 0:
        idx = code.find(CHAIN_THUNK_PAT, idx + 1)
    return None if idx == -1 else idx


def scan_free_return(code: bytes):
    n = len(code)
    for off in range(0, n - 0x90 + 1, 2):
        ldr = u16(code, off)
        if (ldr & 0xF800) != 0x4800 or ((ldr >> 8) & 7) != 2:
            continue
        lit = ldr_literal_u32(code, off)
        if lit is None:
            continue
        lit_off, ctrl = lit
        if ctrl < 0x80 or ctrl >= 0x1000 or ctrl & 3:
            continue
        if all(u16(code, off + k) == v for k, v in FREE_RETURN_FIXED.items()):
            return off, lit_off, ctrl
    return None


def emit_array(name: str, code: bytes, start: int, end: int, note: str):
    window = code[start:end]
    lines = [f"/* {note} */", f"static const uint8_t {name}[] = {{"]
    for i in range(0, len(window), 12):
        chunk = ", ".join(f"0x{b:02X}" for b in window[i : i + 12])
        lines.append(f"    {chunk},")
    lines.append("};")
    return "\n".join(lines)


def get_ext(mrp: str, ext: str) -> bytes:
    for name, data in mrp_entries(Path(mrp)):
        if name == ext:
            return data
    raise SystemExit(f"{ext} not found in {mrp}")


def cut(code: bytes, match_off: int, spans: list[tuple[int, int]]):
    """4-aligned window covering [match_off+lo, match_off+hi) for all spans."""
    start = min(match_off + lo for lo, _ in spans) & ~3
    end = max(match_off + hi for _, hi in spans)
    end = (end + 3) & ~3
    return start, end, match_off - start


HEADER = """\
/*
 * Wrapper 指纹字节资产 —— 由 tool/extract-wrapper-asset.py --emit 生成,
 * 勿手改。窗口从 4 对齐处切出(LDR literal 解码依赖 (off+4)&~3 对齐),
 * 覆盖指纹跨度与 literal pool。来源 fixture 与原始偏移见各数组注释。
 */
#ifndef WRAPPER_ASSETS_H
#define WRAPPER_ASSETS_H

#include <stdint.h>
"""


def emit(out):
    w = out.write
    w(HEADER)

    def one(cname, mrp, ext, kind):
        code = get_ext(mrp, ext)
        if kind == "timer_dispatch":
            off, lit = scan_timer_dispatch(code)
            lit_off, sched = lit
            spans = [(0, len(TIMER_DISPATCH_PAT)), (lit_off - off, lit_off - off + 4)]
            note_extra = f"sched_off=0x{sched:X}"
        elif kind == "chain_thunk":
            off = scan_chain_thunk(code)
            # 窗口须 >= 主模式 32 字节,否则扫描函数入口即拒绝
            spans = [(-8, len(CHAIN_THUNK_PAT) + 24)]
            note_extra = "chain thunk"
        else:
            off, lit_off, ctrl = scan_free_return(code)
            spans = [(0, 0x90), (lit_off - off, lit_off - off + 4)]
            note_extra = f"ctrl_off=0x{ctrl:X}"
        start, end, rel = cut(code, off, spans)
        note = (
            f"{Path(mrp).name}:{ext} [{start}, {end}) 原始命中偏移 {off},"
            f" 窗口内偏移 {rel};{note_extra}"
        )
        w("\n")
        w(emit_array(cname, code, start, end, note))
        w(f"\nenum {{ {cname}_match_off = {rel} }};\n")

    one("asset_timer_dispatch_gxdzc", "test/fixtures/gxdzc.mrp",
        "cfunction.ext", "timer_dispatch")
    one("asset_timer_dispatch_wbrw", "test/fixtures/wbrw.mrp",
        "cfunction.ext", "timer_dispatch")
    one("asset_chain_thunk_gghjt", "test/fixtures/gghjt.mrp",
        "cfunction.ext", "chain_thunk")
    one("asset_free_return_gxdzc", "test/fixtures/gxdzc.mrp",
        "cfunction.ext", "free_return")
    one("asset_free_return_optwar", "test/fixtures/optwar.mrp",
        "cfunction.ext", "free_return")
    w("\n#endif /* WRAPPER_ASSETS_H */\n")


def main(argv):
    if argv and argv[0] == "--emit":
        with open("test/unit/wrapper_assets.h", "w") as f:
            emit(f)
        print("wrote test/unit/wrapper_assets.h", file=sys.stderr)
        return 0
    for arg in argv:
        path = Path(arg)
        for name, data in mrp_entries(path):
            if not name.lower().endswith(".ext"):
                continue
            td = scan_timer_dispatch(data)
            ct = scan_chain_thunk(data)
            fr = scan_free_return(data)
            print(
                f"{path.name}:{name} len={len(data)} "
                f"timer_dispatch={td} chain_thunk={ct} free_return={fr}",
                file=sys.stderr,
            )
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
