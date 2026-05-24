修复执行命令`build/vmrp mythroad/mpc.mrp`启动应用后，点击界面的退出选项没有反应，预期是退出程序；

程序的现有代码不一定正确。
注意trace日志量会特别大。
不要使用xvfb。
修改代码要加注释，不要有兜底代码逻辑。
你可以模拟日志的点击位置触发问题，来验证修复是否成功。

build/vmrp mythroad/mpc.mrp 
__x86_64__
CODE_ADDRESS:0x80000, CODE_SIZE:0x100000
STACK_ADDRESS:0x180000, STACK_SIZE:0x100000
MEMORY_MANAGER_ADDRESS:0x280000, MEMORY_MANAGER_SIZE:0x600000
START_ADDRESS:0x80000, END_ADDRESS:0x880000
TOTAL_MEMORY:0x800000(8388608)
native dsm_init success
/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp
native_mem_get base=0x40a5f000 len=4194304(4096 kb)
got Origin_LG_mem_len:4194304
Total memory:4194304
[INFO]mr_open(mythroad/system/gb16.uc2,1) fd is: 1
[INFO]font load suc fd=1
Used by VM(include screen buffer):192184 bytes
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(1018)
_mr_readFile reallen:2490
mr_unzip
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
mr_get_method(13382)
_mr_readFile reallen:19532
mr_unzip
[INFO]mr_platEx code=1204 in=@000000005E872828 inlen=1 out=@000000005A7C5C08 outlen=@000000005A7C5C10 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000005A7C5C08, 000000005A7C5C10
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@000000005E872828 inlen=1 out=@000000005A7C5C08 outlen=@000000005A7C5C10 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000005A7C5C08, 000000005A7C5C10
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@000000005EE33CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_platEx code=1204 in=@000000005EE33D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
Mem-S: WARNING:no memory,need[665]
Mem-S: WARNING:no memory,need[128]
[INFO]mr_close(2): ret:0
MEM-I:total:4194304,iTop:968,ctrl:614400
[INFO]mr_platEx code=1204 in=@000000005E872828 inlen=1 out=@000000005A7C5C08 outlen=@000000005A7C5C10 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000005A7C5C08, 000000005A7C5C10
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@000000005EE33CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_platEx code=1204 in=@000000005EE33D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(2): ret:0
[INFO]mr_platEx code=1204 in=@000000005E872828 inlen=1 out=@000000005A7C5C08 outlen=@000000005A7C5C10 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000005A7C5C08, 000000005A7C5C10
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@000000005EE33CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_platEx code=1204 in=@000000005EE33D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(2): ret:0
[INFO]mr_platEx code=1204 in=@000000005E872828 inlen=1 out=@000000005A7C5C08 outlen=@000000005A7C5C10 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000005A7C5C08, 000000005A7C5C10
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@000000005EE33CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_platEx code=1204 in=@000000005EE33D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_platEx code=1204 in=@000000005E872828 inlen=1 out=@000000005A7C5C08 outlen=@000000005A7C5C10 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000005A7C5C08, 000000005A7C5C10
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@000000005EE33CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_platEx code=1204 in=@000000005EE33D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(2): ret:0
mr_get_method(84090)
_mr_readFile reallen:157040
mr_unzip
[WARN]mr_cacheSync(0x000000005EE34380, 157120)
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_open(mythroad/mrcc/cfg.dat,12) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_mkDir(mythroad/c/)
[INFO]mr_open(mythroad/mrcc/cfg.dat,12) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_open(mythroad/mrcc/cfg.dat,12) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
read file  "#807022#*" err, code=3006
mrc_verifyMrpCertificate_V1 as DBG MODE
After app init, memory left:3811504
vmrp_runtime_start_dsm('/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp','start.mr',''): 0x0
[CLICK] #1 down x=23 y=305
[CLICK] #1 down ret=0
[CLICK] #1 up x=23 y=305
[INFO]mr_open(mythroad/mrcc/cfg.dat,12) fd is: 2
[INFO]mr_close(2): ret:0
[CLICK] #1 up ret=0
[CLICK] #2 down x=30 y=280
[CLICK] #2 down ret=0
[CLICK] #2 up x=30 y=280
[CLICK] #2 up ret=0
[CLICK] #3 down x=9 y=310
[CLICK] #3 down ret=0
[CLICK] #3 up x=9 y=310
[CLICK] #3 up ret=0