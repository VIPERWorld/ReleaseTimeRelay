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
/***Other***/
const char *p="sdf";
void SYS_INIT(void)
{
    /***延时初始化***/
    delay_init();
    /***中断初始化***/
    NVIC_Configuration();
    uart_init (115200);
    uart2_init(115200);
    uart3_init(115200);

    Sys_Printf(USART1, (char *)"\r\nUSART1 okswerf");
    Sys_Printf(USART2, (char *)"\r\nUSART2 okrth5");
    Sys_Printf(USART3, (char *)"\r\nUSART3 okewtr");
//	  Sys_sPrintf(USART3,(unsigned char *)p,3);
//	  Sys_sPrintf(USART1,(unsigned char *)p,3);
//	  Sys_sPrintf(USART2,(unsigned char *)p,3);
    delay_ms(500);
}

int main(void)
{
    SYS_INIT();
    /***总循环***/
    while (1)
    {
		Sys_Printf(USART1, (char *)"\r\nd okes");
    Sys_Printf(USART2, (char *)"\r\nRToke");
    Sys_Printf(USART3, (char *)"\r\nUSA3 ");
		Sys_Printf(USART1, (char *)"\r\nRT1");
    Sys_Printf(USART2, (char *)"\r\nT2");
    Sys_Printf(USART3, (char *)"\r\n3");
			
	  delay_us(200);
    }
}
