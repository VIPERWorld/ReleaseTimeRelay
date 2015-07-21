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
    Sys_Printf(USART1, (char *)"USART3 USARTSCREEN\r\n");
    Sys_Printf(USART2, (char *)"USART2 DEBUG_UARTNUM\r\n");
    Sys_Printf(USART3, (char *)"USART1 UARTWIFIUARTNUM\r\n");
    delay_ms(100);
}

#include "rtc.h"
#include "data_transfer.h"

u16 task_rtc(void)
{
    _SS
		TimeUnlock.u32=0x7fffffff;
    if (RTC_Init() == 0)
    {
        Sys_Printf(DEBUG_UARTNUM, (char *)"\r\n RTC ok");
        RTC_Set(2015, 5, 25, 11, 9, 30);
    }
    else
    {
        Sys_Printf(DEBUG_UARTNUM, (char *)"\r\n RTC no");
    }
    while (1)
    {
        //  WaitX(60000);
        WaitX(6000);

        unsigned char time[24];
        get_time((u8 *)time);

        u32 tmp = (((((calendar.w_year % 100) * 100
                      + calendar.w_month) * 100
                     + calendar.w_date) * 100
                    + calendar.hour) * 100
                   + calendar.min);
        //Sys_Printf(DEBUG_UARTNUM, (char *)"\r\n RTC: %d %s", tmp, time);
        if (tmp < TimeUnlock.u32)
        {
            TimeUnlockFlag = 1;
            //Sys_Printf(DEBUG_UARTNUM, (char *)"\r\n TimeUnlockFlag 1");
        }
        else
        {
            TimeUnlockFlag = 0;
            //Sys_Printf(DEBUG_UARTNUM, (char *)"\r\n TimeUnlockFlag 0");
        }
    }
    _EE
}

#define RELAY0_OFF Set_C9
#define RELAY0_ON  Clr_C9
#define RELAY1_OFF Set_C8
#define RELAY1_ON  Clr_C8
#define RELAY2_OFF Set_C7
#define RELAY2_ON  Clr_C7
#define RELAY3_OFF Set_C6
#define RELAY3_ON  Clr_C6
#define RELAY4_OFF Set_B15
#define RELAY4_ON  Clr_B15
#define RELAY5_OFF Set_B14
#define RELAY5_ON  Clr_B14
#define RELAY6_OFF Set_B13
#define RELAY6_ON  Clr_B13
#define RELAY7_OFF Set_B12
#define RELAY7_ON  Clr_B12

void RelayControl(void)
{
    if (RelayStata[0]) {RELAY0_ON;} else {RELAY0_OFF;}
    if (RelayStata[1]) {RELAY1_ON;} else {RELAY1_OFF;}
    if (RelayStata[2]) {RELAY2_ON;} else {RELAY2_OFF;}
    if (RelayStata[3]) {RELAY3_ON;} else {RELAY3_OFF;}
    if (RelayStata[4]) {RELAY4_ON;} else {RELAY4_OFF;}
    if (RelayStata[5]) {RELAY5_ON;} else {RELAY5_OFF;}
    if (RelayStata[6]) {RELAY6_ON;} else {RELAY6_OFF;}
    if (RelayStata[7]) {RELAY7_ON;} else {RELAY7_OFF;}
}
void RelayControlOff(void)
{
    RELAY0_OFF;
    RELAY1_OFF;
    RELAY2_OFF;
    RELAY3_OFF;
    RELAY4_OFF;
    RELAY5_OFF;
    RELAY6_OFF;
    RELAY7_OFF;
}

#include "EXTI.h"
int TaskRelay(void)
{
    _SS
    PC9_OUT;
    PC8_OUT;
    PC7_OUT;
    PC6_OUT;
    PB15_OUT;
    PB14_OUT;
    PB13_OUT;
    PB12_OUT;

    RELAY0_ON;
    RELAY1_ON;
    RELAY2_ON;
    RELAY3_ON;
    RELAY4_ON;
    RELAY5_ON;
    RELAY6_ON;
    RELAY7_ON;
    EXTI_Configuration(GPIOD, GPIO_Pin_2, 0);      // 表示作为外部中断 下降沿触发
    //EXTI_Configuration(GPIOB, GPIO_Pin_3, 0);    // 表示作为外部中断 下降沿触发
    EXTI_Configuration(GPIOC, GPIO_Pin_5, 0);      // 表示作为外部中断 下降沿触发
    EXTI_Configuration(GPIOA, GPIO_Pin_7, 0);      // 表示作为外部中断 下降沿触发
    EXTI_NVIC_Configuration(2, 2, 0, 0);
    //EXTI_NVIC_Configuration(3,2,1,1);
    EXTI_NVIC_Configuration(5, 2, 0, 0);
    EXTI_NVIC_Configuration(7, 2, 0, 0);

    Sys_Printf(DEBUG_UARTNUM, "\r\nVAL:%d", AbsoluteOpticalEncoder_VAL);

    while (1)
    {
        WaitX(10);
        if (TimeUnlockFlag)
        {
            //if (AbsoluteOpticalEncoder_VAL != AbsoluteOpticalEncoder_LastVAL)
            {
                //AbsoluteOpticalEncoder_LastVAL = AbsoluteOpticalEncoder_VAL;
                //Sys_Printf(DEBUG_UARTNUM, "\r\n");
                for (int i = 0; i < AbsoluteOpticalEncoderNUM; ++i)
                {
                    if (
                        (AbsoluteOpticalEncoder_VAL > AbsoluteOpticalEncoder_Apart[i][0]) &&
                        (AbsoluteOpticalEncoder_VAL < AbsoluteOpticalEncoder_Apart[i][1])
                    )
                    {
                        RelayStata[i] = 1;
                    }
                    else
                    {
                        RelayStata[i] = 0;
                    }
                    //Sys_Printf(DEBUG_UARTNUM, "%d ", AbsoluteOpticalEncoder_Apart[i][0]);
                    //Sys_Printf(DEBUG_UARTNUM, "%d ", AbsoluteOpticalEncoder_Apart[i][1]);
                }
                //Sys_Printf(DEBUG_UARTNUM, "\r\nVAL:%d", AbsoluteOpticalEncoder_VAL);
                //Sys_Printf(DEBUG_UARTNUM, "\r\nRelayStata:%d %d %d %d %d %d", RelayStata[0], RelayStata[1], RelayStata[2], RelayStata[3], RelayStata[4], RelayStata[5], RelayStata[6]);
            }
            RelayControl();
        }
        else
        {
            RelayControlOff();
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

#define CONFIGSUMNUM 3
const char *ATCommandList[CONFIGSUMNUM][3] = {
    {   "AT+WPRT=?",//网络类型
        "OK=0",
        "AT+WPRT=0"
    },
//    {
//        "AT+SSID=?",
//        "OK=\"2.4G\"",
//        "AT+SSID=2.4G"
//    },
//    {
//        "AT+ENCRY=?",
//        "OK=7",
//        "AT+ENCRY=7"
//    },
//    {
//        "AT+KEY=?",
//        "OK=1,0,\"a7070318\"",
//        "AT+KEY=1,0,\"a7070318\""
//    },
    {
        "AT+NIP=?",//设置为DHCP自动获取IP
        "OK=0",
        "AT+NIP=0"
    },
//    {
//        "AT+ATM=?",//1设置为手动 0 自动
//        "OK=1",
//        "AT+ATM=1"
//    },
    {
        "AT+ATRM=?",
        "OK=0,0,\"192.168.1.116\",4001",
        "AT+ATRM=0,0,\"192.168.1.116\",4001"
    }
//      ，
//      {
//      AT+PMTF
//      AT+Z
//      {
//          "AT+RSTF",//恢复出厂
//          "AT+Z",//复位
//      }
//      手机设置SSID
//      密码
//      加密方式
};

int TaskUsrtWifi(void)
{
    _SS
    WaitX(1000);
    breakENTMFlag = 0;
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
    //Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+ENTM\r");
    while (1)
    {
        WaitX(200);
        //配置 在用手机配置完后 通过AT指令做检查和完成后续配置
        if (1 == breakENTMFlag || 1 == WifiConfigFlag || 1 == WifiRESTFlag)
        {
            static int breakflag;
            breakflag = 0;
            for (static int i1 = 0; i1 < 10; i1++)//退出透明传输模式
            {
                static int i;
                i = 0;
                WaitX(1000);
                for (int i = 0; i < 50; i++)
                {
                    UsrtWifiAtRxBuffer[i] = 0;
                }
                Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+\r"); //空指令
                Sys_Printf(DEBUG_UARTNUM, (char *)"AT+\r\n"); //空指令
                for (; i < 5; ++i)
                {
                    WaitX(1000);
                    if (0x00 != UsrtWifiAtRxBuffer[0])
                    {
                        UsrtWifiAtRxBuffer[0] = 0;
                        Sys_Printf(DEBUG_UARTNUM, (char *)"%s", (UsrtWifiAtRxBuffer + 1));
                        if (0 == strncmp((char*)"OK", (char*)(UsrtWifiAtRxBuffer + 1), 2))
                        {
                            breakflag = 1;
                            breakENTMFlag = 2;
                            if (1 == WifiConfigFlag)
                            {
                                breakENTMFlag = 3;
                                WifiConfigFlag = 2;
                            }
                            if (1 == WifiRESTFlag)
                            {
                                breakENTMFlag = 2;
                            }
                        }
                        break;
                    }
                    if (4 == i)
                    {
                        for (int i = 0; i < 50; i++)
                        {
                            UsrtWifiAtRxBuffer[i] = 0;
                        }
                        Sys_Printf(UARTWIFIUARTNUM, (char *)"+++");
                        Sys_Printf(DEBUG_UARTNUM, (char *)"+++\r\n");
                    }

                }
                if (1 == breakflag)
                {
                    break;
                }
            }
        }
        if (1 == WifiRESTFlag && 2 == breakENTMFlag)
        {
            WifiRESTFlag = 2;
            WaitX(1000);
            Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+RSTF\r"); //恢复出厂设置
            Sys_Printf(DEBUG_UARTNUM, (char *)"AT+RSTF\r"); //恢复出厂设置

            WaitX(1000);
            Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+Z\r"); //复位
            Sys_Printf(DEBUG_UARTNUM, (char *)"AT+Z\r"); //复位
        }
        if (2 == WifiConfigFlag && 2 == WifiRESTFlag && 3 == breakENTMFlag)
        {
            WifiConfigFlag = 3;
            for (static int i2 = 0; i2 < CONFIGSUMNUM; ++i2)
            {
                WaitX(1000);
                {
                    static int breakflag;
                    breakflag = 0;
                    for (static int i1 = 0; i1 < 10; i1++)
                    {
                        static int i;
                        i = 0;
                        WaitX(1000);
                        UsrtWifiAtRxBuffer[0] = 0;
                        for (int i = 0; i < 50; i++)
                        {
                            UsrtWifiAtRxBuffer[i] = 0;
                        }
                        Sys_Printf(UARTWIFIUARTNUM, (char *)"%s\r", ATCommandList[i2][0]);
                        Sys_Printf(DEBUG_UARTNUM, (char *)"%s\r\n", ATCommandList[i2][0]);
                        for (; i < 5; ++i)
                        {
                            WaitX(1000);
                            if (0x00 != UsrtWifiAtRxBuffer[0])
                            {
                                UsrtWifiAtRxBuffer[0] = 0;
                                Sys_Printf(DEBUG_UARTNUM, (char *)"%s", (UsrtWifiAtRxBuffer + 1));
                                if (0 == strncmp(ATCommandList[i2][1], (char*)(UsrtWifiAtRxBuffer + 1), strlen(ATCommandList[i2][1])))
                                {
                                    breakflag = 1;
                                }
                                else
                                {   for (int i = 0; i < 50; i++)
                                    {
                                        UsrtWifiAtRxBuffer[i] = 0;
                                    }
                                    Sys_Printf(UARTWIFIUARTNUM, (char *)"%s\r", ATCommandList[i2][2]);
                                }
                                break;
                            }
                        }
                        if (1 == breakflag)
                        {
                            break;
                        }
                    }
                }
            }
            WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+PMTF\r"); //存到FLASH
            WaitX(1000); Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+Z\r"); //复位
            breakENTMFlag = 0;
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
        //RunTaskA(task_led, 0);
        RunTaskA(task_rtc, 1);
        RunTaskA(TaskRelay, 2);
        RunTaskA(TaskUsrtWifi, 3);
    }
}
