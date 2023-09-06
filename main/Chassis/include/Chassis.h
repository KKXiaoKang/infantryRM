#ifndef __CHASSIS_H
#define __CHASSIS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// #include "driver/ledc.h"  // 底盘pwm引脚ledc输出
// // ------------------------   ledc引脚设置  ----------------------------   //
// #define LEDC_HS_TIMER          LEDC_TIMER_0
// #define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
// #define LEDC_HS_CH0_GPIO       (18)        // 选择GPIO18为PWM信号输出
// #define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0

// #define LEDC_TEST_CH_NUM       (1)
// #define LEDC_TEST_DUTY         (4000)      // 初始占空比为40% (assuming 10-bit resolution)
// #define LEDC_TEST_FADE_TIME    (3000)
// // ------------------------   ledc引脚设置  ----------------------------   //



// 电平状态
static uint8_t motorflagState = 0;
/*
 * 小步兵的底盘开发
 * 开发模块 L298N 电机驱动模块
 * 开发电机 N20 电机
 * 电机A 模块1 motorA IN1 IN2
 * 电机B 模块1 motorB IN3 IN4
 * 电机C 模块2 motorA IN1 IN2
 * 电机D 模块2 motorB IN3 IN4   
*/
// 老引脚图
// 25 26 C 电机  0 顺时针  1 逆时针
// 32 33 D 电机  0 顺时针  1 逆时针
// 16 17 B 电机  0 顺时针  1 逆时针
// 18 19 A 电机  0 顺时针  1 逆时针
// ------------------------   电机设置  ----------------------------   //
//                                       | 炮管
//                 18 19 A 电机      0  -|-  0  B 电机  16 17
//                                      | |
//                                      | |
//                 25 26 C 电机      0  ---  0  D 电机  32 33

//  下面为PCB新引脚图 全向轮步兵存档
//  AB电机一个驱动， CD电机一个驱动

// 说明

// 0 1 逆时针 引脚输出
// 1 0 顺时针 引脚输出
//                  枪口指向
// 13 15 左上电机     ^        0 2 右上电机
//                   ——
//                  |  |        标志
//                   ——        
// 32 33 左下电机              16 17 右下电机
#define GPIO_OUTPUT_A_IN1 16
#define GPIO_OUTPUT_A_IN2 17
#define GPIO_OUTPUT_A_PIN_SEL  ((1ULL<<GPIO_OUTPUT_A_IN1) | (1ULL<<GPIO_OUTPUT_A_IN2)) 

#define GPIO_OUTPUT_B_IN1 32
#define GPIO_OUTPUT_B_IN2 33
#define GPIO_OUTPUT_B_PIN_SEL  ((1ULL<<GPIO_OUTPUT_B_IN1) | (1ULL<<GPIO_OUTPUT_B_IN2)) 

#define GPIO_OUTPUT_C_IN1 13
#define GPIO_OUTPUT_C_IN2 15
#define GPIO_OUTPUT_C_PIN_SEL  ((1ULL<<GPIO_OUTPUT_C_IN1) | (1ULL<<GPIO_OUTPUT_C_IN2)) 

#define GPIO_OUTPUT_D_IN1 0
#define GPIO_OUTPUT_D_IN2 2
#define GPIO_OUTPUT_D_PIN_SEL  ((1ULL<<GPIO_OUTPUT_D_IN1) | (1ULL<<GPIO_OUTPUT_D_IN2)) 

/**
 * @brief  motorA 控制 使能
 * 
 */
static gpio_config_t motorA_conf = {
    .pin_bit_mask = GPIO_OUTPUT_A_PIN_SEL,  // 设置EN使能输出引脚
    .mode = GPIO_MODE_OUTPUT,   // 输出模式
    .pull_up_en = 0,  // 上拉
    .pull_down_en = 0, // 下拉
    .intr_type = GPIO_INTR_DISABLE,  // 中断模式
};

/**
 * @brief  motorB 控制 使能
 * 
 */
static gpio_config_t motorB_conf = {
    .pin_bit_mask = GPIO_OUTPUT_B_PIN_SEL,  // 设置EN使能输出引脚
    .mode = GPIO_MODE_OUTPUT,   // 输出模式
    .pull_up_en = 0,  // 上拉
    .pull_down_en = 0, // 下拉
    .intr_type = GPIO_INTR_DISABLE,  // 中断模式
};

/**
 * @brief  motorC 控制 使能
 * 
 */
static gpio_config_t motorC_conf = {
    .pin_bit_mask = GPIO_OUTPUT_C_PIN_SEL,  // 设置EN使能输出引脚
    .mode = GPIO_MODE_OUTPUT,   // 输出模式
    .pull_up_en = 0,  // 上拉
    .pull_down_en = 0, // 下拉
    .intr_type = GPIO_INTR_DISABLE,  // 中断模式
};

/**
 * @brief  motorD 控制 使能
 * 
 */
static gpio_config_t motorD_conf = {
    .pin_bit_mask = GPIO_OUTPUT_D_PIN_SEL,  // 设置EN使能输出引脚
    .mode = GPIO_MODE_OUTPUT,   // 输出模式
    .pull_up_en = 0,  // 上拉
    .pull_down_en = 0, // 下拉
    .intr_type = GPIO_INTR_DISABLE,  // 中断模式
};

/**
 * @brief 驱动器初始化
 */
uint8_t Chassis_Init();

/**
 * @brief 驱动器使能
 */
uint8_t Chassis_En();

// /**
//  * @brief 驱动器运转
//  */
// uint8_t Chassis_Run(int direct);

/**
 * @brief 底盘前进
 */
void Chassis_Forward();

/**
 * @brief 底盘后退
 */
void Chassis_Backward();

/**
 * @brief 底盘左边平移
 */
void Chassis_Left();

/**
 * @brief 底盘右边平移
 */
void Chassis_Right();

/**
 * @brief 底盘向左旋转
 */
void Chassis_Left_revolve();

/**
 * @brief 底盘向右旋转
 */
void Chassis_Right_revolve();

/**
 * @brief 电机测试
 */
void Motor_Test();

#endif