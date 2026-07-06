#!/usr/bin/env bash
seq='-2,0,3000;-4,0,1000;-4,0,1000;-4,0,1000;-4,0,1000;-4,0,1000;-4,0,1000;-4,0,15000;-4,0,15000';
timeout 90s env VMRP_AUTO_CLICK_DELAY_MS=5000 VMRP_AUTO_CLICKS="$seq" build/vmrp build/mythroad/gzwdzjs.mrp