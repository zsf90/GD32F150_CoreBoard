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
