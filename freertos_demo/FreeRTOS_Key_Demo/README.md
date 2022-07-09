## FreeRTOS 按键消抖程序 - GD32平台

### 版本1 不带消抖 （事件组）

```c
/**
 * @file    main.c
 * @author  信念D力量
 * @brief
 * @version 0.1
 * @date    2022-03-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "main.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "task.h"
#include "debug_printf.h"

#include "gd32f1x0.h"
#include "gd32f1x0r_eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "key.h"

// 任务函数声明
void key_scan_task(void *pvParameters);
void led_task(void *pvParameters);



// 定义一个事件组句柄
EventGroupHandle_t Event_Handle = NULL;

// 定义事件掩码
#define KEY1_EVENT  (0x01 << 0) // 设置事件掩码的位 0

/* 全局变量定义 */



// 主函数
int main(void) {
    // ------ 函数初始化代码区 ------
    key_init();
    debug_printf_init(EVAL_COM0); // 使用 EVAL 库初始化 USART1
    gd_eval_led_init(LED1);       // 使用 EVAL 库初始化 LED1
    gd_eval_led_init(LED2);       // 使用 EVAL 库初始化 LED2

    app_main();

    while (1) {

    }
}

/*******************************************************************************
function_name: app_main
description: while(1) 前调用，该函数可以用来创建RTOS任务
calls:

called by:
input: void*
output:
return: void
others:
*******************************************************************************/
void app_main(void)
{
    Event_Handle =  xEventGroupCreate(); // 创建事件组
    
    if( Event_Handle == NULL) {
        // 创建失败
    } else {
        // 创建成功
        xTaskCreate(key_scan_task, "key_scan_task", 100, NULL, 1, NULL);
        xTaskCreate(led_task, "led_task", 100, NULL, 1, NULL);
        vTaskStartScheduler();  // 启动调度器
    }
    
    
}


/*******************************************************************************
function_name: key_scan_task
description: 按键扫描任务
calls:
called by:
input: void*
output:
return: void
others:
*******************************************************************************/
void key_scan_task(void *pvParameters)
{
    FlagStatus btn_state;
    while(1) {
        // 读取按钮状态
        btn_state = gpio_input_bit_get(BOARD_BTN_PORT, BOARD_BTN_PIN);
        if (btn_state == RESET) {
            // 设置事件组
            xEventGroupSetBits(Event_Handle, KEY1_EVENT);
        }
        vTaskDelay(10);
    }
} // key_scan_task


/*******************************************************************************
function_name: led_task
description: 按键扫描任务
calls:
called by:
input: void*
output:
return: void
others:
*******************************************************************************/
void led_task(void *pvParameters)
{
    EventBits_t r_event_bits;
    while(1) {
    
        /************************************************************* 
        * 等待接收事件标志 
        * 
        * 如果 xClearOnExit 设置为 pdTRUE，那么在 xEventGroupWaitBits()返回之前， 
        * 如果满足等待条件（如果函数返回的原因不是超时），那么在事件组中设置 
        * 的 uxBitsToWaitFor 中的任何位都将被清除。 
        * 如果 xClearOnExit 设置为 pdFALSE， 
        * 则在调用 xEventGroupWaitBits()时，不会更改事件组中设置的位。 
        *
        * xWaitForAllBits 如果 xWaitForAllBits 设置为 pdTRUE，则当 uxBitsToWaitFor 中 
        * 的所有位都设置或指定的块时间到期时，xEventGroupWaitBits()才返回。 
        * 如果 xWaitForAllBits 设置为 pdFALSE，则当设置 uxBitsToWaitFor 中设置的任何 
        * 一个位置 1 或指定的块时间到期时，xEventGroupWaitBits()都会返回。 
        * 阻塞时间由 xTicksToWait 参数指定。 
        *********************************************************/ 
        r_event_bits = xEventGroupWaitBits(Event_Handle, KEY1_EVENT, pdTRUE, pdTRUE, 1000);
        printf("%d\n", r_event_bits);
        if ((r_event_bits & KEY1_EVENT) == KEY1_EVENT) {
            gd_eval_led_toggle(LED1);
        }
        
    }
} // led_task

```

