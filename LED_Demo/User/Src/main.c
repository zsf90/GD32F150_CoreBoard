/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-03-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "gd32f1x0.h"
#include "systick.h"
#include "debug_printf.h"
#include <stdio.h>
#include "delay.h"


/*!
    \brief      LED 闪烁程序，该函数在 系统中断中重复调用（1ms)调用一次。
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0;
	
    // timingdelaylocal 不等于 0
    if(timingdelaylocal){

        if(timingdelaylocal < 500){
            gd_eval_led_on(LED1);
        }else{
            gd_eval_led_off(LED1);
        }    
        timingdelaylocal--;
    }else{ // timingdelaylocal 等于 0
        timingdelaylocal = 1000;
    }
}

int main(void)
{
	systick_config();
	debug_printf_init(EVAL_COM0);
    gd_eval_led_init(LED1);


	while(1)
	{
		printf("Hello,GD32F150C8T6\n");
        delay_1ms(500);
	}
}
