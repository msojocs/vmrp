修复执行命令`build/vmrp mythroad/gghjt.mrp`启动应用后，按特定步骤进入付费步骤，点击付费后卡死；

程序的现有代码不一定正确。
注意trace日志量会特别大。
不要使用xvfb。
修改代码要加注释，不要有兜底代码逻辑。
你可以检测PPM，来验证修复是否成功。
使用反汇编去分析BUG。
模拟点击的每个点击事件都添加自定义延迟，然后我会做成可复现的脚本。

注意：
再解决完BUG后，再去考虑兼容`build/vmrp mythroad/nes.mrp`
`build/vmrp mythroad/dota.mrp`
`build/vmrp mythroad/mrpinfo.mrp`

日志：temp/crash.log