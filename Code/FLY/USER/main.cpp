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
#include "data_transfer.h"

u16 task_rtc(void)
{
    _SS
    if (RTC_Init() == 0)
    {
        Sys_Printf(USART1, (char *)"\r\n RTC ok");
        RTC_Set(2015, 5, 25, 11, 9, 30);
    }
    else
    {
        Sys_Printf(USART1, (char *)"\r\n RTC no");
    }
    while (1)
    {
        WaitX(1000);
        unsigned char time[24];
        get_time((u8 *)time);
        Sys_sPrintf(USART1, time, 24);
        u32 tmp = (((((calendar.w_year % 100) * 100
                      + calendar.w_month) * 100
                     + calendar.w_date) * 100
                    + calendar.hour) * 100
                   + calendar.min);
        if (tmp < TimeUnlock.u32)
            TimeUnlockFlag = 1;
        else
            TimeUnlockFlag = 0;
    }
    _EE
}

#define RELAY0_OFF
#define RELAY0_ON
#define RELAY1_OFF
#define RELAY1_ON
#define RELAY2_OFF
#define RELAY2_ON
#define RELAY3_OFF
#define RELAY3_ON
#define RELAY4_OFF
#define RELAY4_ON
#define RELAY5_OFF
#define RELAY5_ON
#define RELAY6_OFF
#define RELAY6_ON
#define RELAY7_OFF
#define RELAY7_ON

void RelayControl(u8 num, u8 stata)
{
    switch (num << 2 | stata)
    {
    case  0: RELAY0_OFF
    case  1: RELAY0_ON
    case  2: RELAY1_OFF
    case  3: RELAY1_ON
    case  4: RELAY2_OFF
    case  5: RELAY2_ON
    case  6: RELAY3_OFF
    case  7: RELAY3_ON
    case  8: RELAY4_OFF
    case  9: RELAY4_ON
    case 10: RELAY5_OFF
    case 11: RELAY5_ON
    case 12: RELAY6_OFF
    case 13: RELAY6_ON
    case 14: RELAY7_OFF
    case 15: RELAY7_ON
    }
}

char TaskRelay(void)
{
    _SS
    while (1)
    {
        WaitX(200);
        if (TimeUnlockFlag)
        {
            for (int i = 0; i < 8; ++i)RelayControl(i, RelayStata[i]);
        }
        else
        {
            for (int i = 0; i < 8; ++i)RelayControl(i, 0);
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
        RunTaskA(task_led, 0);
        RunTaskA(task_rtc, 1);
        RunTaskA(TaskRelay, 2);
    }
}
