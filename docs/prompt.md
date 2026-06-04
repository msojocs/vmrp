分析并修复执行命令`test/gxdzc/download-resource.sh`启动应用后，自动点击进入资源包下载界面，但是点击“取消”没反应。

预期结果：程序返回游戏UI页面，不是重启，不是重启，不是重启。
实际结果：程序没有任何反应。

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
