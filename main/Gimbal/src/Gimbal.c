#include "../../Gimbal/include/Gimbal.h"
// usleep函数库 进行微秒级等待
#include "sys/unistd.h"

/**
 * @brief mcpwm引脚配置函数
*/
void mcpwm_example_gpio_initialize()
{
    printf("initializing mcpwm servo control gpio......\n");
    // 云台航向电机引脚初始化
    // 云台俯仰电机引脚初始化
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_GIMBAL_YAW);   // MCPWM_UNIT_0 单元
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, GPIO_GIMBAL_PITCH); // MCPWM_UNIT_1 单元
}

/**
 * @brief 输入旋转角度，自动求出脉冲宽度
*/
uint32_t servo_per_degree_init(uint32_t degree_of_rotation)
{
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}

/**
 * @brief 云台配置初始化
*/
void Gimbal_server_start()
{
    // 01. mcpwm gpio initialization
    // 引脚初始化
    mcpwm_example_gpio_initialize();

    // 02. initial mcpwm configuration
    // mcpwm结构体初始化
    printf("Configuring Initial Parameters of mcpwm......\n");
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &Yaw_pwm_config);    //Configure PWM0A & PWM0B with above settings
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &Pitch_pwm_config);  //Configure PWM0A & PWM0B with above settings

    // 03 电机角度初始化 舵机开机自动回中回到0
    // 每台机器人的开电归中位置不一样
    // 0 航向
    // 1 俯仰
    
    // /**
    //  * 编号2的车
    //  * */
    // servo_set_angle_To(180, 0);
    // servo_set_angle_To(50, 1);

    // /**
    //  * 编号3的车
    //  * */
    // servo_set_angle_To(180, 0);
    // servo_set_angle_To(90, 1);

    /**
     * 编号4的车
     * */
    servo_set_angle_To(235, 0);
    servo_set_angle_To(90, 1);
}

/**
 * @brief  输入旋转角度，并进行舵机旋转角度的设置
 * 
 * @param set_angle   要设置的舵机角度
 * @param MotorIndex  0 航向轴  1 俯仰轴
 * @return uint32_t 
 */
uint32_t servo_set_angle_To(uint32_t set_angle, int MotorIndex)
{
    // 安全性检查
    if(set_angle <= 0)
    {
        set_angle = 0;
    }
    if(set_angle >= SERVO_MAX_DEGREE)
    {
        set_angle = SERVO_MAX_DEGREE;
    }

    // 设置的是航向轴
    if(MotorIndex == 0)
    {
        uint32_t Yaw_pulse;
        // 要设置的角度为
        printf(" 航向轴设置的角度为: %d\n", set_angle);
        // 求出脉冲
        Yaw_pulse = servo_per_degree_init(set_angle);
        // 打印脉冲宽度
        printf("航向轴 脉冲pulse width: %dus\n", Yaw_pulse);    
        // 开始设置新的电磁脉冲切换mcpwm
        mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, Yaw_pulse);
        //
        vTaskDelay(10);
    }

    // 设置的是俯仰轴
    if(MotorIndex == 1)
    {
        uint32_t Pitch_pulse;
        // 要设置的角度为
        printf(" 俯仰轴设置的角度为: %d\n", set_angle);
        // 求出脉冲
        Pitch_pulse = servo_per_degree_init(set_angle);
        // 打印脉冲宽度
        printf("航向轴 脉冲pulse width: %dus\n", Pitch_pulse);    
        // 开始设置新的电磁脉冲切换mcpwm
        mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, Pitch_pulse);
        //
        vTaskDelay(10);
    }

    // 正常结束
    return 0;
}