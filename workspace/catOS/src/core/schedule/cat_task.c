/**
 * @file cat_task.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 任务相关
 * @version 0.1
 * @date 2023-06-30
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-30 <td>创建
 * </table>
 */
#include "port.h"
#include "cat_string.h"

#include "cat_task.h"
#include "cat_scheduler.h"

#include "static_prio_sched.h"

cat_task_t *cat_current_task;

struct _cat_list_t cat_task_manage_list; /**< 用于管理的任务链表 */

static void _cat_task_default_exit(void)
{
    while(1);
}

static inline void _cat_task_init_tcb(
    cat_task_t         *task,
    const cat_uint8_t  *task_name,
    cat_uint32_t        initial_state,
    void              (*entry)(void *),
    void               *arg,
    void               *stack_start_addr,
    cat_uint32_t        stack_size
)
{
    task->task_name = (cat_uint8_t *)(task_name);
    task->state     = initial_state;

    cat_memset(stack_start_addr, 0xff, stack_size);

    task->sp = (void *)cat_hw_stack_init(
        (void*)entry,
        (void*)arg,
        (cat_uint8_t *)((cat_uint32_t)stack_start_addr + stack_size - sizeof(cat_uint32_t)),
        (void *)_cat_task_default_exit
    );

    task->entry = (void *)entry;
    task->arg   = arg;

    /* 初始化栈 */
    task->stack_start_addr = stack_start_addr;
    task->stack_size = stack_size;
}

void cat_task_schedule_init(void)
{
    /* 初始化管理链表 */
    cat_list_init(&cat_task_manage_list);

    /* 初始化调度器step1: 初始化调度器管理 */
    cat_scheduler_pre_init();

    /* 初始化调度器step2: 注册所有调度器 */
    /* 注册固定优先级调度器 */
    static_prio_scheduler_register();

    /* 初始化调度器step3: 初始化所有调度器 */
    cat_scheduler_init();
}

void cat_task_create_task_static(
    cat_task_t         *task, 
    cat_uint8_t         strategy, 
    const cat_uint8_t  *task_name,
    void              (*entry)(void *),
    void               *arg,
    void               *stack_start_addr,
    cat_uint32_t        stack_size,
    void                *task_config
)
{
    /* 初始化任务控制块 */
    _cat_task_init_tcb(
        task,
        task_name,
        CATOS_TASK_STATE_RDY,
        entry,
        arg,
        stack_start_addr,
        stack_size
    );

    /* 根据调度策略进行任务创建 */
    cat_scheduler_create_task_static(task, strategy, task_config);

    /* 将任务放进管理链表 */
    cat_list_node_init(&(task->manage_node));
    cat_list_add_last(&cat_task_manage_list, &(task->manage_node));
}

/**
 * @brief 当前任务等待 tick 个时钟周期
 * 
 * @param  tick             要等待的 tick 数
 */
void cat_task_delay_tick(cat_uint32_t tick)
{
    cat_task_t *cur_task = CAT_GET_CURRENT_TASK();

    cat_scheduler_task_delay(cur_task, tick);
}
/**
 * @brief 将任务从等待队列取出并就绪
 * 
 * @param  task             任务结构体指针
 */
void cat_task_delay_wakeup(cat_task_t *task)
{
    cat_scheduler_task_delay_wakeup(task);
}

/* 挂起相关 */

/**
 * @brief 挂起当前任务
 * 
 * @param  task             任务结构体指针
 */
void cat_task_suspend_self(void)
{
    cat_task_t *cur_task = CAT_GET_CURRENT_TASK();

    cat_scheduler_task_suspend(cur_task);
}
/**
 * @brief 挂起任务
 * 
 * @param  task             任务结构体指针
 */
void cat_task_suspend(cat_task_t *task)
{
    cat_scheduler_task_suspend(task);
}
/**
 * @brief 将任务从挂起唤醒
 * 
 * @param  task             任务结构体指针
 */
void cat_task_suspend_wakeup(cat_task_t *task)
{
    cat_scheduler_task_suspend_wakeup(task);
}

/**
 * @brief 当前任务进入临界区
 * 
 */
void cat_task_enter_critical(void)
{
    cat_scheduler_sched_disable();
}

/**
 * @brief 当前任务出临界区
 * 
 */
void cat_task_exit_critical(void)
{
    cat_scheduler_sched_enable();
}


#if (CATOS_ENABLE_CAT_SHELL == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
#include "port.h"

struct _cat_task_info_t
{
    void               *sp;                             /**< 栈顶(堆栈指针)*/
    cat_uint8_t        *task_name;                      /**< 任务名称*/
    cat_uint8_t         sched_strategy;                 /**< 调度策略 */


    //void               *entry;                          /**< 入口函数 */
    //void               *arg;                            /**< 入口函数的参数 */
    void               *stack_start_addr;               /**< 堆栈起始地址*/
    cat_uint32_t        stack_size;                     /**< 堆栈大小*/

    //struct _cat_node_t  link_node;                      /**< 任务表中的链表节点，也用于delay链表*/
    cat_uint32_t        delay;                          /**< 延时剩余tick数*/

    cat_uint32_t        state;                          /**< 当前状态*/

    cat_uint8_t         prio;                           /**< priority of task*/
    cat_uint32_t        slice;                          /**< 时间片(剩余时间)*/
    cat_uint32_t        suspend_cnt;                    /**< 被挂起的次数*/

    cat_uint32_t        sched_times;                    /**< 调度次数*/

    //struct _cat_node_t *manage_node;                    /**< 用于管理的链表节点 */
};

#define CATOS_TASK_STATE_RDY        0               //目前不具备判断功能
#define CATOS_TASK_STATE_DESTROYED  (1 << 1)        //删除
#define CATOS_TASK_STATE_DELAYED    (1 << 2)        //延时
#define CATOS_TASK_STATE_SUSPEND    (1 << 3)        //挂起

static const cat_uint8_t strategy_name_map[][8] = 
{
    "s_prio", /* 固定优先级调度 */
};

static const cat_uint8_t state_name_map[][8] = 
{
    "ready",
    "destroy",
    "delayed",
    "suspend",
};

static inline cat_uint8_t *get_state_name(cat_uint8_t state)
{
    cat_uint8_t *ret = NULL;

    switch(state)
    {
        case CATOS_TASK_STATE_RDY:
        {
            ret = state_name_map[0];
            break;
        }
        case CATOS_TASK_STATE_DESTROYED:
        {
            ret = state_name_map[1];
            break;
        }
        case CATOS_TASK_STATE_DELAYED:
        {
            ret = state_name_map[2];
            break;
        }
        case CATOS_TASK_STATE_SUSPEND:
        {
            ret = state_name_map[3];
            break;
        }
        default:
        {
            CAT_KPRINTF("[cat_task] error! invalid state!\r\n");
            break;
        }
    }

    return ret;
}

void *do_ps(void *arg)
{
    (void)arg;

    cat_task_t *task = NULL;
    struct _cat_task_info_t info = {0};
    cat_node_t         *tmp  = NULL;
    cat_uint32_t       *p = NULL;

    CAT_KPRINTF("-----------------------------------------------------------------------------------------\r\n");
    CAT_KPRINTF("| task_name    | stragegy | state    | stk_sz | stk_top    | stk_use | stk_max |\r\n");
    CAT_KPRINTF("-----------------------------------------------------------------------------------------\r\n");

    CAT_LIST_FOREACH(&cat_task_manage_list, tmp)
    {
        /* 获取任务结构体指针 */
        task = CAT_GET_CONTAINER(tmp, cat_task_t, manage_node);

        /* 在临界区复制需要的值 */
        cat_uint32_t status     = cat_hw_irq_disable();
        info.task_name          = task->task_name;
        info.sched_strategy     = task->sched_strategy;
        info.state              = task->state;
        info.stack_start_addr   = task->stack_start_addr;
        info.stack_size         = task->stack_size;
        info.sp                 = task->sp;
        cat_hw_irq_enable(status);

        for(p=info.stack_start_addr + sizeof(cat_uint32_t); p<=(info.stack_start_addr + info.stack_size - sizeof(cat_uint32_t)); p++)
        {
            if(0xffffffff != *p)
            {
                break;
            }
        }

        CAT_KPRINTF(
            "| %12s | %8s | %8s | %6d | %8x | %6d | %7d |\r\n",
            info.task_name,
            strategy_name_map[info.sched_strategy],
            get_state_name(info.state),
            info.stack_size,
            info.sp,
            (100 - ((info.sp - info.stack_start_addr) * 100 / info.stack_size)),
            (100 - (((void *)p       - info.stack_start_addr) * 100 / info.stack_size))
        );
        //CAT_KPRINTF("------------------------------------------------------------------------------\r\n");
    }

    return NULL;
}
CAT_DECLARE_CMD(ps, print task, do_ps);
#endif