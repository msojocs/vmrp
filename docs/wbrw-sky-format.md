# WBRW `.sky` Cached-Page Binary Format

Reverse-engineered from真机 cache samples in `temp/brw/http/cache3/*.sky`, the built-in
homepage `tmp_assets/wbrw/mphome.sky`, and disassembly of the parser/renderer in
`tmp_assets/wbrw/game.ext` (raw MRP ext module, ARM/Thumb, R9-relative/GOT addressing —
no absolute load base is stored in the file).

Purpose: enough byte-level detail to generate a **minimal renderable `.sky` page** (title +
body text lines + links). The recommended generation strategy (§7) is *template rewrite*, not
from-scratch display-list emission, because the display list has ≥2 dialects and is interpreted
by non-trivial native code.

All multi-byte integers in the **container header and string pool are big-endian** unless noted.
One header field (offset 4) is little-endian.

---

## 1. Top-level container layout

```
+--------------------------------------------------+
| 0x00  HEADER            (fixed 10 bytes)         |
+--------------------------------------------------+
| 0x0A  STRING POOL       (variable)               |  <-- ends at (u16be@8)+10
+--------------------------------------------------+
| poolEnd  DISPLAY LIST   (variable, to EOF)       |
+--------------------------------------------------+
```

Two authoritative section offsets:
- `poolEnd = big16(hdr[8..10]) + 10`  — start of the display list; verified byte-exact on
  all 10 samples: a u16be-length-prefixed walk of the pool from offset 10 lands **exactly** on
  `poolEnd`.
- The display list runs from `poolEnd` to EOF.

---

## 2. HEADER (offset 0, 10 bytes)

| Off | Size | Endian | Field            | Observed / meaning |
|-----|------|--------|------------------|--------------------|
| 0   | 1    | –      | `version_major`  | `0x01` or `0x02`. Selects pool/DL dialect nuances. Most modern pages = `0x02`. |
| 1   | 1    | –      | const            | always `0x02` |
| 2   | 1    | –      | const            | always `0x01` |
| 3   | 2    | **BE** | `pool_count`     | number of string-pool entries. Matches the walked entry count on every `0x02`-major sample (115, 88, 84, 166, 186, 198, 289…). Sometimes `0x0000` on old `0x01`-major pages even when entries exist → treat as advisory; the real boundary is `poolEnd` (offset 8). Generators should write the true count here. |
| 5   | 3    | –      | const            | always `02 00 00`. (Consequently a naive LE u32 read at offset 4 = `0x000200 + pool_count_low`, which is why an earlier "u32@4" looked like ~512–710; it is really `[3..5]=count` + `[5..8]=02 00 00`.) |
| 8   | 2    | **BE** | `pool_end_rel`   | `poolEnd = this + 10`. **The load-bearing field.** Marks the end of the string pool / start of the display list. |

Notes
- Byte layout is easiest read as: `[0]=verMajor [1]=02 [2]=01  [3..5]=pool_count(be16)  [5..8]=02 00 00  [8..10]=pool_end_rel(be16)`.
- There is **no** magic string and **no** total-length field in the header; total length is implicit (EOF), and the DL section carries its own length (§4).

---

## 3. STRING POOL (offset 10 … poolEnd)

A flat sequence of length-prefixed byte strings:

```
repeat pool_count times:
    u16be  len
    u8[len] data
```

The walk terminates exactly at `poolEnd`.

Entries are **heterogeneous by role** (the encoding is not uniform):
- **URLs / hrefs**: ASCII (`http://…`, `?m=baidu2&k=2009&type=mrp`, `./`, `./logo.png`,
  `skyscript:bookmark`).
- **Visible text (titles, link captions, body)**: **UTF-8** (e.g. `网盘搜索引擎` = `e7 bd 91 …`).
- **Some short tokens / captions**: **UCS-2 big-endian** (e.g. `\x00m\x00r\x00p`, `\x00Z\x00I\x00P`,
  `\x00©\x00P\x00o\x00w\x00e\x00r\x00e\x00d…`). These occur for file-extension maps and certain
  chrome captions.
- **Binary blobs**: a handful of entries are raw layout/coordinate/color data (e.g. a 40-byte
  entry `Y*S\x9f_il…`) that the display list references by index like any other pool entry.

Because entries are addressed positionally (see §5), the **index and count matter; the byte
length of each entry is free to change**.

---

## 4. DISPLAY LIST (offset poolEnd … EOF)

This is the renderable scene description. It exists in (at least) two on-disk dialects; both
reference pool strings **by index** (§5), never by byte offset.

### 4a. Common opening marker (search-result / most `page2` pages)

At `poolEnd`:
```
03 00 00  <A:u16be>  00        (6 bytes)  -- A = filesize - poolEnd - 5  (display-list length)
00 00 01 00 00 <C:u16be> <flag>           -- section header; C ≈ A-8
```
Verified: `poolEnd + 5 + A == filesize` byte-exact on every page that has this marker.
`flag` byte varies (`0x10`, `0x0E`, …).

### 4b. Dialect A — tree/attribute token stream (e.g. `32953109.sky`)

After a short palette/style block, the body is a stream of node-building tokens. Grammar:

```
token := PARENT? NODE  LEN  OP16  VALUE
PARENT := 0x40 <nodeId:u8>      -- select/attach-under parent node
NODE   := 0x40 <nodeId:u8>      -- the node being defined
LEN    := 0x00 <n:u8>           -- byte length of OP16+VALUE payload
OP16   := <hi:u8> <lo:u8>       -- 16-bit property/element opcode (big-endian value)
VALUE  := payload bytes; the trailing operand byte is a STRING-POOL INDEX
```

Observed opcodes (16-bit value / on-disk bytes → role):
| value  | bytes  | role (empirical) |
|--------|--------|------------------|
| 0x034E | `4e 03`| create node / set node type (value `01 00 00` root-ish, `00 00 00` child) |
| 0x0445 | `45 04`| relation/attr (value = two node ids) |
| 0x0538 | `38 05`| set attribute; value `<idx> 00 00 FF` (idx = pool index, FF = flags) |
| 0x0654 | `54 06`| set attribute / bind; value `<idx> 00 00 00 00` (idx = pool index) |
| 0x0861 | `61 08`| bind content (text/image); value `<idx> 00 00 00 00 00 00` |
| 0x0B62 | `62 0b`| bind link/child list; value contains 2–3 pool indices |
| 0x004C | `4c 00`| close / terminator (no value) |
| 0x0F0A | `0a 0f`| newline / line break |

The `<idx>` operands increment sequentially through the page (`0x04,0x05,…,0x72`) and map
**1:1 to string-pool indices** (verified: idx 4 → pool[4]=`./`, idx 5 → pool[5]=`./logo.png`,
idx 0x64=100 → pool[100]=`m`, etc.). Node ids (the `0x40 XX` bytes) are a separate small counter.

### 4b-2. Empirical §4a section observations (2026-07-09, from-scratch generator prep)

- Section header right after `03 00 00 <A:be32-low-half> 00`: `00 00 01 00 00 <C:u16be> <flag>`
  with `C == A - 8` on all dialect-A samples checked (32953109: A=0x5b3 C=0x5ab;
  179603: A=0x704 C=0x6fc; 32918048: A=0xa46 C=0xa3e). `flag`=0x10 on verMajor-2 pages,
  0x0E on verMajor-1 pages.
- The style/palette block between the section header and the first `40 01` node token is
  **page-specific and variable-length** (208–679 bytes across samples) — records carry
  RGB triplets (`ec ec ec`, `fc fc fc`, `dd dd dd`, focus colors) grouped per element
  class, patterns like `<u8> 42 <a1|81|91|c1> …`. A from-scratch generator cannot reuse a
  fixed constant blindly; grammar needed.
- Just before the first `40 01` token there is a `0f 00 00 <X:u16be> 00 00` marker where
  X ≈ byte length of the node-token region that follows (179603: X=0x5cc, token region
  ≈0x5ca bytes; 32953109: X=0x362).
- Not all verMajor-2 pages contain `40 01` tokens at all (1349985.sky has none) — there
  are more DL encodings than the two dialects documented above.

### 4b-3. Disassembly-confirmed record grammar (supersedes §4b's empirical token guess)

The DL parser (record fill `0xE582–0xE662`, dispatcher `0xDC60`, shared variable-tail
`0xE306`) consumes **fixed-framing records**, not the `PARENT/NODE/LEN/OP16` shape
guessed in §4b:

```
record := <a:be16> <flag:be16>
          [<e1:be16> if flag&0x8000] [<e2:be16> if flag&0x4000]
          <op:s8> <len:s8> <payload: len bytes>
totalBytes = 6|8|10 (flag bits) + len
```

- `op` is a single byte at struct offset 16 (`0xDC7E ldrsb`), `len` at offset 17 drives
  the payload size for every handler (opcode compares `0xDC80–0xDD42`).
- Node ids appear as `0x4000|id` values in `a`/`flag`/`e2` (e.g. a=0x400D flag=0x4021
  e2=3 op=0x4E = create child node 0x21 under node 0x0D).
- Observed ops in the node region (all three dialect-A samples decode byte-exact to
  stream end with this framing): `0x31` root descriptor (len 12, 6×be16, not gated —
  error 50 is malloc-fail at `0xA5D6`/`0xE2E4`, not a validation reject); `0x4E` node
  create (len 3); `0x38` attr/href bind (len 5: `<poolIdx:be16> 00 00 ff`); `0x54` text
  bind (len 6: `<poolIdx:be16> 00 00 00 00`); `0x4C` inline separator (len 0); `0x45`
  image (len 4: two pool indices — the logo header binds pool `./` + `./logo.png`);
  `0x62` link list (link count = len−8); `0x60` hr/separator (len 1); `0x40` style/color
  record (4×be16+2×s8+2×be16 head, tail = len−14).
- The stream also contains **single-byte inner-state switch tokens** classified by
  state 3 @`0xE718`: `0x0E`→st1, `0x0F`→st2 (record/dispatch state — this is what emits
  output nodes via `0xE66E`), `0x10`→st5, `0x0C`→state-0 record path, `0x0D` terminator-
  class. The node-record region is entered via `0F 00 00 <X:be32-ish> 00 00` where
  X ≈ node-region byte length.
- Outer envelope: `03 <A:be32>` then `<be32 =1> <bodyLen:be32 =A−8>` then the
  style/glyph/node stream; sum of record sizes must equal bodyLen or the parser hits
  need-more-data (`0xEAC8`) and stalls.
- 179603.sky per-row emission pattern (repeats per list row, sequential pool indices):
  `[400D][40xx][e2=3] 4E 03 000000` (row node) → `[40xx][yy] 38 05 <idx> 0000FF`
  (caption anchor; target is `pool[idx-1]`) → `[yy][40zz][e2=3] 54 06 <idx> 00000000`
  (companion/suffix) → `[40xx][40ww][e2=3] 4C 00`
  → `[40ww][40vv][e2=7] 54 06 <idx> 00000000` (suffix text).

### 4b-4. From-scratch generation (implemented in `tool/proxy.js` generateSkyPage, 2026-07-09)

Validated end-to-end by PPM screenshot: a page generated with **no template bytes at all**
renders correctly (title bar + per-row link list). The working minimal DL:

```
03 <A:be32>                          A = C + 8
00 00 00 01                          inner count
<C:be32>                             C = body length
0F <X:be32> 00 00                    !! REQUIRED: state token 0x0F switches the parser
                                     into the record-dispatch state (only that state
                                     emits document nodes — without it the records are
                                     consumed but the document stays empty and the
                                     browser silently stays on the home page).
                                     X = bytes from after the be32 to stream end
                                     (i.e. 2 + nodeRegion length). Verified on
                                     179603 (X=0x5CC) and 32953109 (X=0x362).
records:
  4001 0002      31 0C 00 02 00*10   root descriptor (id 2)
  4001 4002 0003 4E 03 01 00 00      document node   (id 2, e2=3, root-type)
  4002 4003 0004 4E 03 00 00 00      body node       (id 3, e2=4)
  4003 4004 0006 54 06 <titleIdx+1> 00 00 00 00  header text (non-focusable)
  4001 4005 0003 4E 03 01 00 00      content section (id 5, e2=3)
  per item (id += 3):
    4005|4000 40id 0003 4E 03 00 00 00        row node (own node per row = line break)
    40id  id+1     38 05 <capIdx> 00 00 FF    caption (UCS-2BE pool entry)
    id+1  40id+2 0003 54 06 <companionIdx> 00 00 00 00
```

Pool convention: `[0]`=page URL (ASCII), `[1]`=document title (UTF-8 — not referenced by
the DL; read by the browser chrome/cache-index layer, and satisfies UTF-8 substring
checks on the cache), `[2]`=`./` base, `[3]`=title (UCS-2BE, bound by the header record),
`[4..]` per link = href (ASCII) + caption (UCS-2BE) + one-space companion.
Across all 10 real-device samples, all 320 decoded `0x38` anchors use this activation
invariant: `pool[captionIndex-1]` is the target link.

Hard-won pitfalls:
- Without the `0F` token the payload caches fine but never renders (no error shown).
- Do NOT emit an image element (`38` bound to `./logo.png`): the image fetch never
  completes locally and the title bar sticks at "加载图片0/1" instead of the page title.
- Items rendered as sibling `38` records under one section flow inline; a `4E` child
  node per row is what produces line breaks.
- A bare `38` record (no companion `54`) produces NO visible row; `38` + `4C` doesn't
  render either. Rows become visible only with a companion `54`.
- **Plain (non-link) text**: the generator uses a row with only `54(e2=6)`, no `0x38`.
  Its pool layout is caption + one-space companion; operand points at the companion so
  the text append path's `operand-1` resolution lands on the caption.
### 4b-5. Plain text vs hyperlink (render-layer, disassembly-confirmed 2026-07-09)

The render layer (not the DL opcode e2) decides text color and focusability, keyed
**only on the element op byte `[+20]`**:
- **Focusable/selectable iff op is `0x38` ('8') or `0x37` ('7').** Focus scan `0x275B4`
  (`hasFocusableDescendant`) and `0x28718` test only `cmp #56`/`cmp #55` over the `[+8]`
  sibling chain; `e2` (`[+6]`) is read nowhere in the focus decision. `0x54`('T'),
  `0x4E`('N'), `0x4C`('L') are never focusable on their own.
- **Color** (paint `0x27F50`, link check `0x2F96C`): a text box whose subtree contains a
  `0x38`/`0x37` child gets the global link color; otherwise it inherits the default
  (black). No per-element palette, no inline RGB, not keyed on e2.

Consequence for generation — the two row shapes:
- **Hyperlink row** (colored, focusable): `4E`(row) + `0x38`(caption → anchor) +
  `0x54(e2=3)`(companion). The `0x38` makes it focusable and colored; activation
  resolves the target from `pool[captionIndex-1]`, not from the companion.
- **Plain-text row** (black, non-focusable): `4E`(row) + `0x54(e2=6)`(text), **no `0x38`**.
  Binding plain text with `0x38` (the earlier bug) made it blue + cursor-selectable.

Operand quirk (empirically reproduced by PPM, both e2=6 and e2=9): the `0x54` text-bind
renders `pool[operand-1]`, whereas `0x38` renders `pool[operand]` directly. So a
plain-text `0x54` must set `operand = textPoolIndex + 1` with a filler pool entry at that
slot; the render's −1 then lands on the text. (Root cause in the `0x23B38` text-append
index resolution not fully isolated; the +1 compensation is verified black-box.)

Verified end-to-end by screenshot: plain lines render `[0,0,0]` black, link lines
`[8,88,176]` blue, and the focus box (DOWN key) lands only on links, skipping plain text.

### 4b-6 (superseded note)
With no `0x40` style records emitted, links still get the global link color and plain text
stays black — the distinction is the `0x38` child (§4b-5), not palette records. (An earlier
draft here wrongly said all text renders identically; that predated the render-layer trace.)

### 4c. Dialect B — compact 3-byte-op stream (e.g. `mphome.sky`)

Opens directly with `01 00 00 01` (no `03` marker at poolEnd; the `03 00 00` marker appears later
as one record among many). Body is `<op24> 00 <idx:u8> 0A …` groups, e.g. repeating
`d5 97 0c 00 <idx> 0a`, `de 9b 0c 00 <idx> 0a`, `df a8 0c 00 <idx> 0a` with sequential `<idx>`
pool indices and `0a` separators. Same index-binding principle.

> Practical consequence: the DL bytecode is **not** worth emitting from scratch for a minimal
> generator. Reuse a real DL (template) and only rewrite the pool it points into (§7).

---

## 5. String-reference mechanism (how content binds to layout)

- The display list references pool strings **by positional index**, not by byte offset and not
  inline. Evidence: (a) DL operand bytes are small sequential values equal to pool indices and
  resolve to the correct strings; (b) scanning the DL for u16 values equal to pool *byte offsets*
  finds only ~7 incidental hits across a multi-hundred-byte DL (noise, not systematic).
- In `game.ext` the network/response record layer parses records into a `{u16 id; u16 len;
  void* data}` field list (`record_get_field`, file offset **0x26EDC**), consumed at **0x27020**
  and unpacked by a `struct.unpack`-style routine at **0x120CC**. That layer handles the
  **proxy2 response envelope** (e.g. field id 33 = "tag33", checked `==3` before rendering); it is
  a *different layer* from the on-disk `.sky` container decoded here. The `.sky` payload delivered
  inside the response is the header+pool+display-list structure of §1–§4.

### Key parser/renderer offsets in `game.ext` (file-relative)
| Offset | Role |
|--------|------|
| 0x41C60 | response handler: checks status==200, tag33==3, opens cache object, renders |
| 0x41E30–0x42100 | render / paging loop |
| 0x48860 | element-type dispatch (16-bit tag binary search); type table 0x48C04–0x48C30 |
| 0x120CC | `struct.unpack(out, buf, len, fmt)` — BE via `>`; `h/H`=i16/u16, `i/I`=i32/u32, `b/B`=i8/u8, `c<N>`=N raw bytes, `p`=pascal string (len byte+bytes), `x`=pad |
| 0x26EDC | `record_get_field(record, u16 id, *len, *ptr)` |
| 0x27020 | record ingest/build driver |

### Embedded struct format strings (data, in `game.ext`)
- 0x04DB58 `>hc16bihc29c29ihhihhihh` — **HTTP disk-cache / netmoni index entry**, NOT the .sky
  page header (sits amid `content-length`,`date`,`http/1.1`,`%s/cache3`,`proxy2.51mrp.com`).
- 0x04E0F0 `>BHH`, 0x04E114 `>BHHpB`, 0x04E198 `>HHHHIB`, 0x04E1A0 `>HH`, 0x04E1B0 `>II` —
  per-field/record formats for the proxy2 record layer.
- 0x04E15C `application/sky-mrp` — the content-type of the .sky payload.
- Content-type/config markers appear at the pool tail on some pages (`application/sky-mrp`,
  `FALSE`, `base64`, `unicode`, `TRUE`) before the DL.

---

## 5b. Disassembly-verified parser behavior (game.ext, 2026-07-09)

Static analysis of the extracted `game.ext` (Thumb, R9-relative GOT) confirmed the
template-rewrite strategy end-to-end:

- **Pool resolution is a rebuilt walk-table, not byte offsets.** `skyPool_resolveIndex`
  at `0xA470` lazily walks the pool once (BE16 length prefixes via `read_be16` @`0x113C0`,
  bounded by pool byte size `[obj+0x40]`, offsets appended to a u32 table `[obj+0x34]`),
  then resolves DL indices by table lookup. Variable-length entry substitution is safe
  as long as count/order are preserved.
- **Encoding is per-slot.** The platform draw call `T__DrawText(..., is_unicode, font)`
  takes an encoding flag per string; pages mix ASCII/UTF-8/UCS-2BE entries with **no
  per-entry flag byte** in the pool (mphome.sky: nav label pool[1] UTF-8 vs grid label
  pool[19] UCS-2BE), so the decision is DL-opcode-implied or byte-sniffed. Pool strings
  are copied raw into nodes (strdup `0x11D28`); internal canonical form is UCS-2 and the
  draw path re-encodes to UTF-8 (`0x1230C`, `0x48FC8`). The charset framework at
  `0x143A8` (+ name table `0x50648`) is only called from the HTTP `charset=` header path
  (`0x25CB4`, `0x261D4`), not the .sky pool. Keep each slot's original encoding; never
  unify.
- **No entry-length cap, no count validation.** The u16 length is used unclamped and
  zero-copy (the 2 length bytes are NUL'd in place for termination). `hdr[3..5]`
  (pool_count) is advisory — `mphome.sky` ships `0` there and renders. Bound checking
  happens only on the requested index (`0xA4E2`, error 2 if out of range).
- **The §4a `A` field IS consumed — patching it is mandatory.** The DL state machine
  (feed `0xE8E8`, top-level states `0xE97C`, node loop `0xE37C`, jump table `0xE3B8`)
  reads the word after the `03` marker as a **BE32** (`0xEC9C`) into `[ctx+0x24]` — i.e.
  the `00 00` bytes before `A` are the high half of that word and must stay zero — and
  terminates the node-emit loop when bytes-consumed `[ctx+0x1C] >= A` (`0xE38E`/`0xE410`,
  8 bytes per node). Stale-small A silently truncates the page; stale-large A either
  stalls in the need-more-data path (`0xEAC8`) waiting for bytes that never come or
  consumes trailing bytes as garbage nodes. (`A = filesize - poolEnd - 5`; if the DL
  bytes are copied verbatim, A is unchanged because it only depends on DL size.)
- **Error codes** (response handler `0x41C60`, status strings UCS-2BE @`0x4B7A2`,
  "错误码:" @`0x4B7EA`): 2 = empty response body; 8 = cache object open failed;
  **50 (0x32) = .sky payload failed to build a document** (`0x41D16`, `[obj+12]` NULL
  after ingest) — this is the on-screen "获取页面错误" + code seen earlier;
  51 = length consistency check failed (`0x44318`); 74 = envelope gate failed
  (status != 200 or record field 33 != 3, checked via `record_get_field` @`0x26EDC`).
  The envelope is the only pre-parse gate.

## 6. Field/validation checklist (what must be correct or the page is rejected)

1. `hdr[1]==0x02`, `hdr[2]==0x01`, `hdr[5..8]==02 00 00`. (`hdr[0]` = 1 or 2.)
2. `poolEnd = big16(hdr[8..10]) + 10` must equal the actual pool-walk end.
3. Pool = exactly `pool_count` (`big16(hdr[3..5])`) u16be-length entries covering `[10, poolEnd)`
   with no residue. Write the true count into `hdr[3..5]`.
4. Display list from `poolEnd` to EOF; if it uses the §4a marker, then
   `poolEnd + 5 + big16(dl[3..5]) == filesize` must hold (the `A` length field).
5. Every pool index referenced by the DL must be `< pool_count`.
6. Visible-text pool entries encoded UTF-8; URLs ASCII; keep UCS-2BE entries UCS-2BE.
7. The response must first pass the proxy2 envelope gate (status 200 + tag33==3) — handled by
   `tool/proxy.js`, outside this `.sky` payload.

---

## 7. Generating a minimal renderable page (recommended: template rewrite)

Because the DL bytecode is interpreted by native code with ≥2 dialects, do **not** synthesize a DL
from scratch. Instead:

1. Pick a real skeleton whose DL layout matches the target shape (title + N text/link rows), e.g.
   `temp/brw/http/cache3/32953109.sky` (title + logo + list of links).
2. Parse its header → `poolEnd`, `pool_count`.
3. Parse the pool into an ordered list of `(index, role, bytes)`. Determine each entry's role from
   its content and the DL binding (URL vs UTF-8 caption vs UCS-2 vs binary).
4. **Substitute contents in place, preserving `pool_count` and every index**:
   - replace URL entries with your hrefs (ASCII),
   - replace caption/title/body entries with your text (UTF-8; or UCS-2BE if the original entry
     was UCS-2BE),
   - leave binary/coordinate/color entries untouched.
   Each entry keeps its slot; only its `len` + bytes change.
5. Re-emit the pool with recomputed u16be lengths. Compute the new `poolEnd`.
6. Copy the original display-list bytes verbatim (indices still valid because count/positions were
   preserved). Append after the new pool.
7. Fix up header:
   - `hdr[3..5] = pool_count` (unchanged),
   - `hdr[8..10] = poolEnd - 10`.
8. If the DL uses the §4a marker, patch its `A` length field so
   `poolEnd + 5 + A == new_filesize`.

This yields a byte-valid `.sky` whose title, body text lines, and link targets are your content,
while the proven-renderable layout program is reused unchanged.

### Minimal element set to target
- **Title**: the UTF-8 pool entry bound by the page's first text opcode (in `32953109` it is the
  entry at index 1 / index 6 — `网盘搜索引擎`).
- **Body text line**: a UTF-8 pool entry bound by a text opcode (`0x0538`/`0x0654`) with a `0x0F0A`
  (newline) between lines.
- **Link**: a pair — an ASCII URL pool entry + a UTF-8 caption pool entry — bound by a
  link opcode (`0x0B62`) / attribute opcodes. Keep the pair count equal to the template's.

---

## 8. Evidence summary (samples)

| file | verMajor | pool_count(hdr[3:5]) | walked entries | poolEnd(hdr8+10) | §4a marker | poolEnd+5+A == size |
|------|----------|----------------------|----------------|------------------|-----------|---------------------|
| 32953109.sky | 2 | 115 | 115 | 1198 | yes @1198 | 1198+5+1459=2662 ✓ |
| 179603.sky   | 2 | 88  | 88  | 2660 | yes @2660 | 2660+5+1796=4461 ✓ |
| 32557848.sky | 1 | 0*  | 50  | 1587 | yes @1587 | 1587+5+652 =2244 ✓ |
| 1349985.sky  | 2 | 84  | 84  | 2873 | yes @2873 | 2873+5+1151=4029 ✓ |
| 32918048.sky | 2 | 166 | 166 | 2820 | yes @2820 | 2820+5+2630=5455 ✓ |
| 1078817.sky  | 2 | 186 | 186 | 11178| yes @11178| 11178+5+3041=14224 ✓ |
| 113738.sky   | 1 | 289 | 289 | 12864| yes @12864| 12864+5+3770=16639 ✓ |
| 32894452.sky | 2 | 198 | 198 | —    | —         | — |
| mphome.sky (asset) | 2 | 112 | 112 | 2278 | dialect B (marker later @2599) | 2599+5+1170=3774 ✓ |
| mphome.sky (lpg)   | 2 | —   | —   | 2559 | marker @2880 | 2880+5+1135=4020 ✓ |

`*` old `0x01`-major page stores 0 in the count hint; boundary still derived from offset 8.

The `mphome.sky.ut` sidecar (20 bytes) is unrelated to the page format — it is a small opaque
token/checksum blob (`45 e0 83 55 6e 49 52 39 34 2e 58 ee cc cc 70 64 0a 22 f4 0a`), not a
"pre-compiled" page.
