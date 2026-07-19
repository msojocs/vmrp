# DNS Trace Index - VMRP MRP Emulator Network Analysis

**Date:** 2026-06-09  
**Project:** VMRP (Mobile Runtime Platform) Emulator  
**Analysis:** How `rop.skymobiapp.com` DNS resolution is intercepted

---

## 📚 Documentation Structure

### 1. **QUICK_REFERENCE.md** ⚡ START HERE
**Best for:** Quick answers, code snippets, reference tables  
**Length:** 6 KB  
**Time to read:** 5-10 minutes

Contains:
- 30-second answer
- Key locations table
- Default DNS map definition
- Call chain diagram
- Configuration methods
- Quick function reference

**Read this if you:** Want a quick overview or are looking up specific details

---

### 2. **DNS_TRACE_ANALYSIS.md** 📖 MOST COMPLETE
**Best for:** Deep understanding, implementation details, code walkthroughs  
**Length:** 22 KB  
**Time to read:** 20-30 minutes

Contains:
- Executive summary
- DNS map initialization (4 detailed steps)
- Core DNS mapping logic with full code
- Concrete example walkthrough
- Async DNS resolution mechanism
- CMWAP proxy mode handling
- Data structures explained
- Complete call chain with diagram
- Configuration priority levels
- Verification & testing examples

**Read this if you:** Want to understand HOW everything works in detail

---

### 3. **FINDINGS_SUMMARY.txt** 📋 ORGANIZED REFERENCE
**Best for:** Categorized findings, searching by topic, comprehensive checklist  
**Length:** 21 KB  
**Time to read:** 15-25 minutes

Contains:
- All references organized by category:
  * References to `rop.skymobiapp.com`
  * All DNS-related functions (13 functions)
  * Network-related source files
  * Socket connection functions
  * IP address literals
  * HTTP request mechanism
  * Data structures
  * Debug output examples
  * All file references with line numbers

**Read this if you:** Need to find something specific or want a comprehensive checklist

---

## 🎯 Quick Navigation

### I want to...

#### Understand the big picture
→ Start with **QUICK_REFERENCE.md** (5 min)

#### Get all the details
→ Read **DNS_TRACE_ANALYSIS.md** (30 min)

#### Find a specific function
→ Look in **FINDINGS_SUMMARY.txt** (search for function name)

#### See code implementation
→ Check **DNS_TRACE_ANALYSIS.md** section 4 or 14

#### Configure DNS mapping
→ Read **QUICK_REFERENCE.md** section "How to Change the Mapping"

#### Understand the call chain
→ See section "Call Chain" in **QUICK_REFERENCE.md** or section 10 in **DNS_TRACE_ANALYSIS.md**

#### Find debug output
→ See **FINDINGS_SUMMARY.txt** section 13 or **DNS_TRACE_ANALYSIS.md** section 14

#### Check data structures
→ See **FINDINGS_SUMMARY.txt** section 8 or **DNS_TRACE_ANALYSIS.md** section 9

---

## 🔑 Key Findings at a Glance

| Item | Answer | Location |
|------|--------|----------|
| **Domain** | `rop.skymobiapp.com` | skyengine.c:28 |
| **Maps to** | `127.0.0.1` | skyengine.c:28 |
| **Mechanism** | In-memory lookup table | network.c:161-174 |
| **Resolution** | system getaddrinfo() | network.c:480-517 |
| **Entry point** | mr_getHostByName() | network.c:536 |
| **Core function** | my_getHostByNameSync() | network.c:480 |
| **Max entries** | 32 DNS mappings | network.c:53 |
| **Async** | Yes, with pthread | network.c:520-528 |
| **Customizable** | Yes, 4 methods | skyengine.c:216-233 |

---

## 📂 File Organization

### Analysis Documents (This Directory)
```
/home/msojocs/github/skyengine/
├── QUICK_REFERENCE.md          ← Quick lookup (6 KB)
├── DNS_TRACE_ANALYSIS.md       ← Complete analysis (22 KB)
├── FINDINGS_SUMMARY.txt        ← Organized findings (21 KB)
└── DNS_TRACE_INDEX.md          ← This file
```

### Source Code Files Referenced
```
src/
├── skyengine.c                      ← DNS map definition (lines 25-282)
├── network.c                   ← Core DNS/socket implementation (765 lines)
├── arm_ext_executor.c          ← ARM SWI handler (line 1549)
├── native_dsm_funcs.c          ← Native function bridge (lines 178-180)
├── mythroad/
│   ├── dsm.c                   ← MRP wrappers (lines 999-1032)
│   └── mythroad.c              ← Function table setup (line 466)
└── include/
    ├── network.h               ← Function declarations
    └── skyengine_api.h              ← Public API
```

---

## 🔍 Search Tips

### Finding by Topic
- **DNS Configuration**: Search "SKYENGINE_DEFAULT_DNS_MAP" or section 1
- **DNS Functions**: See FINDINGS_SUMMARY.txt section 2
- **Socket Functions**: See FINDINGS_SUMMARY.txt section 4
- **Call Chain**: See all documents, section labeled "Call Chain"
- **Data Structures**: FINDINGS_SUMMARY.txt section 8, DNS_TRACE_ANALYSIS.md section 9

### Finding by File
- **skyengine.c**: FINDINGS_SUMMARY.txt end section, DNS_TRACE_ANALYSIS.md section 2
- **network.c**: FINDINGS_SUMMARY.txt end section, DNS_TRACE_ANALYSIS.md section 3-14
- **arm_ext_executor.c**: FINDINGS_SUMMARY.txt section 7
- **mythroad/dsm.c**: FINDINGS_SUMMARY.txt section 7

### Finding by Line Number
- Use Ctrl+F to search line numbers (e.g., "network.c:480")
- All references include file:line format

---

## 📊 Statistics

| Metric | Count |
|--------|-------|
| DNS functions found | 13 |
| Socket functions found | 10 |
| Network source files | 12 |
| References to rop.skymobiapp.com | 1 (hardcoded) |
| Max DNS mapping entries | 32 |
| DNS map entry size | 256 bytes × 2 |
| Configuration methods | 4 (CLI, API, Env, Default) |
| Call chain depth | 6 layers |
| Files analyzed | 20+ |

---

## 🚀 Quick Start Examples

### Change DNS mapping via CLI
```bash
skyengine --dns-map "rop.skymobiapp.com->192.168.1.100" mythroad/gghjt.mrp
```

### Change DNS mapping via environment variable
```bash
export SKYENGINE_DNS_MAP="rop.skymobiapp.com->192.168.1.100"
skyengine mythroad/gghjt.mrp
```

### Change DNS mapping via API
```c
#include "include/skyengine_api.h"
skyengine_api_set_dns_map("rop.skymobiapp.com->192.168.1.100");
```

### Expected debug output
```
dns map: rop.skymobiapp.com -> 127.0.0.1
getaddrinfo of 127.0.0.1
--- IPv4 address: 127.0.0.1
my_connect('127.0.0.1', 80)
```

---

## 💡 Key Concepts

### DNS Mapping Table
- Location: `static DnsMapEntry dnsMap[32]` in network.c:61
- Size: Up to 32 entries
- Each entry: original domain → fake/mapped domain
- Lookup: Case-insensitive, trailing-dot tolerant

### Resolution Process
1. **Normalize**: Convert domain to lowercase, remove trailing dots
2. **Map**: Search DNS table for match
3. **Resolve**: Call system getaddrinfo() on mapped domain
4. **Return**: IP address as 32-bit integer

### Call Path
```
ARM App → SWI Handler → Wrapper → Native Bridge → Network Layer → DNS Table → getaddrinfo()
```

### Transparency
- MRP application code has NO IDEA it's being intercepted
- Returns 0x7F000001 for rop.skymobiapp.com (transparent)
- Socket connects to 127.0.0.1 instead of real server

---

## ⚠️ Important Notes

1. **No external DNS server**: Uses system getaddrinfo(), not custom DNS
2. **No custom protocol**: Standard Linux socket APIs
3. **No network interception**: Only DNS mapping, not packet capture
4. **Transparent**: MRP code sees normal IP addresses
5. **Async capable**: Supports callbacks with pthread
6. **CMWAP special case**: Line 628 in network.c has special handling
7. **Case-insensitive**: Domain matching is case-insensitive
8. **Trailing dots**: Automatically removes trailing dots from domains

---

## 📖 Recommended Reading Order

### For Quick Understanding (5-10 minutes)
1. This index (current file)
2. QUICK_REFERENCE.md
3. Run: `skyengine --dns-map "rop.skymobiapp.com->127.0.0.1" mythroad/gghjt.mrp`

### For Complete Understanding (30-45 minutes)
1. This index
2. QUICK_REFERENCE.md
3. DNS_TRACE_ANALYSIS.md
4. FINDINGS_SUMMARY.txt (for reference)

### For Implementation/Debugging
1. QUICK_REFERENCE.md (functions table)
2. FINDINGS_SUMMARY.txt (organized by topic)
3. DNS_TRACE_ANALYSIS.md (code examples)
4. Source files (skyengine.c, network.c)

---

## 🔗 Cross References

### Within QUICK_REFERENCE.md
- "Core Function" → explains my_getHostByNameSync()
- "Call Chain" → shows execution path
- "How to Change the Mapping" → 3 configuration methods
- "Functions at a Glance" → quick function reference

### Within DNS_TRACE_ANALYSIS.md
- "Section 4" → Core DNS mapping logic with code
- "Section 5" → Concrete example walkthrough
- "Section 10" → Complete call chain diagram
- "Section 14" → Verification & testing

### Within FINDINGS_SUMMARY.txt
- "Section 2" → All DNS functions list
- "Section 4" → All socket functions
- "Section 7" → Complete call chain
- "Section 13" → Debug output samples

---

## ✅ Verification Checklist

To verify DNS mapping is working:

- [ ] Run: `skyengine --dns-map "rop.skymobiapp.com->127.0.0.1" mythroad/gghjt.mrp`
- [ ] Look for: "dns map: rop.skymobiapp.com -> 127.0.0.1"
- [ ] Look for: "--- IPv4 address: 127.0.0.1"
- [ ] Look for: "my_connect('127.0.0.1', <port>)"
- [ ] Confirm: Connection goes to 127.0.0.1, not real server

---

## 📝 Document Metadata

| Property | Value |
|----------|-------|
| Analysis Date | 2026-06-09 |
| Project | VMRP (Mobile Runtime Platform) |
| Scope | DNS resolution interception mechanism |
| Files Analyzed | 20+ source files |
| Functions Found | 23 (13 DNS + 10 socket) |
| Code References | 50+ file:line locations |
| Code Examples | 15+ annotated code snippets |
| Diagrams | 3 (call chain, flow, priority) |
| Total Documentation | 49 KB (3 files) |

---

## 🎓 Learning Path

### Beginner
1. Read QUICK_REFERENCE.md sections 1-3
2. Understand: "It's just a lookup table"
3. Try: Change DNS mapping via CLI

### Intermediate
1. Read QUICK_REFERENCE.md fully
2. Read FINDINGS_SUMMARY.txt sections 1-4
3. Understand: How functions are connected
4. Try: Look at network.c:480-517

### Advanced
1. Read all three documents
2. Study DNS_TRACE_ANALYSIS.md sections 2-10
3. Review source code: skyengine.c, network.c
4. Understand: Async, threading, CMWAP mode
5. Modify: Create custom DNS mappings

---

## 🔧 Troubleshooting

### DNS mapping not working?
→ Check QUICK_REFERENCE.md "How to Change the Mapping"

### Want to see debug output?
→ See FINDINGS_SUMMARY.txt section 13

### Need function location?
→ See FINDINGS_SUMMARY.txt end section or search all files

### Don't understand call chain?
→ See QUICK_REFERENCE.md "Call Chain" section

### Want configuration examples?
→ See QUICK_REFERENCE.md "How to Change the Mapping"

---

## 📞 Questions Answered

This documentation answers:

1. ✅ How does VMRP access rop.skymobiapp.com?
2. ✅ Is DNS resolution intercepted?
3. ✅ Where is the DNS mapping defined?
4. ✅ How does the DNS resolution work?
5. ✅ What functions are involved?
6. ✅ How can I change the mapping?
7. ✅ What IP does it resolve to?
8. ✅ Is it async or sync?
9. ✅ How does the call chain work?
10. ✅ What happens during socket connection?

---

**For more details, see the specific documents listed above.**

