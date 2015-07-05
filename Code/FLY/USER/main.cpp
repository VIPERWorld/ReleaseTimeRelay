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
    //uart3_init(115200);
    Sys_Printf(USART1, (char *)"USART1 okhghg");
    Sys_Printf(USART2, (char *)"USART2 okrth5");
    //Sys_Printf(USART3, (char *)"USART3 okewtr");
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
    case  0: RELAY0_OFF; break;
    case  1: RELAY0_ON;  break;
    case  2: RELAY1_OFF; break;
    case  3: RELAY1_ON;  break;
    case  4: RELAY2_OFF; break;
    case  5: RELAY2_ON;  break;
    case  6: RELAY3_OFF; break;
    case  7: RELAY3_ON;  break;
    case  8: RELAY4_OFF; break;
    case  9: RELAY4_ON;  break;
    case 10: RELAY5_OFF; break;
    case 11: RELAY5_ON;  break;
    case 12: RELAY6_OFF; break;
    case 13: RELAY6_ON;  break;
    case 14: RELAY7_OFF; break;
    case 15: RELAY7_ON;  break;
    default: break;
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
struct Key
{
    u8 format;//密钥格式
    //0 HEX
    //1 ASCII
    u8 index;//:密钥索引号，1--4用于WEP加密密钥，其它加密方式固定为0
    u8 key;   //:密钥字符串.以双引号包围，根据不同的安全模式.密钥使用的长度与格式要求定义如一卜，
    //安全模式  密钥格式
    //      HEX     ASCll
    //WEP64     10个16进制字符住，)  5个ASCll字符‘住，，
    //WEP128    26个16进制字符     13个ASCll字符
    //WPA_PSK(TKIP)     64个16进制字符     8、63个Asell字符
    //WPA_PSK(CCMP/AEs)     64个16进制字符     s、63个A义11字符
    //WPA2_PSK(TKIP)    64个16进制字符     8、63个Asell字符
    //WPA2_PSK(CCMP/AEs)    64个16进制字符     s、63个A义11字符
};

struct UsrtWifiAttribute
{
    u8 bssid[6];//bssid:网络BSSID.长度为12的十六进制数.格式为001EE3A34455
    u8 type;//type:网络类型
// 值  含义
// 0    infra网络
// l    adhoe网络
    u8 channel;//channel:信道号
    u8 b_encry;//b_encry:加密模式
// 值  门..含义
// 0    开放
// l    WEP
// 7    WPA
    u8 ssid[32];//ssid:无线网络名称，1-32个字符，双引号包围
    u8 rssi;//rssi:网络信号强度.不含负号，单位Db.即50表示信号强度为-50Db
};
char *AT_SEND(char *CMD, char *op)
{
    return 0;
}
enum UsrtWifiEcho
{
    AT_OK = 0
};
//char *AT_ECHO()
//{
//  if ("+OK") {}
//  else("ERR") {
//      switch ()
//      {
//      case -1: return; break;
//      case -2: break;
//      case -3: break;
//      case -4: break;
//      case -5: break;
//      case -6: break;
//      case -7: break;
//      case -10: break;
//      case -11: break;
//      case -12: break;
//      case -13: break;
//      case -100: break;
//      default: break;
//      }
//  }
//}
#define UARTWIFIUARTNUM USART1
#define DEBUG_UARTNUM USART2

int TaskUsrtWifi(void)
{
//    UsrtWifiEcho AtReturnE;
//    u8 RxTmpA[20];
//    struct UsrtWifiAttribute UsrtWifiAttributeA[20];
    _SS
    //UsrtWifiGetFlash();
    WaitX(1000); 
	Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+\r"); //空指令
	Sys_Printf(DEBUG_UARTNUM, (char *)"AT+\r"); //空指令
	
    for (static int i = 0; i < 10; ++i)
    {
        WaitX(1000);
        if (0x00!= UsrtWifiAtRxBuffer[0])
        {
            Sys_Printf(DEBUG_UARTNUM, (char *)"%s", (UsrtWifiAtRxBuffer + 1));
            break;
        }
        if (9 == i)
        {
            Sys_Printf(UARTWIFIUARTNUM, (char *)"+++");
					  Sys_Printf(DEBUG_UARTNUM, (char *)"+++");
					
        }
    }

    // //WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"+++"); //透明模式 逃逸
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+WPRT=0\r"); //0设置为对等网络STA
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+SSID=2.4G\r"); //
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+ENCRY=7\r"); //加密为WPA2
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+KEY=1,0,\"a7070318\"\r"); //
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+NIP=0\r"); //DHCP
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+ATM=1\r"); //工作模式 1 手动 0 自动
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+PMTF\r"); //存到FLASH
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+Z\r"); //复位

    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+SKCT=0,0,192.168.1.116,4001\r"); //建立SOCKET
    // /* WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+SKSTT=1\r"); //查询SOCKET 1
    //    WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+SKCLS=1\r"); //删除SOCKET
    //  */
    // WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+SKSDF=1\r"); //设置默认SOCKET（用在透明传输上）
    // /* WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+WARC=0,255\r"); //B/G模式设置 0 B/G混合 255 自动速率
    //    WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+WLEAV\r"); //断开无线网络
    //    WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+WBGR=255\r"); //设置自动重试次数 255 无限重试
    //    WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+ATRM=?\r"); //查询自动工作模式下的模块自动创建的SOCKET连接信息
    // */

    // AtReturnE = UsrtWifiWJoin();
    // if (AT_OK != AtReturnE)
    // {
    //     AtReturnE = UsrtWifiWScan(UsrtWifiAttributeA);
    //     Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+WSCAN");//

    //     WScanDisplay(UsrtWifiAttributeA);
    //     for (;;)//int i = 0; i < 1000; ++i)
    //     {
    //         WaitX(200);
    //         if (SsidGet())
    //         {
    //             AtReturnE = UsrtWifiWJoin();
    //             if (AT_OK != AtReturnE)
    //             {
    //                 ErrorDisplay();
    //             }
    //             else if (AT_OK == AtReturnE)
    //             {
    //                 SuccessDisplay();
    //                 UsrtWifiPMTF();//将内存中的数据存至Flash
    //                 break;
    //             }
    //         }
    //     }
    // }
    // BuildSocket();
    // CheckSocket();
    // SetSysDefaultSocket();
    // UsrtWifiENTM();//进入串口透明传输模式
    while (1)
    {
        WaitX(200);
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
        //RunTaskA(task_rtc, 1);
      //  RunTaskA(TaskRelay, 2);
			  RunTaskA(TaskUsrtWifi, 3);
			
    }
}
