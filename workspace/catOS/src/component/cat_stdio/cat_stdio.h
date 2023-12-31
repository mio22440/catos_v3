/**
 * @file cat_stdio.h
 * @author mio_wen (648137125@qq.com)
 * @brief 标准输入输出
 * @version 0.1
 * @date 2022-07-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CAT_STDIO_H
#define CAT_STDIO_H

#include <stdarg.h>
#include "../../core/interrupt/cat_intr.h"

#include "catos_config.h"
#include "catos_types.h"

#if (CATOS_ENABLE_SYS_PRINTF == 1)

#if defined(__CC_ARM)
        /* 系统输入输出宏 */
        #define CAT_KPRINTF(_fmt, ...) \
            do{ \
                cat_printf((const cat_uint8_t *)"[%d] ", catos_systicks); \
                cat_printf((const cat_uint8_t *)_fmt, ##__VA_ARGS__); \
            }while(0)

        #define CAT_SYS_PRINTF(_fmt, ...) \
            cat_printf((const cat_uint8_t *)_fmt, ##__VA_ARGS__)

        #define CAT_SYS_SCANF(_fmt, ...) \
            cat_scanf((const cat_uint8_t *)_fmt, ##__VA_ARGS__)

        #define CAT_SYS_PUTCHAR(_ch) \
            cat_putchar(_ch)

        #define CAT_SYS_GETCHAR() \
            cat_getchar()
        
        #define sprintf(_buf, _fmt, ...) \
            cat_sprintf((cat_uint8_t *)_buf, (const cat_uint8_t *)_fmt, ##__VA_ARGS__)

        /* 调试输出宏 */    
        #if( CATOS_ENABLE_DEBUG_PRINTF == 1)
            #define CAT_DEBUG_PRINTF(_fmt, ...) \
                cat_printf((const cat_uint8_t *)_fmt, ##__VA_ARGS__)
        #else //#if( DEBUG_PRINT_ENABLE == 1)
            #define CAT_DEBUG_PRINTF(_fmt, ...)
        #endif //#if( DEBUG_PRINT_ENABLE == 1)

#elif defined(__GNUC__) //#if defined(__CC_ARM)
    /* 系统输入输出宏 */
    #define CAT_KPRINTF(_fmt, ...) \
        do{ \
            cat_printf((const cat_uint8_t *)"[%d] ", catos_systicks); \
            cat_printf((const cat_uint8_t *)_fmt, ##__VA_ARGS__); \
        }while(0)

    #define CAT_SYS_PRINTF(_fmt, ...) \
        cat_printf((const cat_uint8_t *)_fmt, ##__VA_ARGS__)

    #define CAT_SYS_SCANF(_fmt, ...) \
        cat_scanf((const cat_uint8_t *)_fmt, ##__VA_ARGS__)

    #define CAT_SYS_PUTCHAR(_ch) \
        cat_putchar(_ch)

    #define CAT_SYS_GETCHAR() \
        cat_getchar()

    #define sprintf(_buf, _fmt, ...) \
        cat_sprintf((cat_uint8_t *)_buf, (const cat_uint8_t *)_fmt, ##__VA_ARGS__)

    /* 调试输出宏 */    
    #if( CATOS_ENABLE_DEBUG_PRINTF == 1)
        #define CAT_DEBUG_PRINTF(_fmt, ...) \
            cat_printf((const cat_uint8_t *)_fmt, ##__VA_ARGS__)
    #else //#if( DEBUG_PRINT_ENABLE == 1)
        #define CAT_DEBUG_PRINTF(_fmt, ...)
    #endif //#if( DEBUG_PRINT_ENABLE == 1)
#endif //#if defined(__CC_ARM)

/* PUBLIC FUNC DECL START */
/**
 * @brief 设置stdio输入输出设备
 * 
 * @param  name             设备名称
 * @return cat_uint8_t          成功失败
 */
cat_uint8_t cat_stdio_set_device(const cat_uint8_t *name);

/**
 * @brief 检查stdio设备是否设置
 * 
 * @return cat_uint8_t 0：未设置
 *                 1：已设置
 */
cat_uint8_t cat_stdio_is_device_is_set(void);

cat_uint8_t cat_getchar(void);
cat_uint8_t cat_putchar(cat_uint8_t ch);
cat_int32_t cat_scanf(const cat_uint8_t *format, ...);
cat_int32_t cat_printf(const cat_uint8_t *format, ...);
cat_int32_t cat_sprintf(cat_uint8_t *buf, const cat_uint8_t *format, ...);

/* PUBLIC FUNC DECL END */


#endif /* #if (CATOS_ENABLE_SYS_PRINTF == 1) */

#endif /* #define CAT_STDIO_H */
