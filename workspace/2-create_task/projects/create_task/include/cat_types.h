/**
 * @file cat_types.h
 * @brief 
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2024-05-07
 * Change Logs:
 * Date           Author        Notes
 * 2024-05-07     mio     first verion
 * 
 */

#ifndef CAT_DEF_H
#define CAT_DEF_H

typedef signed char             cat_i8;
typedef short int               cat_i16;
typedef int                     cat_i32;

typedef unsigned char           cat_u8;
typedef unsigned short int      cat_u16;
typedef unsigned int            cat_u32;

typedef long                    cat_ibase; /**< 有符号最大位宽数，如32位cpu则该数据类型代表int32_t */
typedef unsigned long           cat_ubase; /**< 无符号最大位宽数，如32位cpu则该数据类型代表uint32_t */

#define CAT_NULL                ((void *)0)

/* 错误代码定义 */
#define CAT_EOK                 (0)     /**< 成功 */
#define CAT_ERROR               (1)     /**< 失败 */
#define CAT_EINVAL              (2)     /**< 非法值 */

/* 对齐 */
/**
 * @brief 向上取整对齐
 */
#define CAT_ALIGN(addr, align) \
    (((addr) + (align) - 1) & ~((align) - 1))
/**
 * @brief 向下取整对齐
 */
#define CAT_ALIGN_DOWN(addr, align) \
    ((addr) & ~((align) - 1))


#endif


