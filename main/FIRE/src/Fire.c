#include "../../FIRE/include/Fire.h"

// usleep函数库 进行微秒级等待
#include "sys/unistd.h"

/*
 * 小步兵的发射 拨弹模块开发
 * 开发模块 无
 * 开发电机 发射为普通马达电机 拨弹为N20电机 100rpm
 * 发射模块 Pin B1_18 B2_5 
 * 拨弹模块 Pin A3_26 A4_25
*/

/**
 * @brief 发射模块整体初始化
*/
void FIRE_Init()
{
    // 使能
    gpio_config(&Fire_A_config);
    gpio_config(&Fire_B_config);
}

/**
 * @brief 摩擦轮发射驱动器运转
*/
void Fire_friction_wheel()
{
    gpio_set_level(GPIO_FIRE_B_IN1, !FireFlagState);
    gpio_set_level(GPIO_FIRE_B_IN2, FireFlagState);
}

/**
 * @brief 拨弹盘驱动器运转
*/
void Fire_Pluck_Run()
{
    gpio_set_level(GPIO_FIRE_A_IN1, FireFlagState);
    gpio_set_level(GPIO_FIRE_A_IN2, !FireFlagState);
}

/**
 * @brief 摩擦轮电机停止运动
*/
void Stop_Fire_friction()
{
    gpio_set_level(GPIO_FIRE_B_IN1, !FireFlagState);
    gpio_set_level(GPIO_FIRE_B_IN2, !FireFlagState);
}

/**
 * @brief 拨弹电机停止运动
*/
void Stop_Fire_Pluck()
{
    gpio_set_level(GPIO_FIRE_A_IN1, !FireFlagState);
    gpio_set_level(GPIO_FIRE_A_IN2, !FireFlagState);
}