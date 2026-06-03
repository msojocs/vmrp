分析并修复执行命令`build/vmrp mythroad/gxdzc.mrp`启动应用后，黑屏。

程序的现有代码不一定正确。
注意trace日志量会特别大。
不要使用xvfb。
修改代码要加注释，不要有兜底代码逻辑。
反汇编检测BUG所在位置。
你可以检测PPM，来验证修复是否成功。

注意：
在解决完BUG后，再去考虑兼容`build/vmrp mythroad/nes.mrp`
`build/vmrp mythroad/dota.mrp`
`build/vmrp mythroad/mrpinfo.mrp`
