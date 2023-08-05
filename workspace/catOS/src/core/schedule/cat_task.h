/**
 * @file cat_task.h
 * @author mio (648137125@qq.com)
 * @brief 任务控制的用户接口
 * @version 0.1
 * @date 2022-07-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CAT_TASK_H
#define CAT_TASK_H

#include "cat_task_types.h"

#include "static_prio_sched.h"

/**
 * @brief 初始化任务调度
 * 
 */
void cat_task_schedule_init(void);


/* 用户函数 */

/**
 * @brief 静态创建任务
 *
 * @param  task             任务结构体指针
 * @param  strategy         任务调度策略
 * @param  task_name        任务名称
 * @param  entry            任务函数入口
 * @param  arg              任务函数参数
 * @param  stack_start_addr 堆栈开始地址
 * @param  stack_size       堆栈大小
 * @param  task_config      任务设置结构体指针
 */
void cat_task_create_task_static(
    cat_task_t         *task, 
    cat_uint8_t         strategy, 
    const cat_uint8_t  *task_name,
    void              (*entry)(void *),
    void               *arg,
    void               *stack_start_addr,
    cat_uint32_t        stack_size,
    void                *task_config
);

/* 延时相关 */

/**
 * @brief 当前任务等待 tick 个时钟周期
 * 
 * @param  tick             要等待的 tick 数
 */
void cat_task_delay_tick(cat_uint32_t tick);
/**
 * @brief 将任务从等待队列取出并就绪
 * 
 * @param  task             任务结构体指针
 */
void cat_task_delay_wakeup(cat_task_t *task);

/* 挂起相关 */

/**
 * @brief 挂起当前任务
 * 
 * @param  task             任务结构体指针
 */
void cat_task_suspend_self(void);
/**
 * @brief 挂起任务
 * 
 * @param  task             任务结构体指针
 */
void cat_task_suspend(cat_task_t *task);
/**
 * @brief 将任务从挂起唤醒
 * 
 * @param  task             任务结构体指针
 */
void cat_task_suspend_wakeup(cat_task_t *task);


/* 调度锁相关 */

/**
 * @brief 禁止调度
 * 
 */
void cat_task_schedule_disable(void);

/**
 * @brief 允许调度且进行一次调度
 * 
 */
void cat_task_schedule_enable(void);

/**
 * @brief 允许调度且不进行调度
 * 
 */
void cat_task_schedule_enable_no_sched(void);

/* 临界区相关 */

/**
 * @brief 当前任务进入临界区
 * 
 */
void cat_task_enter_critical(void);

/**
 * @brief 当前任务出临界区
 * 
 */
void cat_task_exit_critical(void);


#endif
