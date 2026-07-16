# Wiki Log

## [2026-07-15T19:21:26+08:00] complete
- **Pages:** gfhcq-game-stutter-performance-investigation.md
- **Summary:** Completed independent implementation/evidence/requirements audits, replaced rectangle over-damage with actual changed-pixel spans that preserve off-screen, subview, temporary-stride and transparent/no-op semantics, remeasured 5.49x steady throughput with explicit SDL dummy, matched the pre-fix PPM byte-for-byte, and passed the final dummy 31/31 E2E suite.

## [2026-07-15T18:17:18+08:00] update
- **Pages:** gfhcq-game-stutter-performance-investigation.md
- **Summary:** Recorded the first table[120] rectangle-damage implementation and initial regressions; the final audit later replaced rectangle over-damage with actual changed-pixel tracking to preserve off-screen and transparent/no-op behavior.

## [2026-07-15T16:35:00+08:00] update
- **Pages:** gfhcq-game-stutter-performance-investigation.md
- **Summary:** Recorded an intermediate 8 MiB reproduction and timer/pixel-loop disassembly; the completion entry corrects its game-phase baseline because those early artifacts predated the final three-second menu delay.

## [2026-07-15T00:00:00+08:00] add
- **Pages:** gfhcq-game-stutter-performance-investigation.md
- **Summary:** Started the focused gfhcq game-stutter investigation; recorded the SDL dummy-driver constraint, 480x800 scaling factor, fixture identity, low-volume measurement plan, and disassembly requirement.

## [2026-07-10T16:40:00+08:00] update
- **Pages:** wbrw-proxy2-web-page-rendering.md
- **Summary:** Corrected WBRW link activation semantics from caption/href to href/caption/companion; recorded 320 real-device anchor samples, the first-link PPM proof, and the successful child-page request/cache evidence.

## [2026-07-09T10:26:13+08:00] add
- **Pages:** wbrw-proxy2-web-page-rendering.md
- **Summary:** Recorded WBRW proxy2 /page2 web-page rendering fix, live packet/TLV evidence, Baidu cache and PPM proof, rejected tag33/.sky experiments, and full e2e compatibility result.

## [2026-07-06T22:59:48+08:00] add
- **Pages:** gzwdzjs-primary-black-clear-damage-synthesis.md
- **Summary:** Recorded gzwdzjs tutorial top black root cause: primary full-screen black DrawRect cache reset was marked as visible damage and callback-exit synthesis exposed unpresented rows; verification covered focused/full gzwdzjs plus opbzqe/dota/gxdzc regressions.

## [2026-06-26T14:12:07+08:00] update
- **Pages:** gms-payment-latency-analysis.md
- **Summary:** Corrected the screen interpretation: hash dd716938a16f is payment success. Recorded generic MR_SMS_RESULT_FLAG fix and validation that success now appears at confirm+1s.

## [2026-06-26T13:59:05+08:00] add
- **Pages:** gms-payment-latency-analysis.md
- **Summary:** Recorded GMS payment latency analysis: test driver delay vs GMS internal SMS confirm wait, PPM/PC-watch evidence, and rejected unsafe generic fixes.

## [2026-06-25T14:46:53.066Z] ingest
- **Pages:** gghjt-modal-timer-and-cold-extraction-regressions.md
- **Summary:** Created new page "gghjt modal timer and cold extraction regressions"

## [2026-06-25T14:46:53.068Z] add
- **Pages:** gghjt-modal-timer-and-cold-extraction-regressions.md
- **Summary:** Created wiki page gghjt-modal-timer-and-cold-extraction-regressions.md
## [2026-06-26T12:05:26.745Z] ingest
- **Pages:** rx4-5-arm-ext-0x80000000-platform-io-mapping.md
- **Summary:** Created new page "RX4.5 ARM EXT 0x80000000 Platform IO Mapping"

## [2026-06-26T12:05:26.757Z] add
- **Pages:** rx4-5-arm-ext-0x80000000-platform-io-mapping.md
- **Summary:** Created wiki page rx4-5-arm-ext-0x80000000-platform-io-mapping.md

## [2026-06-27T02:03:08.981Z] ingest
- **Pages:** opglqa-font-download-repeat-investigation.md
- **Summary:** Created new page "opglqa font download repeat investigation"

## [2026-06-27T02:03:08.983Z] add
- **Pages:** opglqa-font-download-repeat-investigation.md
- **Summary:** Created wiki page opglqa-font-download-repeat-investigation.md

## [2026-06-27T02:28:27.247Z] ingest
- **Pages:** opglqa-font-download-repeat-investigation.md
- **Summary:** Updated "opglqa font download repeat investigation" with new content

## [2026-06-27T06:49:59.956Z] ingest
- **Pages:** dota-browser-plugin-dsm-package-provenance.md
- **Summary:** Created new page "DOTA browser plugin DSM package provenance"

## [2026-06-27T06:49:59.969Z] add
- **Pages:** dota-browser-plugin-dsm-package-provenance.md
- **Summary:** Created wiki page dota-browser-plugin-dsm-package-provenance.md

## [2026-06-27T07:05:51.287Z] ingest
- **Pages:** dota-browser-plugin-dsm-package-provenance.md
- **Summary:** Updated "DOTA browser plugin DSM package provenance" with new content

## [2026-06-27T08:29:28.334Z] ingest
- **Pages:** dota-browser-plugin-network-state-investigation.md
- **Summary:** Created new page "DOTA browser plugin network state investigation"

## [2026-06-27T08:29:28.338Z] add
- **Pages:** dota-browser-plugin-network-state-investigation.md
- **Summary:** Created wiki page dota-browser-plugin-network-state-investigation.md

## [2026-06-27T08:45:44.402Z] add
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, opglqa-font-download-repeat-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md
- **Summary:** Refreshed wiki index and derived metadata surfaces

## [2026-06-27T09:09:19.252Z] query
- **Pages:** dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-dsm-package-provenance.md, opglqa-font-download-repeat-investigation.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md
- **Summary:** Query "dota simpleDownload browser plugin network state investigation" → 6 results (of 6 total)

## [2026-06-27T09:11:15.121Z] ingest
- **Pages:** dota-browser-plugin-second-simpledownload-trigger.md
- **Summary:** Created new page "DOTA browser plugin second simpleDownload trigger"

## [2026-06-27T09:11:15.124Z] add
- **Pages:** dota-browser-plugin-second-simpledownload-trigger.md
- **Summary:** Created wiki page dota-browser-plugin-second-simpledownload-trigger.md

## [2026-06-27T09:19:00.469Z] ingest
- **Pages:** dota-browser-plugin-second-simpledownload-trigger.md
- **Summary:** Updated "DOTA browser plugin second simpleDownload trigger" with new content

## [2026-06-27T09:22:28.614Z] ingest
- **Pages:** dota-browser-plugin-second-simpledownload-trigger.md
- **Summary:** Updated "DOTA browser plugin second simpleDownload trigger" with new content

## [2026-06-27T09:24:55.248Z] ingest
- **Pages:** dota-browser-plugin-second-simpledownload-trigger.md
- **Summary:** Updated "DOTA browser plugin second simpleDownload trigger" with new content

## [2026-06-27T12:58:15.667Z] ingest
- **Pages:** dota-browser-plugin-record-pack-slot-fix.md
- **Summary:** Created new page "DOTA Browser Plugin Record Pack Slot Fix"

## [2026-06-27T12:58:15.680Z] add
- **Pages:** dota-browser-plugin-record-pack-slot-fix.md
- **Summary:** Created wiki page dota-browser-plugin-record-pack-slot-fix.md

## [2026-06-27T13:11:05.398Z] ingest
- **Pages:** dota-browser-plugin-record-pack-slot-fix.md
- **Summary:** Updated "DOTA Browser Plugin Record Pack Slot Fix" with new content

## [2026-06-27T13:12:04.987Z] ingest
- **Pages:** dota-browser-plugin-record-pack-slot-fix.md
- **Summary:** Updated "DOTA Browser Plugin Record Pack Slot Fix" with new content

## [2026-06-27T13:14:57.236Z] ingest
- **Pages:** dota-browser-plugin-record-pack-slot-fix.md
- **Summary:** Updated "DOTA Browser Plugin Record Pack Slot Fix" with new content

## [2026-06-27T15:02:02.464Z] ingest
- **Pages:** dota-browser-plugin-record-pack-slot-fix.md
- **Summary:** Updated final DOTA browser plugin record/table[100] alias fix and verification evidence

## [2026-06-27T15:07:39.467Z] ingest
- **Pages:** dota-browser-plugin-record-pack-slot-fix.md
- **Summary:** Refreshed final DOTA browser plugin verification evidence from /tmp/vmrp-e2e-GSqFvj
## [2026-06-27T15:46:20.555Z] ingest
- **Pages:** dota-download-browser-components-transition-frame-fix.md
- **Summary:** Created new page "DOTA download-browser-components transition frame fix"

## [2026-06-27T15:46:20.558Z] add
- **Pages:** dota-download-browser-components-transition-frame-fix.md
- **Summary:** Created wiki page dota-download-browser-components-transition-frame-fix.md

## [2026-06-27T15:51:29.852Z] ingest
- **Pages:** dota-download-browser-components-waiting-hypothesis-invalidated.md
- **Summary:** Created new page "DOTA download-browser-components waiting hypothesis invalidated"

## [2026-06-27T15:51:29.856Z] add
- **Pages:** dota-download-browser-components-waiting-hypothesis-invalidated.md
- **Summary:** Created wiki page dota-download-browser-components-waiting-hypothesis-invalidated.md

## [2026-06-27T23:30:09Z] ingest
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-download-browser-components-transition-frame-fix.md
- **Summary:** Recorded final child-scoped table[125] package-provenance fix for DOTA browser component logo/text rendering and marked the earlier transition-frame waiting fix as superseded
## [2026-06-27T23:54:44.678Z] ingest
- **Pages:** optwar-exit-plugin-no-network-investigation.md
- **Summary:** Created new page "Optwar exit plugin no-network investigation"

## [2026-06-27T23:54:44.681Z] add
- **Pages:** optwar-exit-plugin-no-network-investigation.md
- **Summary:** Created wiki page optwar-exit-plugin-no-network-investigation.md

## [2026-06-28T00:09:48.398Z] ingest
- **Pages:** optwar-exit-plugin-no-network-investigation.md
- **Summary:** Updated "Optwar exit plugin no-network investigation" with new content
## [2026-06-28T05:34:02.325Z] ingest
- **Pages:** optwar-exit-plugin-no-network-investigation.md
- **Summary:** Updated "Optwar exit plugin no-network investigation" with new content

## [2026-06-28T05:39:47.805Z] ingest
- **Pages:** optwar-exit-plugin-no-network-investigation.md
- **Summary:** Updated "Optwar exit plugin no-network investigation" with new content
## [2026-07-01T10:59:31.770Z] query
- **Pages:** dota-browser-plugin-second-simpledownload-trigger.md
- **Summary:** Query "wbrw input paste editGetText editGetText table77" → 1 results (of 1 total)

## [2026-07-01T11:08:41.053Z] ingest
- **Pages:** wbrw-input-paste-ucs2-table77-fix.md
- **Summary:** Created new page "wbrw input paste UCS2 table77 fix"

## [2026-07-01T11:08:41.058Z] add
- **Pages:** wbrw-input-paste-ucs2-table77-fix.md
- **Summary:** Created wiki page wbrw-input-paste-ucs2-table77-fix.md
## [2026-07-05T14:42:16.444Z] ingest
- **Pages:** gzwdzjs-arm-ext-drawbitmap-source-overrun.md
- **Summary:** Created new page "gzwdzjs ARM EXT DrawBitmap source overrun"

## [2026-07-05T14:42:16.454Z] add
- **Pages:** gzwdzjs-arm-ext-drawbitmap-source-overrun.md
- **Summary:** Created wiki page gzwdzjs-arm-ext-drawbitmap-source-overrun.md

## [2026-07-06T13:15:59.658Z] ingest
- **Pages:** gzwdzjs-table-bridge-stale-epilogue-guard.md
- **Summary:** Created new page "gzwdzjs table bridge stale epilogue guard"

## [2026-07-06T13:15:59.662Z] add
- **Pages:** gzwdzjs-table-bridge-stale-epilogue-guard.md
- **Summary:** Created wiki page gzwdzjs-table-bridge-stale-epilogue-guard.md
## [2026-07-06T15:00:43.952Z] lint
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-input-paste-ucs2-table77-fix.md
- **Summary:** Lint: 18 issues (15 orphan, 0 stale, 2 broken, 0 contradictions)

## [2026-07-06T15:00:56.228Z] lint
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-input-paste-ucs2-table77-fix.md
- **Summary:** Lint: 18 issues (15 orphan, 0 stale, 2 broken, 0 contradictions)

## [2026-07-06T15:01:06.245Z] lint
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-input-paste-ucs2-table77-fix.md
- **Summary:** Lint: 16 issues (15 orphan, 0 stale, 0 broken, 0 contradictions)
## [2026-07-11T08:57:36.289Z] ingest
- **Pages:** wbrw-proxy2-font-mrp-download.md
- **Summary:** Created new page "WBRW proxy2 font MRP download"
## [2026-07-11T09:13:36.888Z] ingest
- **Pages:** wbrw-proxy2-font-mrp-download.md
- **Summary:** Updated "WBRW proxy2 font MRP download" with new content
## [2026-07-11T09:36:24.799Z] ingest
- **Pages:** wbrw-proxy2-font-mrp-download.md
- **Summary:** Updated "WBRW proxy2 font MRP download" with new content
## [2026-07-11T11:28:02.844Z] ingest
- **Pages:** wbrw-proxy2-font-mrp-download.md
- **Summary:** Updated "WBRW proxy2 font MRP download" with new content
## [2026-07-11T11:28:28.562Z] lint
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-proxy2-font-mrp-download.md, wbrw-proxy2-web-page-rendering.md
- **Summary:** Lint: 18 issues (15 orphan, 0 stale, 1 broken, 0 contradictions)
## [2026-07-12T01:14:44.827Z] ingest
- **Pages:** wbrw-manual-clipboard-and-menu-exit-verification.md
- **Summary:** Created new page "WBRW manual clipboard and menu exit verification"

## [2026-07-12T01:14:44.831Z] add
- **Pages:** wbrw-manual-clipboard-and-menu-exit-verification.md
- **Summary:** Created wiki page wbrw-manual-clipboard-and-menu-exit-verification.md

## [2026-07-12T01:19:32.846Z] lint
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-manual-clipboard-and-menu-exit-verification.md, wbrw-proxy2-font-mrp-download.md, wbrw-proxy2-web-page-rendering.md
- **Summary:** Lint: 19 issues (16 orphan, 0 stale, 1 broken, 0 contradictions)

## [2026-07-12T01:21:38.745Z] lint
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-proxy2-font-mrp-download.md, wbrw-proxy2-web-page-rendering.md
- **Summary:** Lint: 18 issues (15 orphan, 0 stale, 1 broken, 0 contradictions)

## [2026-07-12T10:09:53+08:00] ingest
- **Pages:** wbrw-manual-clipboard-and-menu-exit-verification.md
- **Summary:** Corrected the premature zero-delay result; recorded the edit-mode one-shot timer loss, generic main-loop fix, no-SCREEN clean and persisted-state verification, PPM evidence, guest exit, and cross-application regressions.
## [2026-07-12T04:04:54.545Z] ingest
- **Pages:** gzwdzjs-play-mr-ucs2gb-caller-owned-output-pointer-fix.md
- **Summary:** Created new page "gzwdzjs play MR_UCS2GB caller-owned output pointer fix"
## [2026-07-12T04:06:31.247Z] lint
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-play-mr-ucs2gb-caller-owned-output-pointer-fix.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-proxy2-font-mrp-download.md, wbrw-proxy2-web-page-rendering.md
- **Summary:** Lint: 19 issues (16 orphan, 0 stale, 1 broken, 0 contradictions)
## [2026-07-12T10:37:19.291Z] ingest
- **Pages:** wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Created new page "WBRW Go html2sky rendering progress 2026-07-12"

## [2026-07-12T10:37:19.318Z] add
- **Pages:** wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Created wiki page wbrw-go-html2sky-rendering-progress-2026-07-12.md

## [2026-07-12T11:32:16.280Z] ingest
- **Pages:** wbrw-go-html2sky-rendering-final-verification-2026-07-12.md
- **Summary:** Created new page "WBRW Go html2sky rendering final verification 2026-07-12"

## [2026-07-12T11:32:16.286Z] add
- **Pages:** wbrw-go-html2sky-rendering-final-verification-2026-07-12.md
- **Summary:** Created wiki page wbrw-go-html2sky-rendering-final-verification-2026-07-12.md

## [2026-07-12T11:41:32.802Z] ingest
- **Pages:** wbrw-html2sky-completion-audit-2026-07-12-latest.md
- **Summary:** Created new page "WBRW html2sky completion audit 2026-07-12 latest"

## [2026-07-12T11:41:32.809Z] add
- **Pages:** wbrw-html2sky-completion-audit-2026-07-12-latest.md
- **Summary:** Created wiki page wbrw-html2sky-completion-audit-2026-07-12-latest.md

## [2026-07-12T11:46:39.491Z] ingest
- **Pages:** wbrw-html2sky-post-cleanup-final-evidence-2026-07-12.md
- **Summary:** Created new page "WBRW html2sky post-cleanup final evidence 2026-07-12"

## [2026-07-12T11:46:39.500Z] add
- **Pages:** wbrw-html2sky-post-cleanup-final-evidence-2026-07-12.md
- **Summary:** Created wiki page wbrw-html2sky-post-cleanup-final-evidence-2026-07-12.md

## [2026-07-12T12:38:25.071Z] ingest
- **Pages:** wbrw-html2sky-native-elements-and-ppm-verification-2026-07-12.md
- **Summary:** Created new page "WBRW html2sky native elements and PPM verification 2026-07-12"

## [2026-07-12T12:49:59.988Z] ingest
- **Pages:** wbrw-html2sky-native-elements-and-ppm-verification-2026-07-12.md
- **Summary:** Updated "WBRW html2sky native elements and PPM verification 2026-07-12" with new content

## [2026-07-12T12:50:00.319Z] lint
- **Pages:** dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-play-mr-ucs2gb-caller-owned-output-pointer-fix.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-go-html2sky-rendering-final-verification-2026-07-12.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md, wbrw-html2sky-completion-audit-2026-07-12-latest.md, wbrw-html2sky-native-elements-and-ppm-verification-2026-07-12.md, wbrw-html2sky-post-cleanup-final-evidence-2026-07-12.md, wbrw-proxy2-font-mrp-download.md, wbrw-proxy2-web-page-rendering.md
- **Summary:** Lint: 25 issues (21 orphan, 0 stale, 1 broken, 1 contradictions)

## [2026-07-12T13:06:43.621Z] ingest
- **Pages:** wbrw-html2sky-native-elements-and-ppm-verification-2026-07-12.md
- **Summary:** Updated "WBRW html2sky native elements and PPM verification 2026-07-12" with new content

## [2026-07-13T03:47:00Z] ingest
- **Pages:** wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Recorded the completed proxy2 bitmap-logo fix, selector-12 decoder evidence, generic PNG/JPEG-to-GIF adaptation, and final no-xvfb PPM proof.

## [2026-07-13T03:55:50Z] verify
- **Pages:** wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Re-ran the no-xvfb bitmap E2E after explicit image-request validation, retained and inspected the final PPM, and refreshed its pixel, cache, and hash evidence.

## [2026-07-13T04:00:06Z] verify
- **Pages:** wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Hardened GIF/BMP pass-through against malformed streams, re-ran backend checks and the no-xvfb E2E, and replaced the final retained PPM evidence.

## [2026-07-13T04:23:29Z] complete
- **Pages:** wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Closed the generic WBRW cache-publication gate, added strict image envelopes and resource limits, restored footer-band coverage, and retained the final no-xvfb bitmap PPM/GIF evidence.

## [2026-07-13T04:35:35Z] complete
- **Pages:** wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Completed the post-review GIF preallocation guard, RFC corrected-age handling, concrete image negotiation, quoted cache parsing, and final no-xvfb PPM/GIF verification.

## [2026-07-13T05:38:00Z] verify
- **Pages:** wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Replaced WBRW's exposed transparent-black GIF canvas with a source-derived opaque matte, added a white-background E2E assertion, and retained the final PPM/GIF evidence.

## [2026-07-13T06:01:24Z] complete
- **Pages:** wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Fitted proxy2 images to WBRW tag-31 layout bounds without upscaling or cropping, proved the complete 220x44 logo and rightmost glyph in a no-xvfb PPM, and recorded focused test/race/vet results.

## [2026-07-13T06:35:53Z] complete
- **Pages:** wbrw-go-html2sky-rendering-progress-2026-07-12.md
- **Summary:** Preserved generic CSS background-container identity in html2sky, nested pagination rows and the jump form under one SKY parent, and proved the former white gap is continuously green in the final no-xvfb PPM.

## [2026-07-15T02:48:06Z] complete
- **Pages:** cookie-e2e-key-timer-generation-boundary.md
- **Summary:** Recorded the ARM event-chain evidence and replaced queue/draw/anonymous-timer completion guesses with main-thread short-key release plus timer-generation publication; the unfinished cookie scenario test remains unchanged.

## [2026-07-15T05:09:32Z] complete
- **Pages:** cookie-mrp-restart-and-full-runtime-version-contract.md
- **Summary:** Recorded the four-slot shared-memory handoff repair, RESTART/STOP split, FULL 1968 versus MINI 2011 startup-version regression, rejected native-helper bootstrap, stable DSM PPM proof, and complete 31-test E2E regression.

## [2026-07-15T14:13:48.924Z] ingest
- **Pages:** cookie-child-return-opening-folder-frame-replay.md
- **Summary:** Created new page "Cookie child return opening-folder frame replay"

## [2026-07-15T14:13:48.943Z] add
- **Pages:** cookie-child-return-opening-folder-frame-replay.md
- **Summary:** Created wiki page cookie-child-return-opening-folder-frame-replay.md

## [2026-07-15T14:14:08.770Z] lint
- **Pages:** cookie-child-return-opening-folder-frame-replay.md, cookie-mrp-restart-and-full-runtime-version-contract.md, dota-browser-plugin-dsm-package-provenance.md, dota-browser-plugin-network-state-investigation.md, dota-browser-plugin-record-pack-slot-fix.md, dota-browser-plugin-second-simpledownload-trigger.md, dota-download-browser-components-transition-frame-fix.md, dota-download-browser-components-waiting-hypothesis-invalidated.md, gfhcq-game-stutter-performance-investigation.md, gghjt-modal-timer-and-cold-extraction-regressions.md, gms-payment-latency-analysis.md, gzwdzjs-arm-ext-drawbitmap-source-overrun.md, gzwdzjs-play-mr-ucs2gb-caller-owned-output-pointer-fix.md, gzwdzjs-primary-black-clear-damage-synthesis.md, gzwdzjs-table-bridge-stale-epilogue-guard.md, opglqa-font-download-repeat-investigation.md, optwar-exit-plugin-component-update-stall.md, optwar-exit-plugin-no-network-investigation.md, rx4-5-arm-ext-0x80000000-platform-io-mapping.md, wbrw-go-html2sky-rendering-final-verification-2026-07-12.md, wbrw-go-html2sky-rendering-progress-2026-07-12.md, wbrw-html2sky-completion-audit-2026-07-12-latest.md, wbrw-html2sky-native-elements-and-ppm-verification-2026-07-12.md, wbrw-html2sky-post-cleanup-final-evidence-2026-07-12.md, wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md, wbrw-proxy2-font-mrp-download.md, wbrw-proxy2-web-page-rendering.md
- **Summary:** Lint: 30 issues (24 orphan, 0 stale, 2 broken, 1 contradictions)

## [2026-07-15T14:36:30Z] verify
- **Pages:** cookie-child-return-opening-folder-frame-replay.md
- **Summary:** Re-ran the default parallel E2E suite without `--no-file-parallelism`; 19 files and 31 tests passed. Refreshed retained Cookie PPM evidence at `/tmp/vmrp-e2e-Vdk5ke`, where `return-draw-0047.ppm` shows `正在打开` before the restored file-manager list.

## [2026-07-16T11:10:00+08:00] fix
- **Pages:** cookie-child-return-opening-folder-frame-replay.md
- **Summary:** Corrected direct-ENTER child return by making present-history generations represent distinct full-frame states; paired screen/ARM checkpoints now survive repeated identical startup submits, and retained PPMs prove `正在打开` precedes the exact pre-launch file-manager content.

## [2026-07-16T11:25:00+08:00] verify
- **Pages:** cookie-child-return-opening-folder-frame-replay.md
- **Summary:** Verified the exact direct-ENTER command, both Cookie launch routes, TypeScript, C unit tests, and the default parallel e2e suite; 19 files and 32 tests passed without xvfb in 315.18s.

## [2026-07-16T14:20:00+08:00] fix
- **Pages:** cookie-child-return-opening-folder-frame-replay.md
- **Summary:** Replaced the rejected present-history/frame-replay approach with the real reload-protocol fix: table[138] start_fileparameter is a 128-byte binary "_RL" record; host now memcpy-copies it across the RESTART handoff instead of truncating as a C string, so Cookie's own cold-start path reloads fm.sav, redraws 正在打开, and restores the selection. Removed MrPausedAppSnapshot, present_screen_history, directory-open screenshot replay. Full parallel e2e 19 files/32 tests pass without xvfb.
