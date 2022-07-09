/**
 * @file debug_printf.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-03-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "debug_printf.h"
#include <stdio.h>


void debug_printf_init(uint32_t usart_periph)
{
	gd_eval_com_init(usart_periph);
}


#ifdef GD32F130_150
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART1, (uint8_t)ch);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
    return ch;
}
#endif
