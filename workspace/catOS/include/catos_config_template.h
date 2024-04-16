/**
 * @file cat_config.h
 * @brief the config MACROs
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-19
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-19 1.0    amoigus             内容
 */

#ifndef CATOS_CONFIG_H
#define CATOS_CONFIG_H

/** 版本和构建信息 **/
/* catos版本 */
#define CATOS_CFG_VERSION               "v1.0.2"
/* 构建工具信息 */
#if defined(__CC_ARM)
    #define CATOS_CFG_BUILD_COMPILER    "armcc under 6"
#elif (defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 ))
    #define CATOS_CFG_BUILD_COMPILER    "armcc6(AC6)"
#elif defined(__GNUC__)
    #define CATOS_CFG_BUILD_COMPILER    "gcc(GNU)"
#endif


#endif/* #ifndef CATOS_CONFIG_H */
