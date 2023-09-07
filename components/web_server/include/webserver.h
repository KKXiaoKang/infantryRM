#ifndef WEB_SERVER_h
#define WEB_SERVER_h

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    char My_URLBuff[30];    // 动态分配 url请求类型 = (char *)malloc(sizeof(char));
    char My_URLString[30];  // 动态分配 url参数     = (char *)malloc(sizeof(char));
}URL_Data;

// 创建web数据队列收发句柄 
static QueueHandle_t URLString_queue = NULL;  // 这里必须保证队列是一个全局静态变量，一直在整个大循环生命周期内，否则会消失

void web_server_start(void);      // web服务开启

static TaskHandle_t urlRecHandle = NULL; // URL接收参数任务句柄

static int Fire_ToFlag = 0;          // 摩擦轮发射状态位
static int Fire_PluckFlag = 0;       // 拨弹盘状态位
// void web_rec_server_start(void);  // web接收参数开启

#ifdef __cplusplus
}
#endif  
#endif