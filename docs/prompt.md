修复执行命令`build/vmrp mythroad/mpc.mrp`启动应用后，按特定步骤进入退出步骤，预期是退出应用，现在实际是没有退出；

复现脚本：test/mpc-exit.sh

程序的现有代码不一定正确。
注意trace日志量会特别大。
不要使用xvfb。
修改代码要加注释，不要有兜底代码逻辑。
你可以检测PPM，来验证修复是否成功。
使用反汇编去深入分析BUG。

注意：
在解决完BUG后，再去考虑兼容`build/vmrp mythroad/nes.mrp`
`build/vmrp mythroad/dota.mrp`
`build/vmrp mythroad/mrpinfo.mrp`

参考信息：
根本原因：wrapper的cfunction.ext中的timer管理函数（0x84298）使用 R5 = saved_time - getTime() 来计算elapsed。当
  saved_time ≤ getTime()时，R5被clamp到0，timer node的countdown不减少，回调永远不触发。

  在真机上，ARM指令执行消耗真实时间（几十微秒/指令），每次mr_getTime()调用之间值自然递增几ms。Unicorn同步执行ARM代
  码时，两次mr_getTime()在宿主的同一毫秒内完成，返回相同值，elapsed=0，countdown冻结。

  修复方向已确认：需要让mr_getTime()在Unicorn
  ARM执行期间自然推进（不是人为+1），使得wrapper内部两次getTime调用之间的差值>0。简单的usleep(N)方法因为usleep精度
  和调用次数的关系，效果不稳定且影响其他应用性能。需要一个更精确的方案——可能需要在Unicorn的instruction
  hook中累积虚拟时间，或者直接修补wrapper二进制中的clamp逻辑。