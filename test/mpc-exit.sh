#!/bin/bash
# up和down要间隔500ms；每个事件都显式带 delay，便于复现付费超时路径。
export VMRP_AUTO_CLICKS="14,308,500;44,285,500;14,308,500"
build/vmrp mythroad/mpc.mrp