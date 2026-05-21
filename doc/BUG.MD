分析项目解决BUG：
项目根据`doc/cfunction_x86_refactor.md`脱离了对`cfunction.ext`的依赖。
脱离前后`build/vmrp`渲染正常，但是脱离后`mpc.mrp`渲染异常；分析并解决这个黑屏问题。

日志：
./build/vmrp
__x86_64__
CODE_ADDRESS:0x80000, CODE_SIZE:0x100000
STACK_ADDRESS:0x180000, STACK_SIZE:0x100000
MEMORY_MANAGER_ADDRESS:0x280000, MEMORY_MANAGER_SIZE:0x600000
START_ADDRESS:0x80000, END_ADDRESS:0x880000
TOTAL_MEMORY:0x800000(8388608)
native dsm_init success
dsm_gm.mrp
native_mem_get base=0x41fc8000 len=4194304(4096 kb)
got Origin_LG_mem_len:4194304
Total memory:4194304
[INFO]mr_open(mythroad/system/gb16.uc2,1) fd is: 1
[INFO]font load suc fd=1
Used by VM(include screen buffer):192184 bytes
[INFO]mr_open(mythroad/dsm_gm.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(12576)
_mr_readFile reallen:37230
mr_unzip
[INFO]mr_open(mythroad/dsm_gm.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(13124)
_mr_readFile reallen:42449
mr_unzip
[INFO]mr_platEx code=1201 in=@0000000041FF5410 inlen=1 out=@000000005B700F18 outlen=@000000005B700F30 cb=@000000005B700F10
load  tim ss is 
[INFO]mr_open(mythroad/dsm_gm.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(51)
_mr_readFile reallen:14400
mr_unzip
[INFO]mr_open(mythroad/dsm_gm.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(3078)
_mr_readFile reallen:7200
mr_unzip
[INFO]mr_open(mythroad/dsm_gm.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(6685)
_mr_readFile reallen:18712
mr_unzip
[INFO]mr_open(mythroad/dsm_gm.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(2858)
_mr_readFile reallen:8538
mr_unzip
[INFO]mr_open(mythroad/app.cfg,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_open(mythroad/dsm_gm.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(1816)
_mr_readFile reallen:3584
mr_unzip
[INFO]mr_open(mythroad/applist.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_open(mythroad/applist.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(215)
nozip
error
version is 0
[INFO]mr_findStart(mythroad/)
[INFO]mr_findGetNext 1 gghjt.mrp
[INFO]mr_open(mythroad/gghjt.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_findGetNext 1 time.py.sys
[INFO]mr_findGetNext 1 qqlist8240x320
[INFO]mr_findGetNext 1 plugins
[INFO]mr_findGetNext 1 system
[INFO]mr_findGetNext 1 c
[INFO]mr_findGetNext 1 .
[INFO]mr_findGetNext 1 ydqtwo.mrp
[INFO]mr_open(mythroad/ydqtwo.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_findGetNext 1 ..
[INFO]mr_findGetNext 1 mpc.mrp
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_findGetNext 1 applist.mrp
[INFO]mr_findGetNext 1 dsm_gm.mrp
[INFO]mr_findGetNext 1 disk
[INFO]mr_findGetNext 1 app240320
[INFO]mr_findGetNext 1 app.cfg
[INFO]mr_findGetNext 1 (NULL)
After app init, memory left:3768416
vmrp_runtime_start_dsm('dsm_gm.mrp','start.mr',''): 0x0
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
native_mem_get base=0x41fc8000 len=4194304(4096 kb)
got Origin_LG_mem_len:4194304
Total memory:4194304
[INFO]mr_open(mythroad/system/gb16.uc2,1) fd is: 2
[INFO]font load suc fd=2
Used by VM(include screen buffer):192184 bytes
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 3
[INFO]mr_close(3): ret:0
mr_get_method(1018)
_mr_readFile reallen:2490
mr_unzip
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 3
[INFO]mr_close(3): ret:0
mr_get_method(13382)
_mr_readFile reallen:19532
mr_unzip
[INFO]mr_platEx code=1204 in=@0000000042872828 inlen=1 out=@00000000177E22B8 outlen=@00000000177E22C0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 00000000177E22B8, 00000000177E22C0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000042872828 inlen=1 out=@00000000177E22B8 outlen=@00000000177E22C0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 00000000177E22B8, 00000000177E22C0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000042E15B84 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 3
[INFO]mr_platEx code=1204 in=@0000000042E15BC4 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
Mem-S: WARNING:no memory,need[665]
Mem-S: WARNING:no memory,need[128]
[INFO]mr_close(3): ret:0
MEM-I:total:4194304,iTop:0,ctrl:614400
[INFO]mr_platEx code=1204 in=@0000000042872828 inlen=1 out=@00000000177E22B8 outlen=@00000000177E22C0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 00000000177E22B8, 00000000177E22C0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000042E15B84 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 3
[INFO]mr_platEx code=1204 in=@0000000042E15BC4 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(3): ret:0
[INFO]mr_platEx code=1204 in=@0000000042872828 inlen=1 out=@00000000177E22B8 outlen=@00000000177E22C0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 00000000177E22B8, 00000000177E22C0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000042E15B84 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 3
[INFO]mr_platEx code=1204 in=@0000000042E15BC4 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(3): ret:0
[INFO]mr_platEx code=1204 in=@0000000042872828 inlen=1 out=@00000000177E22B8 outlen=@00000000177E22C0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 00000000177E22B8, 00000000177E22C0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000042E15B84 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 3
[INFO]mr_close(3): ret:0
[INFO]mr_platEx code=1204 in=@0000000042E15BC4 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_platEx code=1204 in=@0000000042872828 inlen=1 out=@00000000177E22B8 outlen=@00000000177E22C0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 00000000177E22B8, 00000000177E22C0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000042E15B84 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(mythroad/mpc.mrp,1) fd is: 3
[INFO]mr_platEx code=1204 in=@0000000042E15BC4 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(3): ret:0
mr_get_method(84090)
_mr_readFile reallen:157040
mr_unzip
[WARN]mr_cacheSync(0x0000000042E16240, 157120)
After app init, memory left:3811552