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
#include "aes.h"
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
	Sys_Printf(USART1, (char *)"ChipUniqueID: %X %X %X",ChipUniqueID[0],ChipUniqueID[1],ChipUniqueID[2]);
    Sys_Printf(USART2, (char *)"\r\nUSART2 okrth5");
    Sys_Printf(USART3, (char *)"\r\nUSART3 okewtr");
	{
	unsigned char dat[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	unsigned char chainCipherBlock[16];
    unsigned char i;
	for(i=0;i<32;i++) AES_Key_Table[i]=i;//做运算之前先要设置好密钥，这里只是设置密钥的DEMO。


	memset(chainCipherBlock,0x00,sizeof(chainCipherBlock));

	aesEncInit();//在执行加密初始化之前可以为AES_Key_Table赋值有效的密码数据

	aesEncrypt(dat, chainCipherBlock);//AES加密，数组dat里面的新内容就是加密后的数据。
	//aesEncrypt(dat+16, chainCipherBlock);//AES源数据大于16字节时，把源数据的指针+16就好了

for(int i=0;i<16;++i)	Sys_Printf(USART1,(char *)" %X",dat[i]);
	}
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
