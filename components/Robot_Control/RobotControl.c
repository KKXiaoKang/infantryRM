#include <string.h>
#include <stdlib.h>
#include <malloc.h>           // 动态分配数组

#include "freertos/FreeRTOS.h"      // FreeRTOS
#include "freertos/task.h"          // 任务创建
#include "freertos/event_groups.h"  // 事件组（信号量）
#include "freertos/queue.h"         // 消息队列传输数据

#include "esp_task_wdt.h"  // new DEBUG 看门狗

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "RobotControl.h"
#include "webserver.h"

static const char *TAG = "RobotControl";

// 从handle_http_request 优先级 6  当中获取当前url的数据和参数，通过队列进行传输打印。
void web_recdata(void *pvParameters)
{
    // 将任务订阅到任务监视计时器（TWDT）
    esp_task_wdt_add(NULL);

    const URL_Data rec_Data = {}; // 要接受的url参数

    BaseType_t recFlag;  // 接收标志位  

    while(1)
    {
        // 当前正在运行的任务重置任务看门狗计时器（TWDT）
        esp_task_wdt_reset();     

        recFlag = xQueueReceive(URLString_queue, (void*)&rec_Data, portMAX_DELAY); // 接收队列数据

        if(recFlag == pdPASS)
        {
            // 打印接收的数据
            printf("//------ receive data！  Type : %s  \nURL : %s  \n\n  -------// ", rec_Data.My_URLBuff, rec_Data.My_URLString);
        }
        // else
        // {
        //     // 如果没成功则跳出循环结束Task
        //     break;
        // }

        // vTaskDelay(20);   // 
        // TODO 检查一下为什么偶尔这里还是会出现看门狗触发，有可能是等待时间太长，或者等待时间太短。
        vTaskDelay(60 / portTICK_RATE_MS);

    }

    // 结束
    vTaskDelete(NULL); // 删除任务
    esp_task_wdt_delete(NULL); // 从任务监视计时器（TWDT）取消订阅任务
}

void robot_server_start(void)
{
    ESP_LOGI(TAG, "RobotControl start...");
    
    // // 队列创建   
    // URLString_queue = xQueueCreate(30, sizeof(URL_Data));
    // if(URLString_queue == NULL) // 如果没有成功创建队列
    // {
    //     printf("Create queue faild!\n"); 
    //     esp_restart(); // reboot
    // }

    // // 创建url接收函数  
    // if(urlRecHandle == NULL){  // 判断是否之前已经创建了web_recdata Task，如果没有创建，则创建该Task
    //     if(xTaskCreate(&web_recdata, "web_recdata", 2048, NULL, 7, &urlRecHandle) == pdPASS){ // 判断是否创建成功！
    //         ESP_LOGI(TAG, "....webURLData_Rec created successfully  and is start..."); // 创建成功
    //     } else {
    //         printf(" --- WARNING : Task web_recdata !! Unable to Create! please check!!.");
    //     }
    // } else {
    //     // 任务已经创建，不用重复创建
    //     printf(" --- WARNING : Task web_recdata !! is Created.");
    // }
}