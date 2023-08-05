/**
 * @file cat_idle.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-29
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-29 1.0    amoigus             内容
 */

#include "cat_idle.h"
#include "cat_task.h"
#include "cat_stdio.h"


/* var decl */
cat_task_t *cat_idle_task;                      /**< 空闲任务全局指针 */

static cat_task_t idle_task;                    /**< 空闲任务变量 */
static cat_stack_type_t idle_task_env[CATOS_IDLE_STACK_SIZE];  /**< 空闲任务堆栈 */
static static_prio_private_data_t idle_task_private_data;
static static_prio_task_config_t idle_task_config = 
{
    .prio = (CATOS_MAX_TASK_PRIO - 1),
    .private_data = &idle_task_private_data
};

static cat_uint32_t idle_cnt;                                      /**< 空闲任务时钟节拍计数*/
static cat_uint32_t idle_max_cnt;                                  /**< 最大节拍输(现在是一秒内的)*/

/* funcs decl */
void cat_idle_entry(void *arg);


void cat_idle_task_create(void)
{
    cat_task_create_task_static(
        &idle_task,
        CAT_TASK_STRATEGY_STATIC_PRIO,
        (const cat_uint8_t *)"idle_task",
        cat_idle_entry,
        NULL,
        idle_task_env,
        CATOS_IDLE_STACK_SIZE,
        &idle_task_config
    );
    cat_idle_task = &idle_task;

    if(cat_stdio_is_device_is_set())
    {
        CAT_KPRINTF("[cat_idle] idle task created\r\n");
    }
}

void cat_idle_entry(void *arg)
{
    (void)arg;
	
    for(;;)
    {

    }
}


