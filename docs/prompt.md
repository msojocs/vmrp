修复执行命令`build/vmrp mythroad/gghjt.mrp`启动应用后，按特定步骤进入付费步骤，等待超时后点击返回，预期是返回游戏，现在实际是卡住不动，只能kill；

复现脚本：test/bug.sh

程序的现有代码不一定正确。
注意trace日志量会特别大。
不要使用xvfb。
修改代码要加注释，不要有兜底代码逻辑。
你可以检测PPM，来验证修复是否成功。
使用反汇编去分析BUG。

理论上超时条件下，点击返回的行为与`test/normal-back.sh`一致，都是加载pak。
注意不是重启！注意不是重启！注意不是重启！注意不是重启！

真机：
付费界面 - 超时 - 点击返回 - 黑屏“请稍等” - 进入游戏菜单界面
付费界面 - 点击取消 - 黑屏“请稍等” - 进入游戏菜单界面
付费界面 - 点击确定 - 付费成功 - 进入游戏游玩界面

注意：
在解决完BUG后，再去考虑兼容`build/vmrp mythroad/nes.mrp`
`build/vmrp mythroad/dota.mrp`
`build/vmrp mythroad/mrpinfo.mrp`