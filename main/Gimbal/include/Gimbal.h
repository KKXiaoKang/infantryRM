#ifndef __GIMBAL_H
#define __GIMBAL_H

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

/*
 * 小步兵的云台开发
 * 开发模块 无
 * 开发电机 常规PWM 舵机
 * 航向电机 Pin 22
 * 俯仰电机 Pin 23
*/
// You can get these value from the datasheet of servo you use, in general pulse width varies between 1000 to 2000 mocrosecond
// 舵机的最大角度和中位角度要根据舵机的性质进行修改数值
// #define SERVO_MIN_PULSEWIDTH 1000 // Minimum pulse width in microsecond
// #define SERVO_MAX_PULSEWIDTH 2000 // Maximum pulse width in microsecond
#define SERVO_MIN_PULSEWIDTH 500   // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2500  // Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 270       // Maximum angle in degree upto which servo can rotate 最大角度值
#define SERVO_MID_DEGREE 135       // 复位中位角度值
// 定义航向轴的Pin为22
#define GPIO_GIMBAL_YAW 22
// 定义俯仰轴的Pin为23     
#define GPIO_GIMBAL_PITCH 23   

// 全局变量记录角度
// 注：这里必须不能是无符号，无符号会导致没有负号，无法进行负数安全性判断，所以还是有符号好
static int global_Yaw_Angle = SERVO_MID_DEGREE;
static int global_Pitch_Angle = SERVO_MID_DEGREE;

static mcpwm_config_t Yaw_pwm_config = {
    .frequency = 50,    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    .cmpr_a = 0,        //duty cycle of PWMxA = 0
    .cmpr_b = 0,        //duty cycle of PWMxb = 0
    .counter_mode = MCPWM_UP_COUNTER,
    .duty_mode = MCPWM_DUTY_MODE_0,
};

static mcpwm_config_t Pitch_pwm_config = {
    .frequency = 50,    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    .cmpr_a = 0,        //duty cycle of PWMxA = 0
    .cmpr_b = 0,        //duty cycle of PWMxb = 0
    .counter_mode = MCPWM_UP_COUNTER,
    .duty_mode = MCPWM_DUTY_MODE_0,
};

/**
 * @brief mcpwm引脚配置函数
*/
void mcpwm_example_gpio_initialize(void);

/**
 * @brief 云台配置初始化
*/
void Gimbal_server_start(void);

/**
 * @brief 输入旋转角度，自动求出脉冲宽度
*/
uint32_t servo_per_degree_init(uint32_t degree_of_rotation);

/**
 * @brief  输入旋转角度，并进行舵机旋转角度的设置
 * 
 * @param set_angle   要设置的舵机角度
 * @param MotorIndex  0 航向轴  1 俯仰轴
 * @return uint32_t 
 */
uint32_t servo_set_angle_To(uint32_t set_angle, int MotorIndex);
#endif