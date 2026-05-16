#!/usr/bin/env python3
"""
Parse and inspect MythRoad .mr bytecode chunks.

Default target:
    temp/dsm_gm/tcpip.mr

Examples:
    python3 tool/analyze-mr.py
    python3 tool/analyze-mr.py --summary
    python3 tool/analyze-mr.py --focus recvbuf
    python3 tool/analyze-mr.py --disasm CmdRun
    python3 tool/analyze-mr.py temp/dsm_gm/commonv2.mr --focus progbar
"""

from __future__ import annotations

import argparse
import struct
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any


DEFAULT_FILE = "temp/dsm_gm/tcpip.mr"
MAXSTACK = 250
MAXARG_SBX = (1 << 18) // 2 - 1

OP_NAMES = """
MOVE LOADK LOADBOOL LOADNIL GETUPVAL GETGLOBAL GETTABLE SETGLOBAL SETUPVAL
SETTABLE NEWTABLE SELF ADD SUB MUL DIV POW UNM NOT CONCAT JMP EQ LT LE TEST
CALL TAILCALL RETURN FORLOOP TFORLOOP TFORPREP SETLIST SETLISTO CLOSE CLOSURE
BITNOT BITAND BITOR BITXOR
""".split()


@dataclass
class LocalVar:
    name: str | None
    startpc: int
    endpc: int


@dataclass
class Proto:
    path: str
    offset: int
    source: str | None
    line: int
    nups: int
    numparams: int
    vararg: int
    maxstack: int
    lines: list[int] = field(default_factory=list)
    locals: list[LocalVar] = field(default_factory=list)
    upvalues: list[str | None] = field(default_factory=list)
    constants: list[Any] = field(default_factory=list)
    children: list["Proto"] = field(default_factory=list)
    code: list[int] = field(default_factory=list)


class Reader:
    def __init__(self, data: bytes) -> None:
        self.data = data
        self.pos = 0

    def u8(self) -> int:
        value = self.data[self.pos]
        self.pos += 1
        return value

    def u32(self) -> int:
        value = struct.unpack_from("<I", self.data, self.pos)[0]
        self.pos += 4
        return value

    def i32(self) -> int:
        value = struct.unpack_from("<i", self.data, self.pos)[0]
        self.pos += 4
        return value

    def string(self) -> str | None:
        size = self.u32()
        if size == 0:
            return None

        raw = self.data[self.pos:self.pos + size]
        self.pos += size
        if raw.endswith(b"\0"):
            raw = raw[:-1]

        # Most DSM resources in this repo are GBK/GB18030-oriented.
        return raw.decode("gb18030", errors="replace")


def ins_fields(ins: int) -> tuple[int, int, int, int, int, int]:
    op = ins & 0x3F
    a = (ins >> 24) & 0xFF
    c = (ins >> 6) & 0x1FF
    b = (ins >> 15) & 0x1FF
    bx = (ins >> 6) & 0x3FFFF
    sbx = bx - MAXARG_SBX
    return op, a, b, c, bx, sbx


def load_proto(reader: Reader, parent_source: str | None = None, path: str = "0") -> Proto:
    offset = reader.pos
    source = reader.string() or parent_source
    proto = Proto(
        path=path,
        offset=offset,
        source=source,
        line=reader.i32(),
        nups=reader.u8(),
        numparams=reader.u8(),
        vararg=reader.u8(),
        maxstack=reader.u8(),
    )

    proto.lines = [reader.i32() for _ in range(reader.u32())]
    proto.locals = [
        LocalVar(reader.string(), reader.i32(), reader.i32())
        for _ in range(reader.u32())
    ]
    proto.upvalues = [reader.string() for _ in range(reader.u32())]

    for _ in range(reader.u32()):
        tag = reader.u8()
        if tag == 0:
            proto.constants.append(None)
        elif tag == 3:
            # This tree uses USE_INT in mr_user_number.h.
            proto.constants.append(reader.i32())
        elif tag == 4:
            proto.constants.append(reader.string())
        else:
            raise ValueError(f"bad constant tag {tag} at 0x{reader.pos - 1:x} in proto {path}")

    proto.children = [
        load_proto(reader, proto.source, f"{path}.{index}")
        for index in range(reader.u32())
    ]
    proto.code = [reader.u32() for _ in range(reader.u32())]
    return proto


def parse_file(path: Path) -> tuple[int, int, Proto]:
    data = path.read_bytes()
    if not data.startswith(b"\x1bMRP"):
        raise ValueError(f"{path} does not start with MRP signature")

    reader = Reader(data)
    reader.pos = 4
    version = reader.u8()
    endian = reader.u8()
    root = load_proto(reader)

    if reader.pos != len(data):
        print(f"[warn] parsed 0x{reader.pos:x} bytes, file length is 0x{len(data):x}")

    return version, endian, root


def walk(proto: Proto):
    yield proto
    for child in proto.children:
        yield from walk(child)


def quote(value: Any) -> str:
    return repr(value)


def rk(proto: Proto, value: int) -> str:
    if value >= MAXSTACK:
        index = value - MAXSTACK
        return f"K{index}={quote(proto.constants[index]) if index < len(proto.constants) else '?'}"
    return f"R{value}"


def map_names(root: Proto) -> dict[str, str]:
    names = {"0": "<root>"}
    reg: dict[int, str] = {}

    for ins in root.code:
        op, a, b, c, bx, _sbx = ins_fields(ins)

        if op == 5:  # GETGLOBAL
            reg[a] = f"G.{quote(root.constants[bx])}"
        elif op == 10:  # NEWTABLE
            reg[a] = "{}"
        elif op == 34:  # CLOSURE
            reg[a] = f"proto {bx} path 0.{bx}"
        elif op == 7 and reg.get(a, "").startswith("proto "):  # SETGLOBAL
            names[reg[a].split()[-1]] = str(root.constants[bx])
        elif op == 9 and c < MAXSTACK and reg.get(c, "").startswith("proto ") and b >= MAXSTACK:
            names[reg[c].split()[-1]] = str(root.constants[b - MAXSTACK])

    return names


def disassemble(proto: Proto, names: dict[str, str]) -> None:
    print(
        f"\nFUNC {proto.path} {names.get(proto.path, '')} "
        f"off=0x{proto.offset:x} line={proto.line} params={proto.numparams} "
        f"stack={proto.maxstack} code={len(proto.code)} subs={len(proto.children)}"
    )
    print("locals:", [(v.name, v.startpc, v.endpc) for v in proto.locals])
    print("upvalues:", proto.upvalues)
    print("consts:", ", ".join(f"{i}:{quote(v)}" for i, v in enumerate(proto.constants)))

    for pc, ins in enumerate(proto.code, start=1):
        op, a, b, c, bx, sbx = ins_fields(ins)
        opname = OP_NAMES[op] if op < len(OP_NAMES) else f"OP{op}"
        extra = ""

        if op in (1, 5, 7):  # LOADK, GETGLOBAL, SETGLOBAL
            extra = f" ; {quote(proto.constants[bx]) if bx < len(proto.constants) else '?'}"
        elif op in (6, 9, 11, 12, 13, 14, 15, 16, 19, 21, 22, 23, 36, 37, 38):
            extra = f" ; {rk(proto, b)} {rk(proto, c)}"
        elif op in (20, 28, 30):  # JMP/FORLOOP/TFORPREP
            extra = f" ; -> {pc + 1 + sbx}"
        elif op == 25:  # CALL
            args = b - 1 if b else "top"
            rets = c - 1 if c else "top"
            extra = f" ; call R{a} args={args} rets={rets}"
        elif op == 34:
            extra = f" ; proto {bx} path {proto.path}.{bx}"

        print(f"{pc:04d} {opname:<10} A={a:<3} B={b:<3} C={c:<3} Bx={bx:<6} sBx={sbx:<6}{extra}")


def matches(proto: Proto, name: str, names: dict[str, str]) -> bool:
    if proto.path == name or names.get(proto.path) == name:
        return True
    lowered = name.lower()
    return lowered in proto.path.lower() or lowered in names.get(proto.path, "").lower()


def focus_hit(proto: Proto, pattern: str, names: dict[str, str]) -> bool:
    lowered = pattern.lower()
    if lowered in proto.path.lower() or lowered in names.get(proto.path, "").lower():
        return True
    for value in proto.constants:
        if isinstance(value, str) and lowered in value.lower():
            return True
    for local in proto.locals:
        if local.name and lowered in local.name.lower():
            return True
    for upvalue in proto.upvalues:
        if upvalue and lowered in upvalue.lower():
            return True
    return False


def print_summary(root: Proto, names: dict[str, str]) -> None:
    for proto in walk(root):
        display = names.get(proto.path, "")
        print(
            f"{proto.path:<8} {display:<18} off=0x{proto.offset:04x} "
            f"line={proto.line:<4} params={proto.numparams} stack={proto.maxstack:<3} "
            f"code={len(proto.code):<4} k={len(proto.constants):<3} subs={len(proto.children)}"
        )


def print_focus(root: Proto, names: dict[str, str], pattern: str) -> None:
    for proto in walk(root):
        if not focus_hit(proto, pattern, names):
            continue

        print(
            f"\nFUNC {proto.path} {names.get(proto.path, '')} "
            f"off=0x{proto.offset:x} line={proto.line} params={proto.numparams} "
            f"stack={proto.maxstack} code={len(proto.code)} subs={len(proto.children)}"
        )
        print("locals:", [(v.name, v.startpc, v.endpc) for v in proto.locals])
        print("upvalues:", proto.upvalues)
        hits = [
            (index, value)
            for index, value in enumerate(proto.constants)
            if isinstance(value, str) and pattern.lower() in value.lower()
        ]
        print("const hits:", hits)
        print("consts:", ", ".join(f"{i}:{quote(v)}" for i, v in enumerate(proto.constants)))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("file", nargs="?", default=DEFAULT_FILE)
    parser.add_argument("--summary", action="store_true", help="print function summary")
    parser.add_argument("--focus", help="print functions whose names/locals/upvalues/constants match")
    parser.add_argument("--disasm", help="disassemble function by path/name, or 'all'")
    args = parser.parse_args()

    version, endian, root = parse_file(Path(args.file))
    names = map_names(root)

    print(f"file={args.file} version=0x{version:x} endian={endian}")

    if args.summary or (not args.focus and not args.disasm):
        print_summary(root, names)

    if args.focus:
        print_focus(root, names, args.focus)

    if args.disasm:
        for proto in walk(root):
            if args.disasm == "all" or matches(proto, args.disasm, names):
                disassemble(proto, names)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
