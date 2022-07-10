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

下面这个例子是上面的改进版本，把`按键扫描功能`给单独定义了一个函数 `key_scan`。
```c
// key.h
/**
 * @file    key.h
 * @author  信念D力量
 * @brief   按键程序头文件
 * @version 0.1
 * @date    2022-05-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __KEY_H
#define __KEY_H
#include "gd32f1x0.h"

/* 主板按键宏定义 */
#define BOARD_BTN_PORT              GPIOA
#define BOARD_BTN_PIN               GPIO_PIN_0
#define BOARD_BTN_CLK               RCU_GPIOA
#define BOARD_BTN_EXTI_LINE         EXTI_0
#define BOARD_BTN_EXTI_SOURCE_PORT  EXTI_SOURCE_GPIOA
#define BOARD_BTN_EXTI_SOURCE_PIN   EXTI_SOURCE_PIN0
#define BOARD_BTN_EXTI_IRQN         EXTI0_1_IRQn

// 定义 KEY 枚举
typedef enum
{
    KEY1 = 0,
} KEY_t;

typedef enum
{
    KEY_ON = 0, // 按键被按下
    KEY_OFF,    // 按键未按下
    KEY_ERROR,
} Key_Scan_Result_t;


void key_init(void); // 按键初始化
Key_Scan_Result_t key_scan(KEY_t keys); // 按键扫描

#endif /* __KEY_H */

```

```c
// key.c
/**
 * @file    key.c
 * @author  信念D力量
 * @brief   按键程序
 * @version 0.1
 * @date    2022-05-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "portmacro.h"
#include "gd32f1x0.h"

extern TaskHandle_t SEND1Task_Handler;

/*
    按键初始化
*/
void key_init(void)
{
    /* 使能外设时钟 */
    rcu_periph_clock_enable(BOARD_BTN_CLK);
    /* GPIO 模式配置 */
    gpio_mode_set(BOARD_BTN_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BOARD_BTN_PIN);
    /* NVIC 中断使能 */
    nvic_irq_enable(BOARD_BTN_EXTI_IRQN, 6U, 6U);

    syscfg_exti_line_config(BOARD_BTN_EXTI_SOURCE_PORT, BOARD_BTN_EXTI_SOURCE_PIN);

    exti_init(BOARD_BTN_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_FALLING);

    exti_interrupt_flag_clear(BOARD_BTN_EXTI_LINE);
}

/*
* 外部中断 0,1 处理函数
*/
void EXTI0_1_IRQHandler(void)
{

    if(RESET != exti_interrupt_flag_get(BOARD_BTN_EXTI_LINE)) {
//        BaseType_t xYieldRequired = xTaskResumeFromISR(SEND1Task_Handler);
//        if (xYieldRequired == pdTRUE) {
//            portYIELD_FROM_ISR(xYieldRequired);
//        }
        exti_interrupt_flag_clear(BOARD_BTN_EXTI_LINE); // 清除中断标志
    }
}

/*******************************************************************************
function_name: key_scan
description: 按键扫描
calls:
called by:
input: KEY_t
output:
return: Key_Scan_Result_t
others:
*******************************************************************************/
Key_Scan_Result_t key_scan(KEY_t keys)
{
    FlagStatus state;
    switch(keys)
    {
    case KEY1:
        state = gpio_input_bit_get(BOARD_BTN_PORT, BOARD_BTN_PIN);
        if(state == RESET) {
            while(SET != gpio_input_bit_get(BOARD_BTN_PORT, BOARD_BTN_PIN)) {
                return KEY_ON;
            }
        } else {
            return KEY_OFF;
        }
        break;
    default:
        break;
    }
    return KEY_ERROR;
} // key_scan

```

```c
// main.c
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
    
    while(1) {
        if(key_scan(KEY1) == KEY_ON) {
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
## 使用二值信号量消抖
```c
/**
 * @file    key.h
 * @author  信念D力量
 * @brief   按键程序头文件
 * @version 0.1
 * @date    2022-05-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __KEY_H
#define __KEY_H
#include "gd32f1x0.h"

/* 主板按键宏定义 */
#define BOARD_BTN_PORT              GPIOA
#define BOARD_BTN_PIN               GPIO_PIN_0
#define BOARD_BTN_CLK               RCU_GPIOA
#define BOARD_BTN_EXTI_LINE         EXTI_0
#define BOARD_BTN_EXTI_SOURCE_PORT  EXTI_SOURCE_GPIOA
#define BOARD_BTN_EXTI_SOURCE_PIN   EXTI_SOURCE_PIN0
#define BOARD_BTN_EXTI_IRQN         EXTI0_1_IRQn

// 定义 KEY 枚举
typedef enum
{
    KEY1 = 0,
} KEY_t;

typedef enum
{
    KEY_ON = 0, // 按键被按下
    KEY_OFF,    // 按键未按下
    KEY_ERROR,
} Key_Scan_Result_t;


void key_init(void); // 按键初始化
Key_Scan_Result_t key_scan(KEY_t keys); // 按键扫描

#endif /* __KEY_H */
```

```c
/**
 * @file    key.c
 * @author  信念D力量
 * @brief   按键程序
 * @version 0.1
 * @date    2022-05-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "portmacro.h"
#include "gd32f1x0.h"
#include "semphr.h"

extern TaskHandle_t SEND1Task_Handler;
extern SemaphoreHandle_t semaphore_handle_1;

/*
    按键初始化
*/
void key_init(void)
{
    /* 使能外设时钟 */
    rcu_periph_clock_enable(BOARD_BTN_CLK);
    /* GPIO 模式配置 */
    gpio_mode_set(BOARD_BTN_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BOARD_BTN_PIN);
    /* NVIC 中断使能 */
    nvic_irq_enable(BOARD_BTN_EXTI_IRQN, 6U, 6U);

    syscfg_exti_line_config(BOARD_BTN_EXTI_SOURCE_PORT, BOARD_BTN_EXTI_SOURCE_PIN);

    exti_init(BOARD_BTN_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_FALLING);

    exti_interrupt_flag_clear(BOARD_BTN_EXTI_LINE);
}

/*
* 外部中断 0,1 处理函数
*/
void EXTI0_1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if(RESET != exti_interrupt_flag_get(BOARD_BTN_EXTI_LINE)) {
        // 释放信号量 1
        xSemaphoreGiveFromISR(semaphore_handle_1, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        
        exti_interrupt_flag_clear(BOARD_BTN_EXTI_LINE); // 清除中断标志
    }
}

/*******************************************************************************
function_name: key_scan
description: 按键扫描
calls:
called by:
input: KEY_t
output:
return: Key_Scan_Result_t
others:
*******************************************************************************/
Key_Scan_Result_t key_scan(KEY_t keys)
{
    FlagStatus state;
    switch(keys)
    {
    case KEY1:
        state = gpio_input_bit_get(BOARD_BTN_PORT, BOARD_BTN_PIN);
        if(state == RESET) {
            while(SET != gpio_input_bit_get(BOARD_BTN_PORT, BOARD_BTN_PIN)) {
                return KEY_ON;
            }
        } else {
            return KEY_OFF;
        }
        break;
    default:
        break;
    }
    return KEY_ERROR;
} // key_scan
```

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

```