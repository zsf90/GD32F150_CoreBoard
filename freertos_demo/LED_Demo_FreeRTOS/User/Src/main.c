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
#include "FreeRTOS.h"
#include "debug_printf.h"

#include "gd32f1x0.h"
#include "gd32f1x0r_eval.h"
#include "task.h"
#include <stdio.h>
#include "key.h"

/* LED1 Task */
#define LED1_TASK_PRIO 0            // 任务优先级
#define LED1_STK_SIZE 50            // 任务堆栈大小
TaskHandle_t LED1Task_Handler;      // 定义任务句柄，一个任务一个任务句柄
void led1_task(void *pvParameters); // 任务函数

/* LED2 Task */
#define LED2_TASK_PRIO  0
#define LED2_STK_SIZE   50
TaskHandle_t LED2Task_Handler;
void led2_task(void *pvParameters);

// 主函数
int main(void) {
    // ------ 函数初始化代码区 ------
    key_init();
    debug_printf_init(EVAL_COM0); // 使用 EVAL 库初始化 USART1
    gd_eval_led_init(LED1);       // 使用 EVAL 库初始化 LED1
    gd_eval_led_init(LED2);       // 使用 EVAL 库初始化 LED1

    // 创建任务 led1_task
    xTaskCreate((TaskFunction_t)led1_task, \
                (const char*)"led1_task", \
                (uint16_t)LED1_STK_SIZE, \
                (void*)NULL, \
                (UBaseType_t)LED1_TASK_PRIO, \
                (TaskHandle_t*)&LED1Task_Handler);
    // 创建任务 led2_task
    xTaskCreate((TaskFunction_t)led2_task, \
                (const char*)"led2_task", \
                (uint16_t)LED2_STK_SIZE, \
                (void*)NULL, \
                (UBaseType_t)LED2_TASK_PRIO, \
                (TaskHandle_t*)&LED2Task_Handler);

    vTaskStartScheduler();  // 启动任务调度器

    while (1) {

    }
}


/*******************************************************************************
function_name: led1_task
description: FreeRTOS 任务函数
calls:
called by:
input: void*
output:
return: void
others:
*******************************************************************************/
void led1_task(void *pvParameters)
{
    static uint32_t count = 0;
    while(1) {
        gd_eval_led_toggle(LED1);
        printf("LED1: Hello,GD32F150C8T6\n");
        vTaskDelay(500);
        count++;
        if(count > 10) {
            count = 0;
            vTaskSuspend(NULL);
        }
    }
}

/*******************************************************************************
function_name: led1_task
description: FreeRTOS 任务函数
calls:
called by:
input: void*
output:
return: void
others:
*******************************************************************************/
void led2_task(void *pvParameters)
{
    while(1) {
        gd_eval_led_toggle(LED2);
        printf("LED2: Hello,GD32F150C8T6\n");
        vTaskDelay(100);
    }
}
