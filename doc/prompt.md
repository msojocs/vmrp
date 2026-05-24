修复执行命令`build/vmrp mythroad/gghjt.mrp`应用启动失败；
@doc/gghjt-fix-report.md
程序的现有代码不一定正确。
注意trace日志量会特别大。
不要使用xvfb。
修改代码要加注释。

build/vmrp mythroad/mpc.mrp 
__x86_64__
CODE_ADDRESS:0x80000, CODE_SIZE:0x100000
STACK_ADDRESS:0x180000, STACK_SIZE:0x100000
MEMORY_MANAGER_ADDRESS:0x280000, MEMORY_MANAGER_SIZE:0x600000
START_ADDRESS:0x80000, END_ADDRESS:0x880000
TOTAL_MEMORY:0x800000(8388608)
native dsm_init success
/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp
native_mem_get base=0x40b28000 len=4194304(4096 kb)
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
[INFO]mr_platEx code=1204 in=@0000000052071828 inlen=1 out=@000000006845A198 outlen=@000000006845A1A0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000006845A198, 000000006845A1A0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000052071828 inlen=1 out=@000000006845A198 outlen=@000000006845A1A0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000006845A198, 000000006845A1A0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000052632CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_platEx code=1204 in=@0000000052632D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
Mem-S: WARNING:no memory,need[665]
Mem-S: WARNING:no memory,need[128]
[INFO]mr_close(2): ret:0
MEM-I:total:4194304,iTop:968,ctrl:614400
[INFO]mr_platEx code=1204 in=@0000000052071828 inlen=1 out=@000000006845A198 outlen=@000000006845A1A0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000006845A198, 000000006845A1A0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000052632CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_platEx code=1204 in=@0000000052632D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(2): ret:0
[INFO]mr_platEx code=1204 in=@0000000052071828 inlen=1 out=@000000006845A198 outlen=@000000006845A1A0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000006845A198, 000000006845A1A0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000052632CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_platEx code=1204 in=@0000000052632D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(2): ret:0
[INFO]mr_platEx code=1204 in=@0000000052071828 inlen=1 out=@000000006845A198 outlen=@000000006845A1A0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000006845A198, 000000006845A1A0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000052632CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_close(2): ret:0
[INFO]mr_platEx code=1204 in=@0000000052632D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_platEx code=1204 in=@0000000052071828 inlen=1 out=@000000006845A198 outlen=@000000006845A1A0 cb=@0000000000000000
[INFO]dsmSwitchPath 'Y', 1, 000000006845A198, 000000006845A1A0
[WARN]dsmSwitchPathBuf: y 'c:/mythroad/'
[INFO]mr_platEx code=1204 in=@0000000052632CD0 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_open(/home/msojocs/github/vmrp/wasm/dist/fs/mythroad/mpc.mrp,1) fd is: 2
[INFO]mr_platEx code=1204 in=@0000000052632D10 inlen=12 out=@0000000000000000 outlen=@0000000000000000 cb=@0000000000000000
[INFO]dsmSwitchPath 'c:/mythroad/', 12, 0000000000000000, 0000000000000000
[WARN]SetDsmWorkPath():'mythroad/'
[INFO]mr_close(2): ret:0
mr_get_method(84090)
_mr_readFile reallen:157040
mr_unzip
[WARN]mr_cacheSync(0x0000000052633380, 157120)
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
[CLICK] #1 down x=17 y=312
[CLICK] #1 down ret=0
[CLICK] #1 up x=17 y=312
[INFO]mr_open(mythroad/mrcc/cfg.dat,12) fd is: 2
[INFO]mr_close(2): ret:0
[CLICK] #1 up ret=0
[CLICK] #2 down x=26 y=287
[CLICK] #2 down ret=0
[CLICK] #2 up x=26 y=287
[CLICK] #2 up ret=0
[CLICK] #3 down x=12 y=304
[CLICK] #3 down ret=0
[CLICK] #3 up x=12 y=304
[CLICK] #3 up ret=0