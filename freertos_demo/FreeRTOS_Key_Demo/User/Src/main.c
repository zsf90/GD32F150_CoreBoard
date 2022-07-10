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


// 定义一个信号量
SemaphoreHandle_t semaphore_handle_1 = NULL; // 信号量1

/* 全局变量定义 */



// 主函数
int main(void) {
    // ------ 函数初始化代码区 ------
    key_init();
    debug_printf_init(EVAL_COM0); // 使用 EVAL 库初始化 USART1
    gd_eval_led_init(LED1);       // 使用 EVAL 库初始化 LED1
    gd_eval_led_on(LED1);
    gd_eval_led_init(LED2);       // 使用 EVAL 库初始化 LED2

    app_main();

    while (1) {

    }
}

/*******************************************************************************
function_name: app_main
description: while(1) 前调用，该函数可以用来创建RTOS任务
calls:

called by: main()
input: void
output: 无
return: void
others:
*******************************************************************************/
void app_main(void)
{
    /* 创建信号量1 */
    semaphore_handle_1 = xSemaphoreCreateBinary();
    // xSemaphoreGive(semaphore_handle_1); // 释放信号量
    
    xTaskCreate(key_scan_task, "key_scan_task", 100, NULL, 1, NULL);
    xTaskCreate(led_task, "led_task", 100, NULL, 1, NULL);
    vTaskStartScheduler();  // 启动调度器

}


/*******************************************************************************
function_name: key_scan_task
description: 按键扫描任务
calls:
called by: 无
input: void*
output:
return: void
others:
*******************************************************************************/
void key_scan_task(void *pvParameters)
{
    static uint32_t count;
    while(1) {
        xSemaphoreTake(semaphore_handle_1, portMAX_DELAY); // 获取二值信号量
        vTaskDelay(10); 
        if(key_scan(KEY1) == KEY_ON) {
            count++;
            printf("count = %d\n", count);
            gd_eval_led_toggle(LED1);
        }
    }
} // key_scan_task


/*******************************************************************************
function_name: led_task
description: 按键扫描任务
calls:
called by: 无
input: void*
output:
return: void
others:
*******************************************************************************/
void led_task(void *pvParameters)
{
    
    while(1) {
        vTaskDelay(100);
    }
} // led_task









