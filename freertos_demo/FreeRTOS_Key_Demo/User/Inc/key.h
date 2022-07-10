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
