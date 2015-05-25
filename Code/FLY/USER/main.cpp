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
	  Sys_Printf(USART1, (char *)"USART1 okhghg");
    Sys_Printf(USART2, (char *)"USART2 okrth5");
    Sys_Printf(USART3, (char *)"USART3 okewtr");
    delay_ms(100);
}

#include "rtc.h"
char task_rtc(void)
{
    _SS
    if (RTC_Init()==0)
    {
        Sys_Printf(USART1, (char *)"\r\n RTC ok");
			  RTC_Set(2015,5,25,11,9,30);
    }
    else
    {
        Sys_Printf(USART1, (char *)"\r\n RTC no");
    }
    while (1)
    {
        WaitX(200);
        unsigned char time[24];
        get_time((u8 *)time);
        Sys_sPrintf(USART1, time, 24);
//			if(u32(u32(u32(u32(u32(calendar.w_year%100)*100
//				+calendar.w_month)*100
//			  +calendar.w_date)*100
//			  +calendar.hour)*100
//			  +calendar.min)<100)
//			{
			uint64_t tmp=((calendar.w_year%100)*100+calendar.w_month)*100+calendar.w_date;
				if(tmp<100)
			{
				
			}
    }
    _EE
}
int main(void)
{
    SYS_INIT();
    /***总循环***/
    while (1)
    {
			RunTaskA(task_led,0);
			//RunTaskA(task_rtc,1);
    }
}
