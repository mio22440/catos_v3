/**
 * @file cat_task.c
 * @brief 任务相关
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2024-05-07
 * Change Logs:
 * Date           Author        Notes
 * 2024-05-07     mio     first verion
 * 
 */
#include "cat_task.h"
#include "stm32f1xx.h"

#define NVIC_INT_CTRL   0xE000ED04
#define NVIC_PENDSVSET  0x10000000
#define NVIC_SYSPRI2    0xE000ED22
#define NVIC_PENDSV_PRI 0x000000FF

#define MEM32(addr)     *(volatile uint32_t *)(addr)
#define MEM8(addr)      *(volatile uint8_t  *)(addr)

cat_task_t *cat_current_task = CAT_NULL, *cat_next_task = CAT_NULL;

void cat_task_create(
    cat_task_t *task,
    void *stack_start_address,
    cat_ubase stack_size,
    void (*task_entry)(void)
)
{
    cat_u32 *stack_top = CAT_NULL;

    /* 计算栈顶地址 */
    stack_top = (cat_u32 *)((cat_ubase)stack_start_address + stack_size - sizeof(cat_u32));

    /* 先加上4字节再8字节向下取整对齐(相当于四舍五入) */
    stack_top = (cat_u32 *)((cat_ubase)stack_top + sizeof(cat_u32));
    stack_top = (cat_u32 *)CAT_ALIGN_DOWN((cat_u32)stack_top, 8);

    /* 根据手册中中断行为以及寄存器说明，初始化栈数据 */
    /* pendsv自动保存的寄存器 */
    *(--stack_top) = (cat_u32)(1 << 24); /* spsr */
    *(--stack_top) = (cat_u32)task_entry; /* pc */
    *(--stack_top) = (cat_u32)0x14; /* lr(r14) */
    *(--stack_top) = (cat_u32)0x12;
    *(--stack_top) = (cat_u32)0x3;
    *(--stack_top) = (cat_u32)0x2;
    *(--stack_top) = (cat_u32)0x1;
    *(--stack_top) = (cat_u32)CAT_NULL; /* r0 */

    /* 需要手动保存的寄存器 */
    *(--stack_top) = (cat_u32)0x11;
    *(--stack_top) = (cat_u32)0x10;
    *(--stack_top) = (cat_u32)0x9;
    *(--stack_top) = (cat_u32)0x8;
    *(--stack_top) = (cat_u32)0x7;
    *(--stack_top) = (cat_u32)0x6;
    *(--stack_top) = (cat_u32)0x5;
    *(--stack_top) = (cat_u32)0x4;

    /* 初始化任务结构 */
    task->stack_pointer = stack_top;
    task->stack_size = stack_size;
    task->stack_start_address = stack_start_address;
    task->task_entry = task_entry;
}

void start_task(cat_task_t *task)
{
    cat_next_task = task;

    MEM8(NVIC_SYSPRI2)      = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL)    = NVIC_PENDSVSET;
}
