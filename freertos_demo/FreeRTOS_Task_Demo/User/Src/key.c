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
