/**
 * @file cat_shell_port.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-10
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-10 1.0    amoigus             内容
 */

#include "cat_task.h"
#include "cat_shell_port.h"
#include "cat_shell.h"
#include "cat_stdio.h"
#include "port.h"

#if (CATOS_ENABLE_CAT_SHELL == 1)
cat_task_t shell_task;
static cat_stack_type_t shell_task_env[CATOS_SHELL_STACK_SIZE];
static static_prio_private_data_t shell_task_private_data;
static static_prio_task_config_t shell_task_config = 
{
    .prio = CATOS_SHELL_TASK_PRIO,
    .private_data = &shell_task_private_data
};

static cat_shell_instance_t port_shell_inst_1 = {0};
static cat_shell_config_t shell_cfg = {0};
static cat_uint8_t shell_space[512];

void cat_shell_task_create(void)
{
    cat_int32_t ret = 0;

    /* 将shellspace分配到各个成员 */
    shell_cfg.buffer = shell_space;
    shell_cfg.buf_size = 512;
    if(sizeof(shell_space) < CAT_BUF_SIZE * (CAT_MAX_HISTORY + 1))
    {
        CAT_KPRINTF("[cat_shell_port:%d] shell_space is not enough !\r\n", __LINE__);
        while(1);
    }

    ret = cat_shell_init(&port_shell_inst_1, &shell_cfg);
    if(ret)
    {
        CAT_KPRINTF("[cat_shell_port:%d] cat_shell_init fail!\r\n", __LINE__);
        while(1);
    }

    cat_task_create_task_static(
        &shell_task,
        CAT_TASK_STRATEGY_STATIC_PRIO,
        (const cat_uint8_t *)"shell_task", 
        cat_shell_task_entry, 
        &port_shell_inst_1, 
        shell_task_env, 
        CATOS_SHELL_STACK_SIZE,
        &shell_task_config
    );
    CAT_KPRINTF("[cat_shell_port] shell task created \r\n");
}

cat_int16_t cat_shell_port_getc(cat_uint8_t *data)
{
    cat_int16_t ret = 0;
    *data = CAT_SYS_GETCHAR();
    return ret;
}

cat_int16_t cat_shell_port_putc(cat_uint8_t data)
{
    cat_int16_t ret = 0;

    CAT_SYS_PUTCHAR(data);

    return ret;
}
#else
    /* 不使用就空函数 */
    void cat_shell_task_create(void)
    {
        
    }
#endif /* #if (CATOS_ENABLE_CAT_SHELL == 1) */
