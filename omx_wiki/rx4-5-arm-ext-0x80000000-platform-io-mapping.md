---
title: "RX4.5 ARM EXT 0x80000000 Platform IO Mapping"
tags: ["rx45", "arm-ext", "unicorn", "platform-memory", "debugging"]
created: 2026-06-26T12:05:26.745Z
updated: 2026-06-26T12:05:26.745Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# RX4.5 ARM EXT 0x80000000 Platform IO Mapping

2026-06-26 RX4.5 startup failure in build/vmrp temp/RX4.5.mrp was traced by disassembly to Thumb PC 0x2DFAC2: ldrsh r5, [r2, r3], where r2 was *(R9+0x100)=0x80110004. game.ext setup at 0x2DFB30..0x2DFC40 selects MTK platform address ranges including 0x80110000..0x81070000 and stores base+4/+8/+12 into R9+0x100..0x110. Root cause was missing ARM-visible 0x80000000-class platform ROM/MMIO/state mapping, not SDL, CLI, MRP entry, or app-specific corruption. Generic fix: add platform_io_mem mapped at 0x80000000 with 18 MiB size, support it in arm_ptr() and arm_addr(), install the R9 restore block hook for that range, and free it on unload. No app-specific if(is_rx) branch. Verification without xvfb used SDL dummy + PPM: RX reached the blue RX文件管理器 shell, PPM 240x320 unique=173 nonblack=76008, and no UC_ERR/unmapped logs. Regression e2e for rx, gwkdl, gxdzc, and opbzqe passed.
