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
void write_task(void *pvParameters);
void Timer1_Callback(TimerHandle_t xTimer);

// 定义定时器
TimerHandle_t timer1;

/* 全局变量定义 */



// 主函数
int main(void) {
    // ------ 函数初始化代码区 ------
    key_init();
    debug_printf_init(EVAL_COM0); // 使用 EVAL 库初始化 USART1
    gd_eval_led_init(LED1);       // 使用 EVAL 库初始化 LED1
    gd_eval_led_init(LED2);       // 使用 EVAL 库初始化 LED2
    gd_eval_led_init(LED3);       // 使用 EVAL 库初始化 LED3

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
    timer1 = xTimerCreate("timer-1", pdMS_TO_TICKS(100), pdTRUE, (void*)0, Timer1_Callback);
    xTimerStart(timer1, portMAX_DELAY);
    
    xTaskCreate(write_task, "write_task", 100, NULL, 1, NULL);
    
    vTaskStartScheduler();  // 启动调度器
}


/*******************************************************************************
function_name: write_task
description: FreeRTOS 任务函数
calls:
called by:
input: void*
output:
return: void
others:
*******************************************************************************/
void write_task(void *pvParameters)
{
    while(1) {
        gd_eval_led_toggle(LED1);
        vTaskDelay(1000);
    }
} // send_task1 END!


void Timer1_Callback(TimerHandle_t xTimer)
{
    static uint32_t count = 0;
    count++;
    printf("Timer...(%d)\n", count);
    gd_eval_led_toggle(LED2);
    
    uint32_t ulCallCount;
    ulCallCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
    ulCallCount++;
    vTimerSetTimerID( xTimer, ( void * ) ulCallCount );
    printf("ulCallCount: %d\n", ulCallCount);
}





