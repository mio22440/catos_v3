
#ifndef CAT_TASK_TYPES_H
#define CAT_TASK_TYPES_H

#include "cat_list.h"

/** 宏定义 */
/* 调度策略 */
#define CAT_TASK_STRATEGY_STATIC_PRIO  0               /**< 固定优先级调度 */
#define CAT_TASK_STRATEGY_EDF          1               /**< 最早截止时间优先调度 */

/* 任务状态 */
#define CATOS_TASK_STATE_RDY        0               //目前不具备判断功能
#define CATOS_TASK_STATE_DESTROYED  (1 << 1)        //删除
#define CATOS_TASK_STATE_DELAYED    (1 << 2)        //延时
#define CATOS_TASK_STATE_SUSPEND    (1 << 3)        //挂起
#define CATOS_TASK_EVENT_MASK       (0xff << 16)    //高16位用作事件相关的状态

/** 数据结构定义 */
/* 任务控制块 */
typedef struct _cat_task_t cat_task_t;
struct _cat_task_t
{
    void               *sp;                             /**< 栈顶(堆栈指针)*/
    cat_uint8_t        *task_name;                      /**< 任务名称*/
    cat_uint8_t         sched_strategy;                 /**< 调度策略 */
    cat_uint32_t        state;                          /**< 当前状态*/

    void               *entry;                          /**< 入口函数 */
    void               *arg;                            /**< 入口函数的参数 */
    void               *stack_start_addr;               /**< 堆栈起始地址*/
    cat_uint32_t        stack_size;                     /**< 堆栈大小*/

    void               *private_data;                   /**< 任务私有数据(调度相关) */

    struct _cat_node_t  manage_node;                    /**< 用于管理的链表节点 */
};

extern cat_task_t *cat_current_task;

/**
 * @brief 获取当前任务
 * 
 */
#define CAT_GET_CURRENT_TASK() \
    (cat_current_task)

/**
 * @brief 设置当前任务
 * 
 */
#define CAT_SET_CURRENT_TASK(_task) \
    do{cat_current_task = _task;}while(0)

#endif
