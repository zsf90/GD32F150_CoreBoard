#ifndef __DELAY_H
#define __DELAY_H
#include "gd32f1x0.h"
void us_timer_init(void); // 微秒延时定时器初始化
void delay_1us(uint16_t us);
void delay_ms(uint16_t ms);

#endif /* __DELAY_H */
