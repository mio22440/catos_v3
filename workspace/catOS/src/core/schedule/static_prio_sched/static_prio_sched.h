/**
 * @file static_prio_sched.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 固定优先级任务头文件
 * @version 0.1
 * @date 2023-07-04
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-07-04 <td>内容
 * </table>
 */

#ifndef STATIC_PRIO_SCHED_H
#define STATIC_PRIO_SCHED_H

#include "catos_types.h"
#include "catos_config.h"

#include "cat_task_types.h"

typedef struct _static_prio_private_data_t static_prio_private_data_t;
struct _static_prio_private_data_t
{
    struct _cat_node_t  link_node;                      /**< 任务表中的链表节点，也用于delay链表*/
    cat_uint32_t        delay;                          /**< 延时剩余tick数*/

    cat_uint8_t         prio;                           /**< 优先级*/
    cat_uint32_t        slice;                          /**< 时间片(剩余时间)*/

    cat_uint32_t        suspend_cnt;                    /**< 被挂起的层数*/
    cat_uint32_t        sched_times;                    /**< 调度次数*/

    cat_task_t          *owner_task;                    /**< 拥有该数据的任务结构体指针 */
};

typedef struct _static_prio_task_config_t static_prio_task_config_t;
struct _static_prio_task_config_t
{
    cat_uint8_t                       prio;
    static_prio_private_data_t       *private_data;
};

/**
 * @brief 注册固定优先级调度器
 * 
 */
void static_prio_scheduler_register(void);

#endif


