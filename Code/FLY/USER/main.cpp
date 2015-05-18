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
const char *p = "sdf";
void SYS_INIT(void)
{
    /***延时初始化***/
    delay_init();
    /***中断初始化***/
    NVIC_Configuration();
    uart_init (115200);
    uart2_init(115200);
    uart3_init(115200);
  u32 ChipUniqueID[3];
 ChipUniqueID[2] = *(__IO u32*)(0X1FFFF7E8);  // 低字节
 ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
 ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
	Sys_Printf(USART1, (char *)"ChipUniqueID: %d %d %d",ChipUniqueID[0],ChipUniqueID[1],ChipUniqueID[2]);
    Sys_Printf(USART2, (char *)"\r\nUSART2 okrth5");
    Sys_Printf(USART3, (char *)"\r\nUSART3 okewtr");
    delay_ms(500);
}

int main(void)
{
    SYS_INIT();
    /***总循环***/
    while (1)
    {
//        Sys_Printf(USART1, (char *)"ndokes");
//        Sys_Printf(USART2, (char *)"nRToke");
//        Sys_Printf(USART3, (char *)"nUSA3 ");
//        Sys_Printf(USART1, (char *)"nRT1");
//        Sys_Printf(USART2, (char *)"nT2");
//        Sys_Printf(USART3, (char *)"n3");
//        Sys_Printf(USART1, (char *)"\r\n");

        delay_ms(100);
    }
}
