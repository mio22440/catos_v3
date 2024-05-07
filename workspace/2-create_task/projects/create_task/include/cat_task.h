/**
 * @file cat_task.h
 * @brief 
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2024-05-07
 * Change Logs:
 * Date           Author        Notes
 * 2024-05-07     mio     first verion
 * 
 */
#ifndef _CAT_TASK_H
#define _CAT_TASK_H

#include "cat_types.h"

typedef struct _cat_task_t
{
    void *stack_pointer;       /**< 任务栈顶指针 */
    void *stack_start_address; /**< 任务栈起始地址 */
    cat_ubase stack_size;      /**< 任务栈大小 */

    void (*task_entry)(void); /**< 任务入口函数地址 */
}cat_task_t;

extern cat_task_t *cat_current_task, *cat_next_task;

void cat_task_create(
    cat_task_t *task,
    void *stack_start_address,
    cat_ubase stack_size,
    void (*task_entry)(void)
);

void cat_context_switch(void);

void start_task(cat_task_t *task);

#endif
