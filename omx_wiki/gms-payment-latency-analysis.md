---
title: "gms payment latency analysis"
tags: ["vmrp", "gms", "payment", "sms", "arm-ext", "e2e"]
created: 2026-06-26T13:59:05+08:00
updated: 2026-06-26T14:12:07+08:00
sources: ["docs/gms-payment-latency-analysis.md"]
links: []
category: debugging
confidence: high
schemaVersion: 1
---

# gms payment latency analysis

2026-06-26 analysis of `pnpm vitest run test/e2e/gms/game-prepare.test.ts`. The test is only an automated entry path: after confirm payment it checks the payment-confirm UI pixel `[248,80,0]` and sleeps `5s`; it does not wait for a payment-success state. Most of the test's roughly 62s runtime is scripted navigation: default e2e key hold around `500ms`, many `1000ms` delays, and 28 `ENTER` steps.

The post-click delay is in the GMS app's own `cfunction.ext` payment/SMS confirm state machine, not in the platform SMS bridge. Disassembly and narrow runtime probes showed `_strCom(801)` event dispatch through `0xEA4238`, timer dispatch through `0xEA70B0`, charge/SMS setup around `0xEA207C`, `mr_sendSms` wrapper `0xEA15CC`, call/return at `0xE9A3F0/0xE9A3F4`, and state record `0xEA5C5C`. `mr_sendSms(10668001)` returns synchronously with `MR_SUCCESS`, then GMS immediately logs `ChrageExOverSea Ret -- 0` and writes `mythroad/gms/ms.paydate`.

Long PPM observation showed the original success transition around 30s: hash changed from `f36a187e1797` to `dd716938a16f`, and the user confirmed `dd716938a16f` is the payment-success screen. `pixel(61,119)` stays `[248,80,0]` on both screens, so that pixel alone cannot distinguish confirm from success. Focused PC watch saw only `EA207C`, `E9A3F4`, `EA5C5C`, and normal event checks after confirm; it did not hit `reConfirm_nextSms` (`0xEA8044`), secondary send helper (`0xEA8124`), or `reConfirm_smsCheck` (`0xEA8400/0xEA8584`) during the observed payment wait.

Root cause and fix: GMS passes sendSms flags `24` (`MR_SMS_REPORT_FLAG | MR_SMS_RESULT_FLAG`), so the platform should send an asynchronous `MR_SMS_RESULT` after `mr_sendSms` returns. The emulator only returned synchronously, causing GMS to wait for its roughly 30s fallback. Implemented a generic ARM EXT fix in `table[59]`: when `mr_sendSms` succeeds and flags include `MR_SMS_RESULT_FLAG`, queue a pending result and dispatch `MR_SMS_RESULT` at the next safe host boundary, outside the sendSms call stack. This is not GMS-specific and does not touch GMS state slots or the embedded `30000` timeout. Verification: after the fix, `payment-long-probe` reaches success hash `dd716938a16f` at confirm+1s and stays there through 90s; the probe passed.
