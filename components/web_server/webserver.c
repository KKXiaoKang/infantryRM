#include <string.h>           // 字符串比较函数
#include <stdlib.h>
#include <malloc.h>           // 动态分配数组

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"   // 队列传输数据
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

#include "webserver.h"
// #include "RobotControl.h"

// 导入底盘控制库
#include "../../main/Chassis/include/Chassis.h"
// 导入云台控制库
#include "../../main/Gimbal/include/Gimbal.h"
// 导入发射模块库
#include "../../main/FIRE/include/Fire.h"

#define UART_MAX_NUM_RX_BYTES (1024)
#define  BUFF_SIZE 1024
/*
#include <netdb.h>
#include <sys/socket.h>
*/

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");

//  全局常量 
static const char *TAG = "WEB_SERVER";

static const char *HTTP_200 =   "HTTP/1.1 200 OK\r\n"
                                "Server: lwIP/1.4.0\r\n"
						        "Content-Type: text/html\r\n"
						        "Connection: Keep-Alive\r\n"
				                "Content-Length: %d \r\n\r\n";

static const char *HTTP_400 =   "HTTP/1.0 400 BadRequest\r\n"
				                "Content-Length: 0\r\n"
				                "Connection: Close\r\n"
				                "Server: lwIP/1.4.0\r\n\n";

// webserver函数
int my_write(int fd,void *buffer,int length)
{
    int bytes_left;
    int written_bytes;
    char *ptr;

    ptr=buffer;
    bytes_left=length;
    while(bytes_left>0)
    {
		written_bytes=send(fd,ptr,bytes_left,0);
        if(written_bytes<=0)
        {
                if(errno==EINTR)
                    written_bytes=0;
                else
                    return(-1);
        }
        bytes_left-=written_bytes;
        ptr+=written_bytes;
		vTaskDelay(10);
    }
    return(0);
}

// 响应创建socket请求Task
void handle_http_request(void *pvParameters)
{
    // --------------------- 队列数据发送 ---------------------------- // 
    // URL_Data myurl_data = {}; // 要发送的URL数据
    // myurl_data.My_URLBuff =  (char *)malloc(sizeof(char));
    // myurl_data.My_URLString = (char *)malloc(sizeof(char));
    URL_Data myurl_data = {};
    BaseType_t sendFlag; // 发送标志位置

    char buff[BUFF_SIZE] = { 0 };  //数据缓冲器
    int length = 0;

    int fd = *(int*)pvParameters;
    int bytes_recvd = 0;
	char *uri = NULL;

    // 长度
    int buff_length = 0; 
    int uri_length = 0;

    // fd socket计数器
	ESP_LOGI(TAG,"Http Sub Task Run with socket: %d",fd);
	
	vTaskDelay(30);

	// 读取HTTP请求头
    bytes_recvd = recv(fd, buff, BUFF_SIZE - 1,0);
	
    if (bytes_recvd <= 0) 
    {
        ESP_LOGE(TAG,"Recv requst header error!");
        goto requst_error;
    }

	// 解析请求类型及请求URI
	uri = strstr(buff,"HTTP");
	if(uri == NULL)
	{
		ESP_LOGE(TAG,"Parase requst header error!");
        goto requst_error;
	}
	uri[0] = 0; uri = NULL;

	uri = strstr(buff," ");
	if(uri == NULL)
	{
		ESP_LOGE(TAG,"Parase requst uri error!");
        goto requst_error;
	}
	uri[0] = 0; uri ++;

    // 解析成功 进行相关打印
    /*
        buff  解析的类型
        uri   文本类型
    */
    ESP_LOGI(TAG,"the reqqust type is %s, uri is: %s",buff,uri);

    // ------------------------------------- 队列数据发送 --------------------------------//
     
    // 先求出长度
    buff_length = sizeof(buff) / sizeof(buff[0]);  // buff 是普通 char buff
    uri_length = strlen(uri);                      // uri  是指针 char* uri
    
    // Type 赋值 只需要开头4个字母即可
    for(int i = 0; i < 4; i++)
    {
        myurl_data.My_URLBuff[i] = buff[i];
    }
    // myurl_data.My_URLBuff[0] = buff[0];

    // URL 赋值 全部进行赋值
    for(int j = 0; j < uri_length; j++)
    {
        myurl_data.My_URLString[j] = uri[j];
    }

    // DEBUG 长度显示正常
    // printf(" buff : %d \n", buff_length);  // 1024
    // printf(" url :  %d \n", uri_length);    // 24

    // // DEBUG url参数显示正常
    // printf(" url : %s \n\n", myurl_data.My_URLString);
    // printf(" buff : %s \n\n", myurl_data.My_URLBuff);
    
    // 先清空复位数据
    xQueueReset(URLString_queue);

    // 发送
    sendFlag = xQueueSend(URLString_queue, (void*)&myurl_data, 0);  // 发送 portMAX_DELAY
    // sendFlag = xQueueSend(URLString_queue, (void*)&myurl_data, portMAX_DELAY);
    
    // 安全性检查
    if(sendFlag != pdPASS)
    {
        printf("URLString_queue send failed!\n Could not send to the queue.\r\n");
    }

    // 指针式子
    // // 记得回收指针
    // free(myurl_data.My_URLString);
    // free(myurl_data.My_URLBuff);

    // ------------------------------------- 队列数据发送 --------------------------------//


    // // Debug TODO 直接加入这段代码 esp32无限重启 未找到原因
    // /*
    //    将uri的数据同步给全局变量url_gobal
    // */
    // for(int url_i = 0; url_i < 20; url_i++)
    // {
    //     url_gobal[url_i] = uri[url_i];
    // }
    // ESP_LOGI(TAG,"the url_gobal is : %s ------------------------ ",url_gobal);


	if(strcmp(buff,"GET") == 0) //响应GET请求 通过strcmp函数进行比较
	{
		length = sprintf(buff,HTTP_200,index_html_end - index_html_start);
		my_write(fd,buff,length);
		my_write(fd,index_html_start, index_html_end - index_html_start);
	}
	else //其他请求不响应
	{
		my_write(fd,HTTP_400,strlen(HTTP_400));
	}

    vTaskDelay(30);

requst_error:
    ESP_LOGI(TAG,"close socket %d",fd);
    // xQueueReset(URLString_queue); // 清空队列数据
    close(fd);           // 
    vTaskDelete(NULL);
}

// 从handle_http_request 优先级 6  当中获取当前url的数据和参数，通过队列进行传输打印。
void web_recdata(void *pvParameters)
{
    // 将任务订阅到任务监视计时器（TWDT）
    esp_task_wdt_add(NULL);

    URL_Data rec_Data = {}; // 要接受的url参数

    BaseType_t recFlag;  // 接收标志位  

    // 解析URL
    char *pTempBuf = (char *)malloc(UART_MAX_NUM_RX_BYTES);

    char url_Buff_01[7];     // 字段类型
    char val_Buff_02[4];     // 具体数值

    while(1)
    {
        // 当前正在运行的任务重置任务看门狗计时器（TWDT）
        esp_task_wdt_reset();     

        recFlag = xQueueReceive(URLString_queue, (void*)&rec_Data, portMAX_DELAY); // 接收队列数据

        if(recFlag == pdPASS)
        {
            // // 打印接收的数据
            // printf("//------ receive data！  Type : %s  \nURL : %s  \n\n  -------// ", rec_Data.My_URLBuff, rec_Data.My_URLString);
            // printf("//------ receive data！  successful!  -------- // \n\n  ");
        }


        // -------------------------- 开启解析 ---------------------------- //
        for( int i = 0; i < 7; i++)  // 解析前7位 buff
        {
            url_Buff_01[i] = rec_Data.My_URLString[i];
        }

        for( int j = 12; j < 16; j++) // 解析后4位 val
        {
            val_Buff_02[j-12] = rec_Data.My_URLString[j];
        }
 
        // ---------- buff : /rocker0505 且buff后面val数字会变换，所以下列使用bcmp比较前n位字符, 而不是strcmp比较
        // ---------- val : 0505
        // debug打印数值
        // printf(" ---------- buff : %s \n\n ", url_Buff_01);
        // printf(" -----------buff_length: %d", sizeof(url_Buff_01)/sizeof(char));
        // printf(" ---------- val : %s \n\n ", val_Buff_02);

        if (bcmp(url_Buff_01, "/gimbal?",5) == 0) // 通过bcmp比较前5位字符是否相等
        {
            printf(" gimbal  触发 : \n\n");
            /*
                按钮触发            
            */

            // 数值分析
            int Myvalue = 0;
            Myvalue = atoi(val_Buff_02); // atoi函数转换为整数型
            // printf(" value : %d \n\n", Myvalue);

            // 函数调用 
            // 开机自动回中，每个初始都为0
            if(Myvalue == 0)
            {
                printf(" 云台抬头 \n\n");
                // 数值向上
                global_Pitch_Angle = global_Pitch_Angle - 10;
                // 安全性检查
                if(global_Pitch_Angle <= 0)
                {
                    global_Pitch_Angle = 0;
                }
                if(global_Pitch_Angle >= SERVO_MAX_DEGREE)
                {
                    global_Pitch_Angle = SERVO_MAX_DEGREE;
                }
                // 调整
                servo_set_angle_To(global_Pitch_Angle, 1);
            }
            if(Myvalue == 1)
            {
                printf(" 云台下降 \n\n");
                // 数值向下
                global_Pitch_Angle = global_Pitch_Angle + 10;
                // 安全性检查
                if(global_Pitch_Angle <= 0)
                {
                    global_Pitch_Angle = 0;
                }
                if(global_Pitch_Angle >= SERVO_MAX_DEGREE)
                {
                    global_Pitch_Angle = SERVO_MAX_DEGREE;
                }
                // 调整
                servo_set_angle_To(global_Pitch_Angle, 1);
            }
            if(Myvalue == 2)
            {
                printf(" 云台左转 \n\n");
                // 数值向左
                global_Yaw_Angle = global_Yaw_Angle + 10;
                // 安全性检查
                if(global_Yaw_Angle <= 0)
                {
                    global_Yaw_Angle = 0;
                }
                if(global_Yaw_Angle >= SERVO_MAX_DEGREE)
                {
                    global_Yaw_Angle = SERVO_MAX_DEGREE;
                }
                // 调整
                servo_set_angle_To(global_Yaw_Angle, 0);
            }
            if(Myvalue == 3)
            {
                printf(" 云台右转 \n\n");
                // 数值向右
                global_Yaw_Angle = global_Yaw_Angle - 10;
                // 安全性检查
                if(global_Yaw_Angle <= 0)
                {
                    global_Yaw_Angle = 0;
                }
                if(global_Yaw_Angle >= SERVO_MAX_DEGREE)
                {
                    global_Yaw_Angle = SERVO_MAX_DEGREE;
                }
                // 调整
                servo_set_angle_To(global_Yaw_Angle, 0);
            }

        }else if(bcmp(url_Buff_01, "/blaste?",5) == 0){
            printf(" blaste  触发 : \n\n");
            /*
                按钮触发            
            */

            // 摩擦轮发射
            Fire_friction_wheel();
            //Fire_Pluck_Run();

        }else if(bcmp(url_Buff_01, "/fireSt?",5) == 0){
            printf(" fireSt  触发 : \n\n");
            /*
                按钮触发            
            */
            // 摩擦轮停止
            Stop_Fire_friction();
            //Stop_Fire_Pluck();
        }else if(bcmp(url_Buff_01, "/Pluck1?",5) == 0){
            printf(" fireSt  触发 : \n\n");
            /*
                按钮触发            
            */
            // 开始拨弹
            Fire_Pluck_Run();
            //Fire_friction_wheel();
        }
        else if(bcmp(url_Buff_01, "/luck2P?",5) == 0){
            printf(" fireSt  触发 : \n\n");
            /*
                按钮触发            
            */
            // 停止拨弹
            Stop_Fire_Pluck();
            //Stop_Fire_friction();
        }          
        else if(bcmp(url_Buff_01, "/robott?",5) == 0){
            printf(" robott  触发 : \n\n");
            /*
                按钮触发            
            */
        }else if(bcmp(url_Buff_01, "/rocker?",5) == 0){
            printf(" rocker  触发 : n\n");
            /*
                矩形触发            
            */

            // 坐标获取
            int Myvalue = 0;
            int chassis_x = 0;
            int chassis_y = 0;
            Myvalue = atoi(val_Buff_02); // 将坐标转换为整数形 606
            // printf(" value : %d \n\n", Myvalue);

            chassis_x = Myvalue / 100;  // x 坐标
            chassis_y = Myvalue % 100;  // y 坐标
            // printf(" --- X : %d   Y: %d ----- \n\n", chassis_x, chassis_y);
            if( (chassis_x > 4 && chassis_x < 6) && (chassis_y > 4 && chassis_y < 6) )
            {
                /*
                    底盘停止
                */
            //    printf(" 底盘向前 \n\n");
               Chassis_En();
            }

            if( (chassis_x >= 4 && chassis_x <=6) && (chassis_y >= 0 && chassis_y <= 4) )
            {
                /*
                    底盘向前
                */
            //    printf(" 底盘向前 \n\n");
               Chassis_Forward();
            }

            if( (chassis_x >= 4 && chassis_x <=6) && (chassis_y >= 6 && chassis_y <= 10) )
            {
                /*
                    底盘向后
                */
            //    printf(" 底盘向后 \n\n");
               Chassis_Backward();
            }

            if( (chassis_x >= 0 && chassis_x <=4) && (chassis_y >= 4 && chassis_y <= 6) )
            {
                /*
                    底盘向左
                */
            //    printf(" 底盘向左 \n\n");
               Chassis_Left();
            }

            if( (chassis_x >= 6 && chassis_x <=10) && (chassis_y >= 4 && chassis_y <= 6) )
            {
                /*
                    底盘向右
                */
            //    printf(" 底盘向右 \n\n");
               Chassis_Right();
            }

            if( ((chassis_x >= 0 && chassis_x <=3) && (chassis_y >= 0 && chassis_y <= 3)) || ((chassis_x >= 0 && chassis_x <= 3) && (chassis_y >= 7 && chassis_y <= 10)) )
            {
                /*
                    底盘向左旋转
                */
            //    printf(" 底盘左旋转 \n\n");
               Chassis_Left_revolve();
            }

            if( ((chassis_x >= 7 && chassis_x <=10) && (chassis_y >= 0 && chassis_y <= 3))  || ((chassis_x >= 7 && chassis_x <= 10) && (chassis_y >= 7 && chassis_y <= 10)))
            {
                /*
                    底盘向右旋转
                */
            //    printf(" 底盘右旋转 \n\n");
               Chassis_Right_revolve();
            }
        }else{
            // 默认情况下底盘都保持不动
            Chassis_En();
        }



        // -------------------------- 结束解析 ---------------------------- //

        // vTaskDelay(20);   // 
        // TODO 检查一下为什么偶尔这里还是会出现看门狗触发，有可能是等待时间太长，或者等待时间太短。
        vTaskDelay(60 / portTICK_RATE_MS);

    }

    // 结束
    vTaskDelete(NULL); // 删除任务
    esp_task_wdt_delete(NULL); // 从任务监视计时器（TWDT）取消订阅任务
}

void webserver(void *pvParameters)
{
    int sockfd,new_fd;/*socket句柄和建立连接后的句柄*/
	struct sockaddr_in my_addr;/*本方地址信息结构体，下面有具体的属性赋值*/
	struct sockaddr_in their_addr;/*对方地址信息*/
	socklen_t sin_size;

	struct timeval tv;//发送接收超时时间
	tv.tv_sec = 10;
    tv.tv_usec = 0;

    sin_size=sizeof(struct sockaddr_in);
	sockfd=socket(AF_INET,SOCK_STREAM,0);//建立socket 
	if(sockfd==-1)
    {
		ESP_LOGE(TAG, "socket failed:%d",errno);
		goto web_err;;
	}
	my_addr.sin_family=AF_INET;/*该属性表示接收本机或其他机器传输*/
	my_addr.sin_port=htons(80);/*端口号*/
	my_addr.sin_addr.s_addr=htonl(INADDR_ANY);/*IP，括号内容表示本机IP*/
	bzero(&(my_addr.sin_zero),8);/*将其他属性置0*/

	if(bind(sockfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))<0)//绑定地址结构体和socket
    {
		ESP_LOGE(TAG,"bind error");
		goto web_err;
	}

    listen(sockfd,8);//开启监听 ，第二个参数是最大监听数 
    ESP_LOGI(TAG, "webserver start...");

    // 通过队列接收 URL参数 进行控制
    // ！！！！代码Todo 这里应该加入冗余设计，在创建好听到socket的时候才开始创建Task，否则自动关闭URL数据接收。
    /*    其实可以在handle_http_request中进行参数解析并且调用相关的电机函数，但是这样有一点很不好的地方就是，有可能
        会同时进行电机调用，导致Pin脚同时发送，信号会错乱？所以用队列数据进行一一解析比较好，但就是有一点，如果没有接收到socket的时候应该停止。 */
    // 该代码如果用户没有及时连接，esp32则会无限连接触发看门狗。
    // xTaskCreate(&web_recdata, "web_recdata", 2048, NULL, 7, NULL);
    // ESP_LOGI(TAG, "webURLData_Rec is start...");

    // 无限循环，创建监听socker，如果socker不存在则取消。   
    while(1)
    {
        new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);//在这里阻塞知道接收到消息，参数分别是socket句柄，接收到的地址信息以及大小 
        if(new_fd==-1)
        {
           ESP_LOGE(TAG,"accept failed");
        }
        else
        {
            // 创建url接收函数  
            if(urlRecHandle == NULL){  // 判断是否之前已经创建了web_recdata Task，如果没有创建，则创建该Task
                if(xTaskCreate(&web_recdata, "web_recdata", 2048, NULL, 7, &urlRecHandle) == pdPASS){ // 判断是否创建成功！
                    ESP_LOGI(TAG, "....webURLData_Rec created successfully  and is start..."); // 创建成功
                } else {
                    printf(" --- WARNING : Task web_recdata !! Unable to Create! please check!!.");
                }
            } else {
                // 任务已经创建，不用重复创建
                printf(" --- WARNING : Task web_recdata !! is Created.");
            }


            // 创建socket解析
            ESP_LOGI(TAG,"Accept new socket: %d",new_fd);

			setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
			setsockopt(new_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

            int *para_fd = malloc(sizeof(int));
            *para_fd = new_fd;

            // 解析请求URL 通过队列进行发送
            xTaskCreate(&handle_http_request, "socket_task", 4096, para_fd, 6, NULL);

            // // 通过队列接收 URL参数 进行控制
            // // ***ERROR*** A stack overflow in task web_recdata has been detected.
            // // 注意 此函数不能放在这里，否则会无限创建
            // xTaskCreate(&web_recdata, "web_recdata", 2048, NULL, 7, NULL);
        }
		vTaskDelay(10);
	}

web_err:
    vTaskDelete(NULL);
}

void web_server_start(void)
{
    // 队列创建   
    URLString_queue = xQueueCreate(30, sizeof(URL_Data));
    if(URLString_queue == NULL) // 如果没有成功创建队列
    {
        printf("Create queue faild!\n"); 
        esp_restart(); // reboot
    }

    xTaskCreate(&webserver, "webserver_task", 2048, NULL, 5, NULL);
}

// void web_rec_server_start(void)
// {
//     xTaskCreate(&web_recdata, "web_recdata", 2048, NULL, 7, NULL);
// }