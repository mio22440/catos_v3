/**
 * @file cat_scheduler.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 调度器源文件
 * @version 0.1
 * @date 2023-07-01
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-07-01 <td>创建
 * </table>
 */
#include "catos_config.h"
#include "catos_types.h"

#include "cat_scheduler.h"
#include "port.h"
#include "cat_error.h"

static cat_list_t cat_scheduler_list;
static cat_uint8_t cat_scheduler_list_is_inited = 0;

static cat_uint8_t  cat_scheduler_sched_lock;

void print_scheduler(void)
{
    cat_scheduler_t *scheduler = NULL;
    cat_node_t *p = NULL;
    cat_uint32_t i = 0;

    CAT_LIST_FOREACH(&cat_scheduler_list, p)
    {
        scheduler = CAT_LIST_ENTRY(p, cat_scheduler_t, link_node);
        printf("%d: name=%s, prio=%u\r\n", i, scheduler->scheduler_name, scheduler->scheduler_prio);
        i++;
    }
}

/**
 * @brief 根据策略获取调度器
 * 
 * @param  strategy         调度策略
 * @return cat_scheduler_t* 调度器结构体指针
 */
static inline cat_scheduler_t *cat_scheduler_get_by_strategy(cat_uint8_t strategy)
{
    cat_scheduler_t *ret = NULL;

    cat_scheduler_t *scheduler = NULL;
    cat_node_t *p = NULL;

    CAT_LIST_FOREACH(&cat_scheduler_list, p)
    {
        scheduler = CAT_LIST_ENTRY(p, cat_scheduler_t, link_node);
        if(strategy == scheduler->strategy)
        {
            ret = scheduler;
            break;
        }
    }

    return ret;
}

/**
 * @brief 初始化所有调度器
 * 
 */
static inline void cat_scheduler_init_all_scheduler(void)
{
    cat_scheduler_t *scheduler = NULL;
    cat_node_t *p = NULL;

    CAT_LIST_FOREACH(&cat_scheduler_list, p)
    {
        scheduler = CAT_LIST_ENTRY(p, cat_scheduler_t, link_node);
        CAT_ASSERT(NULL != scheduler);

        if(NULL != scheduler->scheduler_init)
        {
            scheduler->scheduler_init();
        }
    }
}


void cat_scheduler_pre_init(void)
{
    CAT_ASSERT(0 == cat_scheduler_list_is_inited);

    /* 初始化管理调度器的链表 */
    cat_list_init(&cat_scheduler_list);

    /* 初始化调度锁变量 */
    cat_scheduler_sched_lock = 0;

    /* 设置已经初始化 */
    cat_scheduler_list_is_inited = 1;
}

void cat_scheduler_init(void)
{
    cat_scheduler_init_all_scheduler();
}

void cat_scheduler_sched_disable(void)
{
    cat_uint32_t status = cat_hw_irq_disable();

    if(cat_scheduler_sched_lock < 255)
    {
        cat_scheduler_sched_lock++;
    }

    cat_hw_irq_enable(status);
}

void cat_scheduler_sched_enable(void)
{
    cat_uint32_t status = cat_hw_irq_disable();

    if(cat_scheduler_sched_lock > 0)
    {
        cat_scheduler_sched_lock--;
    }

    cat_hw_irq_enable(status);

    if(0 == cat_scheduler_sched_lock)
    {
        cat_scheduler_schedule();
    }
}

void cat_scheduler_sched_enable_not_sched(void)
{
    cat_uint32_t status = cat_hw_irq_disable();

    if(cat_scheduler_sched_lock > 0)
    {
        cat_scheduler_sched_lock--;
    }

    cat_hw_irq_enable(status);
}

cat_task_t *cat_scheduler_get_highest_rdy_task(void)
{
    cat_task_t *ret = NULL;

    /* 寻找有就绪任务的最高优先级调度器 */
    cat_scheduler_t *highest_scheduler = CAT_LIST_ENTRY(&(cat_scheduler_list.head_node.next_node), cat_scheduler_t, link_node);
    CAT_ASSERT(NULL != highest_scheduler);

    cat_uint8_t has_rdy = 0; /* 是否有已就绪任务 */

    cat_scheduler_t *scheduler = NULL;
    cat_node_t *p = NULL;

    CAT_LIST_FOREACH(&cat_scheduler_list, p)
    {
        scheduler = CAT_LIST_ENTRY(p, cat_scheduler_t, link_node);

        CAT_ASSERT(NULL != scheduler);
        CAT_ASSERT(NULL != scheduler->has_task_rdy);
        has_rdy = scheduler->has_task_rdy();

        if(
            (1 == has_rdy) &&
            (scheduler->scheduler_prio > highest_scheduler->scheduler_prio)
        )
        {
            highest_scheduler = scheduler;
        }
    }

    /* 获取该调度器最高优先级任务 */
    ret = highest_scheduler->get_highest_rdy();
    CAT_ASSERT(NULL != ret);

    return ret;
}

void cat_scheduler_register(cat_scheduler_t *scheduler)
{
    cat_scheduler_t *exist = NULL;

    /* 确保已经初始化 */
    CAT_ASSERT(1 == cat_scheduler_list_is_inited);

    /* 查找是不是已经有该调度策略的调度器存在 */
    exist = cat_scheduler_get_by_strategy(scheduler->strategy);
    CAT_ASSERT(NULL == exist);

    /* 将调度器挂到管理链表上 */
    cat_list_add_last(&cat_scheduler_list, &(scheduler->link_node));
}

void cat_scheduler_create_task_static(cat_task_t *task, cat_uint8_t strategy, void *task_config)
{
    CAT_ASSERT(NULL != task_config);

    cat_scheduler_t *scheduler = cat_scheduler_get_by_strategy(strategy);

    CAT_ASSERT(NULL != scheduler);
    CAT_ASSERT(NULL != scheduler->task_create_static);

    scheduler->task_create_static(task, task_config);
}

void cat_scheduler_deal_in_tick(void)
{
    cat_scheduler_t *scheduler = NULL;
    cat_node_t *p = NULL;

    // CAT_DEBUG_PRINTF("cur=%s\r\n", CAT_GET_CURRENT_TASK()->task_name);

    CAT_LIST_FOREACH(&cat_scheduler_list, p)
    {
        scheduler = CAT_LIST_ENTRY(p, cat_scheduler_t, link_node);
        CAT_ASSERT(NULL != scheduler);

        if(NULL != scheduler->deal_in_tick)
        {
            scheduler->deal_in_tick();
        }
    }
}


void cat_scheduler_schedule(void)
{
    cat_uint8_t highest_prio = 0xff, has_rdy = 0;

    cat_scheduler_t *scheduler = NULL, *highest_prio_scheduler = NULL;
    cat_node_t *p = NULL;

    cat_uint32_t status = cat_hw_irq_disable();
    if(cat_scheduler_sched_lock == 0)
    {
        CAT_LIST_FOREACH(&cat_scheduler_list, p)
        {
            scheduler = CAT_LIST_ENTRY(p, cat_scheduler_t, link_node);
            CAT_ASSERT(NULL != scheduler);
            CAT_ASSERT(NULL != scheduler->has_task_rdy);

            has_rdy = scheduler->has_task_rdy();

            if(
                (scheduler->scheduler_prio < highest_prio) &&
                (0 != has_rdy)
            )
            {
                highest_prio_scheduler = scheduler;
            }
        }

        CAT_ASSERT(NULL != highest_prio_scheduler);
        CAT_ASSERT(NULL != highest_prio_scheduler->schedule);

        cat_hw_irq_enable(status);
        highest_prio_scheduler->schedule();

        // /* 不会到达这里 */
        // while(1);
    }
    else{
        /* 上锁就返回 */
        cat_hw_irq_enable(status);
    } 
}


void cat_scheduler_task_rdy(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_scheduler_t *scheduler = cat_scheduler_get_by_strategy(task->sched_strategy);

    CAT_ASSERT(NULL != scheduler);
    CAT_ASSERT(NULL != scheduler->task_rdy);

    scheduler->task_rdy(task);
}

void cat_scheduler_task_unrdy(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_scheduler_t *scheduler = cat_scheduler_get_by_strategy(task->sched_strategy);

    CAT_ASSERT(NULL != scheduler);
    CAT_ASSERT(NULL != scheduler->task_unrdy);

    scheduler->task_unrdy(task);
}

void cat_scheduler_task_delay(cat_task_t *task, cat_uint32_t tick)
{
    CAT_ASSERT(task);

    cat_scheduler_t *scheduler = cat_scheduler_get_by_strategy(task->sched_strategy);

    CAT_ASSERT(NULL != scheduler);
    CAT_ASSERT(NULL != scheduler->task_delay);

    /* 不要求 tick 大于零了，这样可以通过 delay(0) 来放弃cpu */
    scheduler->task_delay(task, tick);
}

void cat_scheduler_task_delay_wakeup(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_scheduler_t *scheduler = cat_scheduler_get_by_strategy(task->sched_strategy);

    CAT_ASSERT(NULL != scheduler);
    CAT_ASSERT(NULL != scheduler->task_delay_wakeup);

    scheduler->task_delay_wakeup(task);
}

void cat_scheduler_task_suspend(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_scheduler_t *scheduler = cat_scheduler_get_by_strategy(task->sched_strategy);

    CAT_ASSERT(NULL != scheduler);
    CAT_ASSERT(NULL != scheduler->task_suspend);

    scheduler->task_suspend(task);
}

void cat_scheduler_task_suspend_wakeup(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_scheduler_t *scheduler = cat_scheduler_get_by_strategy(task->sched_strategy);

    CAT_ASSERT(NULL != scheduler);
    CAT_ASSERT(NULL != scheduler->task_suspend_wakeup);

    scheduler->task_suspend_wakeup(task);
}
