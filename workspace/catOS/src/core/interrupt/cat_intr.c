/**
 * @file cat_intr.c
 * @author mio (648137125@qq.com)
 * @brief 中断管理
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "cat_task.h"
#include "cat_scheduler.h"
#include "../../../bsp/interface/port.h"

#include "../../component/cat_stdio/cat_stdio.h"

#include "cat_intr.h"

/** 系统时钟 START */
/* vars */
cat_uint32_t catos_systicks;    /**< 系统时钟数 */

/* funcs */
void cat_systick_init(void)
{
    catos_systicks = 0;
}

/** 系统时钟 END */

/**
 * @brief 系统时钟中断处理函数
 *        处理tick等
 */
void cat_intr_systemtick_handler(void)
{
    cat_uint32_t status = cat_hw_irq_disable();
    /* 处理等待的任务 */
    cat_scheduler_deal_in_tick();
    /* 系统tick数 */
    catos_systicks++;
    cat_hw_irq_enable(status);
	
    /* 进行一次调度 */
    cat_scheduler_schedule();

}

/**
 * @brief 默认中断服务函数
 * 
 */
void cat_intr_default_handler(cat_uint32_t ipsr_val)
{
#if 0
    cat_uint32_t irq_num = vector & 0x1f;
    cat_uint32_t  exti_pr_reg = *((cat_uint32_t *)(0x40010414));
    CAT_KPRINTF("cat_intr_default_handler triggered, vec=%d, irq=%d, exti_pr=%d\r\n", vector, irq_num, exti_pr_reg);
#endif
    /* 减去不可编程的向量数得到向量号 */
    cat_uint32_t vector = ipsr_val - 16;

    (void)vector;

    while(1);
}

