#ifndef __MYLED_H
#define __MYLED_H

/**
 * @file Myled.c
 * @author KK
 * @brief ESP32老款Micropython常规芯片 自带电源LED 点亮程序
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// 电平状态
static uint8_t MyLED_flagState = 0;

// Motor A IN 引脚设置
#define GPIO_OUTPUT_MY_LED 22 // 芯片手册 pin 22 为激活板载LED灯pin

/**
 * @brief  motorA 控制 使能
 * 
 */
static gpio_config_t MYLED_conf = {
    .pin_bit_mask = GPIO_OUTPUT_MY_LED,  // 设置EN使能输出引脚
    .mode = GPIO_MODE_OUTPUT,   // 输出模式
    .pull_up_en = 0,  // 上拉
    .pull_down_en = 0, // 下拉
    .intr_type = GPIO_INTR_DISABLE,  // 中断模式
};

/**
 * @brief 板载LED初始化
 * 
 */
void MyLED_Init();

/**
 * @brief 板载LED开灯
 * 
 */
void MyLED_ON();


/**
 * @brief 板载LED关灯
 * 
 */
void MyLED_Off();

#endif