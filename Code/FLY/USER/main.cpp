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
#include "data_transfer.h"

#include "bak.h"
#include "pvd.h"
void SYS_INIT(void)
{
	/***延时初始化***/
	delay_init();
	/***中断初始化***/
	NVIC_Configuration();
	uart_init (38400);
	uart2_init(115200);
	uart3_init(115200);
	//PvdInit();
	Sys_Printf(USART1, (char *)"USART USARTSCREEN\r\n");
	Sys_Printf(USART2, (char *)"USART2 DEBUG_UARTNUM\r\n");
	Sys_Printf(USART3, (char *)"USART3 UARTWIFIUARTNUM\r\n");
	delay_ms(100);
}

#include "rtc.h"
#include "data_transfer.h"

void SendTime()//从串口发送时间(当天时间 剩余时间)
{
	Data_Send_VAL(0x0300, calendar.w_year);
	Data_Send_VAL(0x0301, calendar.w_month);
	Data_Send_VAL(0x0302, calendar.w_date);
	Data_Send_VAL(0x0303, calendar.hour);
	Data_Send_VAL(0x0304, calendar.min);
	Data_Send_VAL(0x0305, calendar.sec);

	DeathTime.w_year  = TimeUnlockEx.w_year  - calendar.w_year;
	DeathTime.w_month = TimeUnlockEx.w_month - calendar.w_month;
	DeathTime.w_date  = TimeUnlockEx.w_date  - calendar.w_date;
	DeathTime.hour    = TimeUnlockEx.hour    - calendar.hour;
	DeathTime.min     = TimeUnlockEx.min     - calendar.min;
	DeathTime.sec     = TimeUnlockEx.sec     - calendar.sec;

	Data_Send_VAL(0x0310, TimeUnlockEx.w_year);
	Data_Send_VAL(0x0311, TimeUnlockEx.w_month);
	Data_Send_VAL(0x0312, TimeUnlockEx.w_date);
	Data_Send_VAL(0x0313, TimeUnlockEx.hour);
	Data_Send_VAL(0x0314, TimeUnlockEx.min);
	Data_Send_VAL(0x0315, TimeUnlockEx.sec);
}

int IsUnlock()//0上锁 1解锁
{
	if (DeathTime.w_year > 0)
		return 1;
	else if (DeathTime.w_year < 0)
		return 0;
	else
	{
		if (DeathTime.w_month > 0)
			return 1;
		else if (DeathTime.w_month < 0)
			return 0;
		else
		{
			if (DeathTime.w_date > 0)
				return 1;
			else if (DeathTime.w_date < 0)
				return 0;
			else
			{
				if (DeathTime.hour > 0)
					return 1;
				else if (DeathTime.hour < 0)
					return 0;
				else
				{
					if (DeathTime.min > 0)
						return 1;
					else if (DeathTime.min < 0)
						return 0;
					else
					{
						if (DeathTime.sec > 0)
							return 1;
						else
							return 0;
					}
				}
			}
		}
	}
}
u16 task_rtc(void)
{
	_SS
	DataSaveInit();
	Data_Read();
	TimeUnlock.u32 = 0x7fffffff;
	if (RTC_Init() == 0)
	{
		Sys_Printf(DEBUG_UARTNUM, (char *)"\r\n RTC ok");
	}
	else
	{
		Sys_Printf(DEBUG_UARTNUM, (char *)"\r\n RTC no");
	}

	while (1)
	{
		WaitX(1000);
		unsigned char time[24];
		get_time((u8 *)time);
		//Sys_Printf(DEBUG_UARTNUM, (char *)"\r\n RTC: %s",time);
		SendTime();
		if (1 == IsUnlock()) //判断是否解锁（是否还有剩余时间）
		{
			TimeUnlockFlag = 1;
		}
		else
		{
			TimeUnlockFlag = 0;
		}
	}
	_EE
}

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
	RELAY8_OFF;
	RELAY9_OFF;
	RELAY10_OFF;
}
void RelayControlOn(void)
{
	RELAY0_ON;
	RELAY1_ON;
	RELAY2_ON;
	RELAY3_ON;
	RELAY4_ON;
	RELAY5_ON;
	RELAY6_ON;
	RELAY7_ON;
	RELAY8_ON;
	RELAY9_ON;
	RELAY10_ON;
}

#include "EXTI.h"
int TaskRelay(void)
{
	_SS
	RELAY0_INIT; RELAY0_OFF;
	RELAY1_INIT; RELAY1_OFF;
	RELAY2_INIT; RELAY2_OFF;
	RELAY3_INIT; RELAY3_OFF;
	RELAY4_INIT; RELAY4_OFF;
	//RELAY5_INIT; RELAY5_OFF;
//	RELAY6_INIT; RELAY6_OFF;
	//RELAY7_INIT; RELAY7_OFF;
//	RELAY8_INIT; RELAY8_OFF;
//	RELAY9_INIT; RELAY9_OFF;
	RELAY10_INIT; RELAY10_OFF;





	WaitX(1000);

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO, ENABLE ); GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	EXTI_Configuration(GPIOB, GPIO_Pin_6, 0);// 表示作为外部中断 0下降沿触发
	EXTI_Configuration(GPIOB, GPIO_Pin_7, 0);// 表示作为外部中断 0下降沿触发
	EXTI_Configuration(GPIOB, GPIO_Pin_8, 0);// 表示作为外部中断 0下降沿触发 1上升沿
	EXTI_Configuration(GPIOB, GPIO_Pin_9, 2);// 表示作为外部中断 2上升和下降沿触发
	EXTI_NVIC_Configuration(6, 2, 1, 1);//-
	EXTI_NVIC_Configuration(7, 2, 1, 1);//+
	EXTI_NVIC_Configuration(8, 2, 1, 1);//0
	EXTI_NVIC_Configuration(9, 2, 1, 1);//光电开关

	while (1)
	{
		WaitX(10);
		if (TimeUnlockFlag)//判断解锁标志（剩余时间标志）
		{
			for (int i = 0; i < AbsoluteOpticalEncoderNUM ; ++i)
			{
				if (AbsoluteOpticalEncoder_Apart[i][0] < AbsoluteOpticalEncoder_Apart[i][1])
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
				}
				else
				{
					if (
					    (AbsoluteOpticalEncoder_VAL < AbsoluteOpticalEncoder_Apart[i][0]) &&
					    (AbsoluteOpticalEncoder_VAL > AbsoluteOpticalEncoder_Apart[i][1])
					)
					{
						RelayStata[i] = 1;
					}
					else
					{
						RelayStata[i] = 0;
					}
				}
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



#define CONFIGSUMNUM 3
const char *ATCommandList[CONFIGSUMNUM][3] = {
	{
		"AT+WPRT=?",//网络类型
		"OK=0",
		"AT+WPRT=0"
	},
	{
		"AT+NIP=?",//设置为DHCP自动获取IP
		"OK=0",
		"AT+NIP=0"
	},
	{
		"AT+ATRM=?",
		"OK=0,0,\"192.168.31.158\",8080",
		"AT+ATRM=0,0,\"192.168.31.158\",8080"
	}
};

int TaskUsrtWifi(void)
{
	_SS
	WaitX(1000);
	breakENTMFlag = 0;
	while (1)
	{
		WaitX(200);
		//配置 在用手机配置完后 通过AT指令做检查和完成后续配置
		if (1 == breakENTMFlag ||
		        1 == WifiConfigFlag ||
		        1 == WifiRESTFlag
		   )//加入AT模式
		{
			static int breakflag;
			breakflag = 0;
			UsrtWifiENTMFlag = 1;
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
						if ((0 == strncmp((char*)"OK", (char*)(UsrtWifiAtRxBuffer + 1), 2)) ||
						        (0 == strncmp((char*)"ERR=-1", (char*)(UsrtWifiAtRxBuffer + 1), 6))
						   )
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
			UsrtWifiENTMFlag = 0;
		}
		if (1 == WifiRESTFlag && 2 == breakENTMFlag)//复位
		{
			WifiRESTFlag = 2;
			WaitX(1000);
			Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+RSTF\r"); //恢复出厂设置
			Sys_Printf(DEBUG_UARTNUM, (char *)"AT+RSTF\r"); //恢复出厂设置

			WaitX(1000);
			Sys_Printf(UARTWIFIUARTNUM, (char *)"AT+Z\r"); //复位
			Sys_Printf(DEBUG_UARTNUM, (char *)"AT+Z\r"); //复位
		}
		if (2 == WifiConfigFlag && 2 == WifiRESTFlag && 3 == breakENTMFlag)//保存参数
		{
			WifiConfigFlag = 3;
			UsrtWifiENTMFlag = 1;
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
								{	for (int i = 0; i < 50; i++)
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
			UsrtWifiENTMFlag = 0;
		}
	}
	_EE
}
void DisPlaySendLock(void)//发送指令返回锁定界面
{
	Data_Send_Reg2(0x03, 0x00);
	return;
}
void DisPlayGetCurPID_ID(void)
{
	DataGetReg(0x03, 0x02);
}

#define FREETIME 60 //定义当多久没操控触摸屏后退出控制界面(s)
int TaskControl(void)
{
	_SS
	static int i;
	StmFlash_Read();
	for ( i = 0; i < AbsoluteOpticalEncoderNUM; ++i)
	{
		WaitX(20);
		for (int j = 0; j < 2; ++j)
			Data_Send_EncoderApartStatus(i, j);
	}
	Data_Send_VAL(0x0001, AbsoluteOpticalEncoder_VAL);//发送屏幕默认值
	for (;;)
	{
		WaitX(1000);
		u16FreeTime++;
		if (u16FreeTime > FREETIME)
		{
			DisPlayGetCurPID_ID();//得到当前界面ID;
		}
		if (u16FreeTime > (FREETIME + 1))
		{
			if (9 == CurPID_ID)
			{
				//DisPlaySendLock();
				u16FreeTime = 0;
			}
			else
				u16FreeTime = u16FreeTime / 2;
		}
		{
			static int i;
			for ( i = 0; i < AbsoluteOpticalEncoderNUM; ++i)
			{
				WaitX(20);
				for (int j = 0; j < 2; ++j)
				{
					Data_Send_EncoderApartStatus(i, j);
				}
			}
			Data_Send_VAL(0x0001, AbsoluteOpticalEncoder_VAL);
		}//每隔一秒钟更新数据
	}
	_EE
}
int main(void)
{
	SYS_INIT();
	/***总循环***/
	while (1)
	{
		RunTaskA(task_rtc, 1);
		RunTaskA(TaskRelay, 2);
		RunTaskA(TaskUsrtWifi, 3);
		RunTaskA(TaskControl, 4);
	}
}
