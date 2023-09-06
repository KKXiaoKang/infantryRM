#ifndef __FIRE_H
#define __FIRE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"            // LOG检查
#include "freertos/FreeRTOS.h"  // FreeRTOS
#include "freertos/task.h"      // Task基本任务
#include "freertos/queue.h"     // FreeRTOS 消息队列
#include "driver/gpio.h"        // GPIO引脚
#include "esp_attr.h"

#include "driver/mcpwm.h"       // mcpwm控制
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"


// 电平状态
static uint8_t FireFlagState = 0;

/*
 * 小步兵的发射 拨弹模块开发
 * 开发模块 无
 * 开发电机 发射为普通马达电机 拨弹为N20电机 100rpm
 * 拨弹模块 Pin B1_18 B2_5  
 * 发射模块 Pin A3_26 A4_25
*/
// 拨弹
#define GPIO_FIRE_A_IN1 18
#define GPIO_FIRE_A_IN2 5
#define GPIO_FIRE_A_PIN_SEL ((1ULL<<GPIO_FIRE_A_IN1) | (1ULL<<GPIO_FIRE_A_IN2))

// 摩擦轮发射
#define GPIO_FIRE_B_IN1 26
#define GPIO_FIRE_B_IN2 25
#define GPIO_FIRE_B_PIN_SEL ((1ULL<<GPIO_FIRE_B_IN1) | (1ULL<<GPIO_FIRE_B_IN2))


// ----------------- 使能区 -------------------------- //
/**
 * @brief   拨弹使能配置
*/
static gpio_config_t Fire_A_config = {
    .pin_bit_mask = GPIO_FIRE_A_PIN_SEL,
    .mode = GPIO_MODE_OUTPUT,   // 输出模式
    .pull_up_en = 0,  // 上拉
    .pull_down_en = 0, // 下拉
    .intr_type = GPIO_INTR_DISABLE,  // 中断模式
};


/**
 * @brief  摩擦轮发射 使能配置
*/
static gpio_config_t Fire_B_config = {
    .pin_bit_mask = GPIO_FIRE_B_PIN_SEL,
    .mode = GPIO_MODE_OUTPUT,   // 输出模式
    .pull_up_en = 0,  // 上拉
    .pull_down_en = 0, // 下拉
    .intr_type = GPIO_INTR_DISABLE,  // 中断模式
};


// ----------------- 函数区 -------------------------- //
/**
 * @brief 发射模块整体初始化
*/
void FIRE_Init();

/**
 * @brief 摩擦轮发射驱动器运转
 * IN1 电平 1 
 * IN2 电平 0 
*/
void Fire_friction_wheel();

/**
 * @brief 拨弹驱动器运转
 * IN1 电平 1 
 * IN2 电平 0 
*/
void Fire_Pluck_Run();

/**
 * @brief 摩擦轮电机停止运动
 * 电平状态 
 * 1
 * 1
 * 刹车 / 慢衰减
 * 电平状态
 * 0
 * 0
 * 滑行 / 快衰减
*/
void Stop_Fire_friction();

/**
 * @brief 拨弹电机停止运动
 * 电平状态 
 * 1
 * 1
 * 刹车 / 慢衰减
*/
void Stop_Fire_Pluck();

#endif