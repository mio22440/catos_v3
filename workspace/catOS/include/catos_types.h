/**
 * @file catos_types.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-10
 * 
 * Copyright (c) 2024
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2024-04-10 <td>内容
 * </table>
 */

#ifndef CATOS_TYPES_H
#define CATOS_TYPES_H

/************** MACROS*********************/

typedef signed char     cat_i8;
typedef short int       cat_i16;
typedef int             cat_i32;

typedef unsigned char           cat_u8;
typedef unsigned short int      cat_u16;
typedef unsigned int            cat_u32;

/* 错误代码定义 */
#define CAT_EOK                 (0)     /**< 成功 */
#define CAT_ERROR               (1)     /**< 失败 */
#define CAT_EINVAL              (2)     /**< 非法值 */

/************** struct type*********************/


#endif


