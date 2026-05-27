 修复 mrpinfo.mrp 退出按钮：table[31/32] dispatch 的 R9 上下文问题

     Context

     mrpinfo.mrp 使用 wrapper（cfunction.ext）加载嵌套 ext（game.ext）。game.ext 在 mr_c_function_load 期间将
     table[31]（timerStart）和 table[32]（timerStop）覆盖为自己的 dispatch 函数（game.ext offset 0x4D74/0x4DC0）。这些
     dispatch 函数通过 P->mrc_extChunk 的函数指针回调 wrapper（cfunction.ext）代码。

     当前的 guard 机制在 arm_ext_call 路由到 primary（嵌套 ext）时，临时将 table[31/32] 恢复为 hook 地址以避免 R9
     崩溃。这使得 timer/rendering 工作正常，但退出按钮不工作——因为 dispatch 函数被绕过了，而退出逻辑在 dispatch 链中。

     根因

     dispatch 函数（game.ext 0x4D74）的调用链：
     1. 读 P->mrc_extChunk，取 [extChunk+40] 得到 wrapper 函数指针
     2. BLX 调用该 wrapper 函数
     3. wrapper 函数用 R9 + offset 访问全局变量 → R9 是嵌套 ext 的值，不是 wrapper 的 → 崩溃

     非 native 路径下有 fixR9_saveMythroad()/fixR9_begin()/fixR9_end() 在 ext 和 mythroad 之间切换 R9。但 native 路径下
     fixR9 是空操作。

     修复方案

     在 hook_table 中拦截 table[31] 和 table[32] 的调用，当检测到来自嵌套 ext 代码区域的调用（通过 LR 判断）时，先切换 R9
     为 wrapper 的值，执行 dispatch 函数，再恢复 R9。

     具体实现

     修改文件：src/arm_ext_executor.c

     1. 移除 arm_ext_call 中的 guard 机制

     当前的 guard 在所有 primary 调用时替换 table[31/32] 为 hook。改为不替换——让 dispatch 函数正常存在。

     2. 在 hook_table case 31/32 中增加 dispatch 转发逻辑

     当 ARM 代码调用 table[31] 或 table[32] 时，hook_table 被触发。当前直接调用 mr_timerStart/mr_timerStop。

     新逻辑：
     - 检查 table[31/32] 的实际值是否已被覆盖（不等于默认 hook 地址）
     - 如果已覆盖，说明有 dispatch 函数。读取覆盖的地址（dispatch 函数地址）
     - 在调用 dispatch 函数前，保存当前 R9，设置 R9 为 wrapper 的 m->p_addr[0]（start_of_ER_RW）
     - 通过 run_arm 执行 dispatch 函数（以正确的 R9）
     - 执行后恢复 R9
     - 用 dispatch 的返回值作为 table call 的结果

     伪代码：
     case 31: {
         uint32_t real_addr = 0;
         memcpy(&real_addr, arm_ptr(m, EXT_TABLE_ADDR + 31*4), 4);
         if (real_addr != EXT_TABLE_ADDR + 31*4 && m->p_addr) {
             // dispatch 函数存在，需要 R9 上下文切换
             uint32_t saved_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
             uint32_t wrapper_rw = 0;
             memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
             if (wrapper_rw) reg_write32(m->uc, UC_ARM_REG_R9, wrapper_rw);
             // 设置参数并调用 dispatch
             // ... run_arm(m, real_addr) ...
             reg_write32(m->uc, UC_ARM_REG_R9, saved_r9);  // 恢复
             ret = reg_read32(m->uc, UC_ARM_REG_R0);
         } else {
             // 标准处理
             ret = mr_timerStart((uint16)r0);
             internal_slot_write(m, m->mr_timer_state_slot, 1);
             mr_timer_state = 1;
         }
     } break;

     3. 移除 arm_ext_call 中的 guard/saved_t31/t32 代码

     不再需要临时替换 table 条目。dispatch 函数保留原值，在 hook_table 层面处理 R9。

     4. 清理 init_guard 相关代码

     移除 arm_ext_set_init_guard、init_guard 字段等。

     为什么这个方案可行

     - dispatch 函数被 game.ext 写入 table[31/32]
     - 当 ARM 代码执行 BLX table[31] 时，unicorn 跳到 table[31] 的值（dispatch 地址）
     - 但 table[31] 的地址仍是 hook 区域（EXT_TABLE_ADDR + 31*4 = 0x1007C），所以 hook_table 仍会被触发
     - 关键：hook_table 拦截在 dispatch 函数执行之前。我们可以在 hook 中读取 dispatch 地址并手动执行它

     等等，这不对。当 game.ext 覆盖 table[31] 的值后，[0x1007C] 的内容不再是 0x1007C（自引用），而是 dispatch 函数地址（如
     0x64AE6C）。ARM 代码做 LDR r0, [table+0x7C]; BLX r0 时，r0 = dispatch 地址，BLX 跳到 dispatch 地址，不触发
     hook_table。

     所以上面的方案需要调整：

     修正方案

     不在 hook_table 层拦截（因为 dispatch 已经绕过了 hook）。改为：

     在 hook_table case 25（_mr_c_function_new）中，当嵌套 ext 覆盖 table[31/32] 时，将覆盖的 dispatch
     地址保存到模块状态中，并将 table[31/32] 恢复为 hook 地址。在 hook_table case 31/32 中检查是否有保存的 dispatch
     地址，若有则以正确的 R9 执行 dispatch。

     最终方案

     修改文件：src/arm_ext_executor.c

     Step 1: 添加 dispatch 地址存储字段

     在 ArmExtModule 结构体中添加：
     uint32_t dispatch_timer_start;  // 嵌套 ext 的 timerStart dispatch 函数地址
     uint32_t dispatch_timer_stop;   // 嵌套 ext 的 timerStop dispatch 函数地址

     Step 2: 在 table[25]（nested）中捕获 dispatch 地址

     game.ext 的 mr_c_function_load 写入 table[31/32] 后，hook_table 无法拦截（因为是直接内存写入）。但我们可以在
     table[25]（_mr_c_function_new）nested 分支执行后，检查 table[31/32] 是否被修改：

     在 table[25] nested 处理结束后：
     if (nested) {
         // ... existing nested setup code ...

         // 检查嵌套 ext 是否覆盖了 table[31/32]
         uint32_t t31_val = 0, t32_val = 0;
         memcpy(&t31_val, arm_ptr(m, EXT_TABLE_ADDR + 31*4), 4);
         memcpy(&t32_val, arm_ptr(m, EXT_TABLE_ADDR + 32*4), 4);
         if (t31_val != EXT_TABLE_ADDR + 31*4) {
             m->dispatch_timer_start = t31_val;
             // 恢复为 hook 地址
             uint32_t hook = EXT_TABLE_ADDR + 31*4;
             memcpy(arm_ptr(m, EXT_TABLE_ADDR + 31*4), &hook, 4);
         }
         if (t32_val != EXT_TABLE_ADDR + 32*4) {
             m->dispatch_timer_stop = t32_val;
             uint32_t hook = EXT_TABLE_ADDR + 32*4;
             memcpy(arm_ptr(m, EXT_TABLE_ADDR + 32*4), &hook, 4);
         }
     }

     但问题：table[31/32] 不是在 table[25] 中被覆盖的，而是在 game.ext 的 mr_c_function_load 函数中通过 STR r1, [r0, #124]
     直接写入 ARM 内存。这发生在 table[25] 调用之后。所以在 table[25] 中检查太早了。

     更好的时机：在 arm_ext_load 的 run_arm 返回后（即 wrapper 的 mr_c_function_load 执行完毕后），或者在 arm_ext_call 的
     wrapper code=0 执行完毕后，检查 table[31/32] 并捕获。

     Step 3: 在适当时机捕获 dispatch 地址

     在 arm_ext_call 中，wrapper 的 code=0 执行后（嵌套 ext 已加载），检查 table[31/32]：

     // 在 run_arm_with_sp 返回后
     if (call_ret == MR_SUCCESS && m->primary_helper_addr && !m->dispatch_timer_start) {
         uint32_t t31_val = 0, t32_val = 0;
         memcpy(&t31_val, arm_ptr(m, EXT_TABLE_ADDR + 31*4), 4);
         memcpy(&t32_val, arm_ptr(m, EXT_TABLE_ADDR + 32*4), 4);
         if (t31_val != EXT_TABLE_ADDR + 31*4) {
             m->dispatch_timer_start = t31_val;
             uint32_t hook = EXT_TABLE_ADDR + 31*4;
             memcpy(arm_ptr(m, EXT_TABLE_ADDR + 31*4), &hook, 4);
         }
         if (t32_val != EXT_TABLE_ADDR + 32*4) {
             m->dispatch_timer_stop = t32_val;
             uint32_t hook = EXT_TABLE_ADDR + 32*4;
             memcpy(arm_ptr(m, EXT_TABLE_ADDR + 32*4), &hook, 4);
         }
     }

     Step 4: 在 hook_table case 31/32 中使用 dispatch

     case 31: {
         if (m->dispatch_timer_start && m->p_addr) {
             uint32_t saved_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
             uint32_t wrapper_rw = 0;
             memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
             if (wrapper_rw) reg_write32(m->uc, UC_ARM_REG_R9, wrapper_rw);
             // 传递原始参数并调用 dispatch
             int call_ok = run_arm(m, m->dispatch_timer_start);
             reg_write32(m->uc, UC_ARM_REG_R9, saved_r9);
             ret = call_ok == MR_SUCCESS ? reg_read32(m->uc, UC_ARM_REG_R0) : MR_FAILED;
         } else {
             ret = mr_timerStart((uint16)r0);
             internal_slot_write(m, m->mr_timer_state_slot, 1);
             mr_timer_state = 1;
         }
     } break;

     Step 5: 移除 arm_ext_call 中的 guard 机制

     不再需要 guard/saved_t31/saved_t32。table[31/32] 始终是 hook 地址，dispatch 通过 hook_table 间接执行。

     Step 6: 清理

     - 移除 init_guard 字段和 arm_ext_set_init_guard
     - 移除 arm_ext_primary_helper（若不再需要）
     - 清理 mythroad.c 中 case 801 的 arm_ext_set_init_guard 调用

     验证

     1. build/vmrp mythroad/mrpinfo.mrp 不黑屏，正常显示
     2. 点击右下角退出按钮(219,306)能触发退出
     3. build/vmrp mythroad/xmwq.mrp 不受影响，正常运行
     4. 其他 mrp（dsm_gm.mrp 等）不回退