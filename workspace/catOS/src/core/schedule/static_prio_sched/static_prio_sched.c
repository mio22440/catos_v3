/**
 * @file static_prio_sched.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 固定优先级源文件
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
#include "port.h"
#include "cat_list.h"
#include "cat_bitmap.h"
#include "cat_string.h"

#include "static_prio_sched.h"
#include "cat_scheduler.h"

#include "cat_stdio.h"
#include "cat_error.h"


/* PUBLIC */
static cat_bitmap cat_static_prio_task_bitmap;     /**< 就绪位图 */

/* PRIVATE */
static struct _cat_list_t cat_static_prio_rdy_tbl[CATOS_MAX_TASK_PRIO];    /**< 就绪表 */
static struct _cat_list_t cat_static_prio_delay_list;                /**< 延时链表 */

/******************************************************/

static void _default_sp_task_exit(void)
{
    while(1);
}

static void static_prio_scheduler_init(void);
static void static_prio_scheduler_task_create_static(cat_task_t *task, void *task_config);
static void static_prio_scheduler_deal_in_tick(void);
static cat_uint8_t static_prio_scheduler_has_task_rdy(void);
static void static_prio_scheduler_sched(void);

static void static_prio_scheduler_task_rdy(cat_task_t *task);
static void static_prio_scheduler_task_unrdy(cat_task_t *task);
static void static_prio_scheduler_task_delay(cat_task_t *task, cat_uint32_t tick);
static void static_prio_scheduler_task_delay_wakeup(cat_task_t *task);
static void static_prio_scheduler_task_suspend(cat_task_t *task);
static void static_prio_scheduler_task_suspend_wakeup(cat_task_t *task);

/**
 * @brief 获取最高优先级任务
 * 
 * @return cat_task_t* NULL:无就绪任务; !NULL:就绪任务指针
 */
static inline cat_task_t *_static_prio_get_highest_ready(void)
{
    cat_task_t *ret;
    static_prio_private_data_t *pdata = NULL;

    /* 获取最低非零位(有任务就绪的最高优先级) */
    cat_uint32_t highest_prio = cat_bitmap_get_first_set(&cat_static_prio_task_bitmap);

    /* 获取链表的第一个节点 */
    struct _cat_node_t *node = cat_list_first(&(cat_static_prio_rdy_tbl[highest_prio]));

    /* 获取任务结构指针 */
    pdata = CAT_GET_CONTAINER(node, static_prio_private_data_t, link_node);
    ret = pdata->owner_task;

    return ret;
}

static void static_prio_scheduler_init(void)
{
    /* 初始化位图 */
    cat_bitmap_init(&cat_static_prio_task_bitmap);

    /* 初始化等待链表 */
    cat_list_init(&cat_static_prio_delay_list);

    /* 初始化就绪表 */
    int i;
    for(i=0; i < CATOS_MAX_TASK_PRIO; i++)
    {
        cat_list_init(&(cat_static_prio_rdy_tbl[i]));
    }

    if(cat_stdio_is_device_is_set())
    {
        CAT_KPRINTF("[static_prio_sched] static priority scheduler init\r\n");
    }
}
static void static_prio_scheduler_task_create_static(cat_task_t *task, void *task_config)
{
    static_prio_task_config_t *config = (static_prio_task_config_t *)task_config;
    static_prio_private_data_t *private_data = (static_prio_private_data_t *)(config->private_data);

    /* 在该函数被调用前, tcb 已经被初始化, 如果不需要更改默认tcb值则只需初始化私有数据即可 */
    /**
     * 若需要可以更改默认tcb值
     * 
     * 例如:
     * task->state = CATOS_TASK_STATE_DELAYED;
     */

    /* 初始化私有数据 */
    cat_list_node_init(&(private_data->link_node));

    private_data->delay = 0;

    private_data->prio = config->prio;
    private_data->slice = CATOS_MAX_SLICE;
    private_data->suspend_cnt = 0;

    private_data->sched_times = 0;

    private_data->owner_task = task;

    /* 将私有数据放进 tcb 中 */
    task->private_data = private_data;

    /* 放入就绪表 */
    static_prio_scheduler_task_rdy(task);
}
static void static_prio_scheduler_deal_in_tick(void)
{
    struct _cat_node_t *node, *next_node;

    static_prio_private_data_t *pdata = NULL;
    cat_task_t *task = NULL;

    /* 处理延时队列 */
    /* 取得等待链表第一个节点 */
    node = cat_static_prio_delay_list.head_node.next_node;
    /* 遍历等待链表 */
    while(node != &(cat_static_prio_delay_list.head_node))
    {
        /* 保存下一个节点，否则在任务控制块中只有一个链表节点的情况下进行wakeup操作后
         * node的下一个节点会被改变为就绪链表的下一个节点 */
        next_node = node->next_node;
        
        /* 取得任务控制块指针 */
        pdata = CAT_GET_CONTAINER(node, static_prio_private_data_t, link_node);
        task = pdata->owner_task;
        
        CAT_ASSERT(NULL != task);

        pdata->delay--;
        if(pdata->delay == 0)
        {
            static_prio_scheduler_task_delay_wakeup(task);/* 从延时表取出并挂到就绪表中 */
        }

        /* 在下一次循环前挪动当前指针到先前保存的next_node位置 */
        node = next_node;
    }

    /* 处理时间片 */
    task = CAT_GET_CURRENT_TASK();
    if(CAT_TASK_STRATEGY_STATIC_PRIO == task->sched_strategy)
    {
        pdata = task->private_data;
        pdata->slice--;
        if(pdata->slice == 0)
        {
            if(cat_list_count(&(cat_static_prio_rdy_tbl[pdata->prio])))
            {
                /* 将当前任务节点从任务链表首位去掉 */
                cat_list_remove_first(&(cat_static_prio_rdy_tbl[pdata->prio]));
                /* 将当前任务节点加到任务链表末尾 */
                cat_list_add_last(&(cat_static_prio_rdy_tbl[pdata->prio]), &(pdata->link_node));

                /* 重置时间片 */
                pdata->slice = CATOS_MAX_SLICE;
            }
        }
    }
}
static cat_uint8_t static_prio_scheduler_has_task_rdy(void)
{
    /* 有 idle 任务保底, 必有就绪任务 */
    return 1;
}
static cat_task_t *static_prio_scheduler_get_highest_rdy(void)
{
    cat_task_t *ret;

    /* 获取最低非零位(有任务就绪的最高优先级) */
    cat_uint32_t highest_prio = cat_bitmap_get_first_set(&cat_static_prio_task_bitmap);

    /* 获取链表的第一个节点 */
    struct _cat_node_t *node = cat_list_first(&(cat_static_prio_rdy_tbl[highest_prio]));

    /* 获取任务结构指针 */
    static_prio_private_data_t *pdata = CAT_GET_CONTAINER(node, static_prio_private_data_t, link_node);
    ret = pdata->owner_task;

    return ret;
}
static void static_prio_scheduler_sched(void)
{
    cat_task_t *from_task, *to_task, *cur_task;
    static_prio_private_data_t *pdata = NULL;

    cat_uint32_t status = cat_hw_irq_disable();

    to_task  = _static_prio_get_highest_ready(); /* 获取最高固定优先级任务 */
    cur_task = CAT_GET_CURRENT_TASK();           /* 获取当前任务 */

    CAT_ASSERT(NULL != to_task);                 /* 有 idle 任务在, 如果没有就绪任务说明有问题 */

    if(to_task != cur_task)
    {
        from_task = cur_task;
#if 0
        /* 我是傻逼, 这里的 cur_task 又不是系统的全局当前任务指针, 设置了也没用 */
        cur_task = to_task;
#else
        CAT_SET_CURRENT_TASK(to_task);
#endif


        /* 增加调度次数信息 */
        ((static_prio_private_data_t *)(to_task->private_data))->sched_times++;

        // CAT_DEBUG_PRINTF("[sp_sched]%s->%s\r\n", from_task->task_name, to_task->task_name);

        /* 切换上下文 */
        cat_hw_context_switch(
            (cat_uint32_t)&(from_task->sp),
            (cat_uint32_t)&(to_task->sp)
        );
    }
    // else
    // {
    //     CAT_DEBUG_PRINTF("[sp_sched]%s\r\n", cur_task->task_name);
    // }

    cat_hw_irq_enable(status);
}

static void static_prio_scheduler_task_rdy(cat_task_t *task)
{
    CAT_ASSERT(NULL != task);
    static_prio_private_data_t *pdata = (static_prio_private_data_t *)task->private_data;

    cat_uint32_t status = cat_hw_irq_disable();

    /* 添加到相应优先级的就绪队列末尾 */
    cat_list_add_last(&(cat_static_prio_rdy_tbl[pdata->prio]), &(pdata->link_node));

    /* 设置位图 */
    cat_bitmap_set(&cat_static_prio_task_bitmap, pdata->prio);

    cat_hw_irq_enable(status);
}
static void static_prio_scheduler_task_unrdy(cat_task_t *task)
{
    CAT_ASSERT(NULL != task);
    static_prio_private_data_t *pdata = (static_prio_private_data_t *)task->private_data;

    cat_uint32_t status = cat_hw_irq_disable();

    /* 从相应优先级就绪队列删除 */
    cat_list_remove_node(&(cat_static_prio_rdy_tbl[pdata->prio]), &(pdata->link_node));
    if(cat_list_count(&(cat_static_prio_rdy_tbl[pdata->prio])) == 0)/* 如果没有任务才清除就绪位 */
    {
        cat_bitmap_clr(&cat_static_prio_task_bitmap, pdata->prio);
    }
    cat_hw_irq_enable(status);
}
static void static_prio_scheduler_task_delay(cat_task_t *task, cat_uint32_t tick)
{
    CAT_ASSERT(NULL != task);
    static_prio_private_data_t *pdata = (static_prio_private_data_t *)task->private_data;

    cat_uint32_t status = cat_hw_irq_disable();

    /* 设置等待的 tick 数 */
    pdata->delay = tick;

    /* 从就绪队列取出 */
    static_prio_scheduler_task_unrdy(task);

    /* 放入等待队列末尾 */
    cat_list_add_last(&cat_static_prio_delay_list, &(pdata->link_node));

    /* 置位等待状态 */
    task->state |= CATOS_TASK_STATE_DELAYED;

    cat_hw_irq_enable(status);

    /* 进行一次调度 */
    cat_scheduler_schedule();
}
static void static_prio_scheduler_task_delay_wakeup(cat_task_t *task)
{
    CAT_ASSERT(NULL != task);
    static_prio_private_data_t *pdata = (static_prio_private_data_t *)task->private_data;

    cat_uint32_t status = cat_hw_irq_disable();

    /* 从等待链表取出 */
    cat_list_remove_node(&cat_static_prio_delay_list, &(pdata->link_node));

    /* 复位等待状态位 */
    task->state &= ~CATOS_TASK_STATE_DELAYED;

    /* 将任务就绪 */
    static_prio_scheduler_task_rdy(task);

    cat_hw_irq_enable(status);
}
static void static_prio_scheduler_task_suspend(cat_task_t *task)
{
    CAT_ASSERT(NULL != task);
    static_prio_private_data_t *pdata = (static_prio_private_data_t *)task->private_data;

    cat_uint32_t status = cat_hw_irq_disable();

    /* 只有不在延时状态时可以被挂起 */
    if(!(task->state & CATOS_TASK_STATE_DELAYED))
    {
        /* 增加被阻塞次数 */
        pdata->suspend_cnt++;
        if(pdata->suspend_cnt <= 1)
        {                      
            /* 若当前未被阻塞，则阻塞(是否可用后缀简化？) */
            /* 置位阻塞状态位 */
            task->state |= CATOS_TASK_STATE_SUSPEND;

            /* 从就绪表取出 */
            static_prio_scheduler_task_unrdy(task);
        }
    }

    cat_hw_irq_enable(status);

    /* 如果被阻塞的是当前任务，则需要执行下一个任务，即进行一次调度 */
    if(task == CAT_GET_CURRENT_TASK())
    {
        cat_scheduler_schedule();
    }
}
static void static_prio_scheduler_task_suspend_wakeup(cat_task_t *task)
{
    CAT_ASSERT(NULL != task);
    static_prio_private_data_t *pdata = (static_prio_private_data_t *)task->private_data;

    cat_uint32_t status = cat_hw_irq_disable();

    /* 只有已经被挂起至少一次的任务才能被唤醒 */
    if(task->state & CATOS_TASK_STATE_SUSPEND)
    {
        /* 减少被阻塞层数 */
        pdata->suspend_cnt--;
        if(pdata->suspend_cnt == 0)
        {
            /* 若被阻塞层数为零，则退出阻塞状态 */
            task->state &= ~(CATOS_TASK_STATE_SUSPEND);

            /* 挂到就绪表 */
            static_prio_scheduler_task_rdy(task);

            cat_hw_irq_enable(status);

            /* 进行一次调度 */
            cat_scheduler_schedule();
        }
    }

    cat_hw_irq_enable(status);
}

cat_scheduler_t static_prio_scheduler  = {
    .strategy               = CAT_TASK_STRATEGY_STATIC_PRIO,          
    .scheduler_prio         = CAT_SCHEDULER_PRIO_STATIC_PRIO,     

    .scheduler_name         = "static_prio",
     
    .scheduler_init         = static_prio_scheduler_init,
    .task_create_static     = static_prio_scheduler_task_create_static,
    .deal_in_tick           = static_prio_scheduler_deal_in_tick,
    .has_task_rdy           = static_prio_scheduler_has_task_rdy,
    .get_highest_rdy        = static_prio_scheduler_get_highest_rdy,
    .schedule               = static_prio_scheduler_sched,

    .task_rdy               = static_prio_scheduler_task_rdy,
    .task_unrdy             = static_prio_scheduler_task_unrdy,
    .task_delay             = static_prio_scheduler_task_delay,
    .task_delay_wakeup      = static_prio_scheduler_task_delay_wakeup,
    .task_suspend           = static_prio_scheduler_task_suspend,
    .task_suspend_wakeup    = static_prio_scheduler_task_suspend_wakeup,
};

void static_prio_scheduler_register(void)
{
    cat_scheduler_register(&static_prio_scheduler);
}
