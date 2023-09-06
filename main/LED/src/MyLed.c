#include "../../LED/include/MyLed.h"

// usleep函数库 进行微秒级等待
#include "sys/unistd.h"

/**
 * @brief 板载LED初始化
 * 
 */
void MyLED_Init()
{
    // pin 配置
    gpio_config(&MYLED_conf);
    
    // // 使能初始化 进入关闭状态
    // gpio_set_level(GPIO_OUTPUT_MY_LED, MyLED_flagState);
}

/**
 * @brief 板载LED开灯
 * 
 */
void MyLED_ON()
{
    // 使能 高电平
    gpio_set_level(GPIO_OUTPUT_MY_LED, !MyLED_flagState);
}


/**
 * @brief 板载LED关灯
 * 
 */
void MyLED_Off()
{
    // 使能 低电平
    gpio_set_level(GPIO_OUTPUT_MY_LED, MyLED_flagState);
}
