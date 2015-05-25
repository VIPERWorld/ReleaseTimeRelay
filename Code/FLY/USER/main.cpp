/***SYS***/
#include "sys.h"
#include "gpio.h"
#include <string.h>
#include "usr_usart.h"
#include "minos_delay.h"
#include "sys_os.h"
#include "gpio.h"
#include "stm32_config.h"
#include "G32_timer.h"
/***C***/
#include "string.h"
#include "stdlib.h"
/***Other***/
#include "aes.h"
#include "adc.h"

#include "task_led.h"

void SYS_INIT(void)
{
    /***延时初始化***/
    delay_init();
    /***中断初始化***/
    NVIC_Configuration();
    uart_init (115200);
    uart2_init(115200);
    uart3_init(115200);
    Sys_Printf(USART2, (char *)"\r\nUSART2 okrth5");
    Sys_Printf(USART3, (char *)"\r\nUSART3 okewtr");
   delay_ms(100);
}

int main(void)
{
    SYS_INIT();
    /***总循环***/
    while (1)
    {
			RunTaskA(task_led,0);
			
			//delay_ms(100);
    }
}
