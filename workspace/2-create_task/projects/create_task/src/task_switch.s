
    .cpu cortex-m3
    .syntax unified
    .thumb
    .text

    .extern  cat_current_task
    .extern  cat_next_task

    .equ NVIC_INT_CTRL,   0xE000ED04
    .equ NVIC_PENDSVSET,  0x10000000
    .equ NVIC_SYSPRI2,    0xE000ED22
    .equ NVIC_PENDSV_PRI, 0x000000FF

/**
 * void cat_context_switch(void)
 * 触发pendsv中断进行任务切换(pendsv的优先级在开始第一个任务时已经设置)
 */
    .global cat_context_switch
    .type cat_context_switch, %function
cat_context_switch:
    ldr r0, =NVIC_INT_CTRL      
    ldr r1, =NVIC_PENDSVSET
    str r1, [r0]                /* *(NVIC_INT_CTRL) = NVIC_PENDSVSET */
    bx  lr

    .global  PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:

    ldr r0, =cat_current_task
    ldr r1, [r0]
    cbz r1, PendSV_Handler_nosave

    mrs r0, psp
    stmfd r0!, {r4-r11}             /* 保存 r4-r11（减前/递减满堆栈）*/
    
    ldr r1, =cat_current_task       /* 记录最后的指针到任务栈curstk->stack */
    ldr r1, [r1]
    str r0, [r1]
		
PendSV_Handler_nosave:               
    ldr r0, =cat_current_task       /* curtask = rdytask */
    ldr r1, =cat_next_task         
    ldr r2, [r1]
    str r2, [r0]

    ldr r0, [r2]                    /* 恢复新任务的寄存器 */
    ldmfd r0!, {r4-r11}

    msr psp, r0                     /* 从r0恢复下一任务的堆栈指针 */
    orr lr, lr, #0x04               /* 设置退出异常后使用psp堆栈 */
    bx lr                           /* 退出异常，自动从堆栈恢复部分寄存器 */

    .end
