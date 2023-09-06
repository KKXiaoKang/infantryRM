#include "../../Chassis/include/Chassis.h"
// usleep函数库 进行微秒级等待
#include "sys/unistd.h"

/**
 * @brief L298N初始化
 * 
 * @return uint8_t 
 */
uint8_t Chassis_Init()
{
    // motor使能
    gpio_config(&motorA_conf);
    gpio_config(&motorB_conf);
    gpio_config(&motorC_conf);
    gpio_config(&motorD_conf);

    // // 使能 引脚为低电平让其进入工作模式
    // Chassis_En();

    return 0;
}

/**
 * @brief 驱动器使能
 */
uint8_t Chassis_En()
{
    // 使能 引脚为低电平让其进入工作模式
    gpio_set_level(GPIO_OUTPUT_A_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_A_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_B_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_B_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_C_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_C_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_D_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_D_IN2, !motorflagState);

    return 0;
}

// /**
//  * @brief 驱动器带动运转
//  * 
//  * @param direct 方向
//  * @return uint8_t 
//  */
// uint8_t Chassis_Run(int direct){

//     if (direct == 0) // 往前顶
//     {
//         gpio_set_level(GPIO_OUTPUT_C_IN1, !motorflagState);
//         gpio_set_level(GPIO_OUTPUT_C_IN2, motorflagState);
//     }
    
//     else if(direct == 1) // 往回缩
//     {
//         gpio_set_level(GPIO_OUTPUT_C_IN1, motorflagState);
//         gpio_set_level(GPIO_OUTPUT_C_IN2, !motorflagState);
//     }

//     return 0;
// }

/**
 * @brief 底盘向前
 * 
 * @param 
 * @return uint8_t 
 */
void Chassis_Forward()
{
    // 
    // 前面两个轮子
    gpio_set_level(GPIO_OUTPUT_C_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_C_IN2, motorflagState);

    gpio_set_level(GPIO_OUTPUT_D_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_D_IN2, motorflagState);

    // 后面两个轮子
    gpio_set_level(GPIO_OUTPUT_A_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_A_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_B_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_B_IN2, !motorflagState);

    // // 前进500微妙
    // usleep(50000);
    // // 0.1s
    // vTaskDelay(100 / portTICK_RATE_MS); 

    // // 使能 引脚为低电平让其进入工作模式
    // Chassis_En();
}

/**
 * @brief 底盘后退
 * 
 * @param 
 * @return uint8_t 
 */
void Chassis_Backward()
{
    // 
    // 后面两个轮子
    gpio_set_level(GPIO_OUTPUT_A_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_A_IN2, motorflagState);

    gpio_set_level(GPIO_OUTPUT_B_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_B_IN2, motorflagState);

    // 前面两个轮子
    gpio_set_level(GPIO_OUTPUT_C_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_C_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_D_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_D_IN2, !motorflagState);

    // // 前进500微妙
    // usleep(50000);
    // // 0.1s
    // vTaskDelay(100 / portTICK_RATE_MS); 

    // // 使能 引脚为低电平让其进入工作模式
    // Chassis_En();
}

/**
 * @brief 底盘左边平移
 * 
 * @param 
 * @return uint8_t 
 */
void Chassis_Left()
{

    // 左边两个轮子
    gpio_set_level(GPIO_OUTPUT_B_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_B_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_C_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_C_IN2, !motorflagState);

    // 右边两个轮子
    gpio_set_level(GPIO_OUTPUT_D_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_D_IN2, motorflagState);

    gpio_set_level(GPIO_OUTPUT_A_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_A_IN2, motorflagState);

    // // 前进500微妙
    // usleep(50000);
    // // 0.1s
    // vTaskDelay(100 / portTICK_RATE_MS); 

    // // 使能 引脚为低电平让其进入工作模式
    // Chassis_En();
}

/**
 * @brief 底盘右边平移
 * 
 * @param 
 * @return uint8_t 
 */
void Chassis_Right()
{
    // 
    // 右边两个轮
    gpio_set_level(GPIO_OUTPUT_A_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_A_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_D_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_D_IN2, !motorflagState);

    // 左边两个轮子
    gpio_set_level(GPIO_OUTPUT_B_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_B_IN2, motorflagState);

    gpio_set_level(GPIO_OUTPUT_C_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_C_IN2, motorflagState);



    // // 前进500微妙
    // usleep(50000);
    // // 0.1s
    // vTaskDelay(100 / portTICK_RATE_MS); 

    // // 使能 引脚为低电平让其进入工作模式
    // Chassis_En();
}


/**
 * @brief 底盘向左旋转
 */
void Chassis_Left_revolve()
{
    // 向左旋转
    gpio_set_level(GPIO_OUTPUT_A_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_A_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_B_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_B_IN2, motorflagState);

    gpio_set_level(GPIO_OUTPUT_C_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_C_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_D_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_D_IN2, motorflagState);

    // // 前进500微妙
    // usleep(50000);
    // // 0.1s
    // vTaskDelay(100 / portTICK_RATE_MS); 

    // // 使能 引脚为低电平让其进入工作模式
    // Chassis_En();
}

/**
 * @brief 底盘向右旋转
 */
void Chassis_Right_revolve()
{
    // 向右旋转
    gpio_set_level(GPIO_OUTPUT_A_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_A_IN2, motorflagState);

    gpio_set_level(GPIO_OUTPUT_B_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_B_IN2, !motorflagState);

    gpio_set_level(GPIO_OUTPUT_C_IN1, !motorflagState);
    gpio_set_level(GPIO_OUTPUT_C_IN2, motorflagState);

    gpio_set_level(GPIO_OUTPUT_D_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_D_IN2, !motorflagState);

    // // 前进500微妙
    // usleep(50000);
    // // 0.1s
    // vTaskDelay(100 / portTICK_RATE_MS); 

    // // 使能 引脚为低电平让其进入工作模式
    // Chassis_En();
}

/**
 * @brief 电机测试
 */
void Motor_Test()
{
    // A 电机
    gpio_set_level(GPIO_OUTPUT_A_IN1, motorflagState);
    gpio_set_level(GPIO_OUTPUT_A_IN2, !motorflagState);
}