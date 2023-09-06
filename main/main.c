#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include <netdb.h>
#include <sys/socket.h>

// -----------------------------自定义组件文件导入----------------------------------- //
#include "RobotControl.h"
#include "my_dns_server.h"
#include "webserver.h"
// ----------------------------------------------------------------------------------- //

#include "esp_task_wdt.h"  // new DEBUG 看门狗
#include "driver/uart.h"

// -----------------------------自定义库文件导入---------------------------------------- //
// #include "LED/include/MyLed.h"       // 板载LED（代码报错，需DEBUG，可能是HTTP流程选择的不太正确）
#include "Chassis/include/Chassis.h" // 车子底盘 融合了L298N电机驱动
#include "Gimbal/include/Gimbal.h"   // 车子云台 融合了mcpwm 控制舵机的代码
#include "FIRE/include/Fire.h"
// ----------------------------------------------------------------------------------- //

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;   // WIFI 连接状态 FreeRTOS 事件组

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

static const char *TAG = "RMInfantry";

static uint8_t MyLed_Flag = 0;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    //
    // TODO 用信号量事件组 对MyLED进行状态切换
    //
    switch(event->event_id) 
    {
        case SYSTEM_EVENT_STA_START:
            //esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP32 WiFi libs don't currently
            auto-reassociate. 这是一个变通办法，因为ESP32 WiFi库目前没有自动重新关联*/
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            /* 如果有设备连接，则亮灯 */
            ESP_LOGI(TAG,"a phone connected!");
            // MyLed_Flag = 0; 
            // printf("MyLed_Flag : %s", &MyLed_Flag);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            /* 如果无设备连接，则灭灯 */
            ESP_LOGI(TAG,"A Phone disconnect  ...");
            // MyLed_Flag = 1;
            // printf("MyLed_Flag : %s", &MyLed_Flag);
            
        default:
            break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = "全向轮步兵控制器_5",
            .ssid_len = strlen("全向轮步兵控制器_5"),
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 3,
        },
    };

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );

    ESP_LOGI(TAG, "Setting WiFi softAP SSID %s...", wifi_ap_config.ap.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_ap_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

// // --------------------------------------------------- RTOS 任务函数  --------------------------------------------------   // 
// /**
//  * @brief MyLED 板载LED开灯
//  * 
//  * @param pvParam 
//  */
// void MyLED_runTask(void *pvParam){
//     // 将任务订阅到任务监视计时器（TWDT）
//     esp_task_wdt_add(NULL);

    

//     // 获取LED的状态量
//     uint8_t* ptflag = (uint8_t*)pvParam;

//     // 强制要求绝对频率运行
//     const TickType_t xFrequency = 200; // 2s 

//     // 得到上次的时间
//     TickType_t xLastWakeTime = xTaskGetTickCount();

//     while(1)
//     {
//         // 当前正在运行的任务重置任务看门狗计时器（TWDT）
//         esp_task_wdt_reset();

//         // 根据状态量变换
//         if(*ptflag == 0)
//         {
//             MyLED_ON();
//         }
//         else if(*ptflag == 1)
//         {
//             MyLED_Off();
//         }

//         // 绝对频率运行
//         vTaskDelayUntil(&xLastWakeTime, xFrequency); 
//     }
// }


void app_main()
{       
    // TODO 通过FreeRTOS事件组同步信息
    // // 初始化
    // MyLED_Init();

    // 底盘模块初始化
    Chassis_Init();

    // 云台模块初始化
    Gimbal_server_start();

    // 发射模块初始化
    FIRE_Init();

    vTaskDelay( 1000 / portTICK_RATE_MS); // 等待1s

    // // 底盘前进
    // Chassis_Forward();

    // // 底盘后退
    // Chassis_Backward();

    // // 底盘左平移
    // Chassis_Left();

    // // 底盘右平移
    // Chassis_Right();

    // // 底盘左旋转
    // Chassis_Left_revolve();

    // // 底盘右旋转
    // Chassis_Right_revolve();

    // ---------------------------- 网络控制初始化 ----------------------------- //
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();   // 初始化wifi
    
    dns_server_start();    // 开启DNS服务
    web_server_start();    // 开启http服务
    // ----------------------------网络控制初始化 ----------------------------- //

    
    // 下面代码不用打开
    // TODO 一开始是想把Queue放在这里，但是没找到消息队列跨C文件传输的方法
    // web_rec_server_start(); // 开启web url参数接收工作

    // xTaskCreate(MyLED_runTask, "MyLED_runTask", 4096, (void*)&MyLed_Flag, 2, NULL);
}