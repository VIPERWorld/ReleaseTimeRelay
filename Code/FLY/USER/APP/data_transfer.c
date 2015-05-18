#include "data_transfer.h"
#include "sys.h"
#include "usr_usart.h"
#include "bak.h"

#define DATA_TRANSFER_USE_USART
#define SUM_CHECK_HOLD 0//1 OPEN 0 CLOSE
void Data_Send_Check(u16 check);
struct DATA_TRANSFER_SWITCH Ex_ON_OFF, Send;
u16 AbsoluteOpticalEncoder_VAL = 0;
u8 RelayStata;
u16 AbsoluteOpticalEncoder_Apart[8] =
{
    30, 60, 90, 120,
    150, 180, 210, 360
};


void Ex_Anl(void)
{
    for (int i = 0; i < 8; ++i)
    {
        if (AbsoluteOpticalEncoder_VAL < AbsoluteOpticalEncoder_Apart[i])
        {
            RelayStata = i;
            break;
        }
    }
    Sys_Printf(Printf_USART, "\r\nAbsoluteOpticalEncoder_VAL:%d", AbsoluteOpticalEncoder_VAL);
    Sys_Printf(Printf_USART, "\r\nRelayStata:%d", RelayStata);
}

void Data_Receive_Anl(u8 *data_buf, u8 num)
{
    vs16 rc_value_temp;
    u8 sum = 0;
    Sys_sPrintf(Printf_USART, data_buf, num);
    for (u8 i = 0; i < (num - 1); i++)
        sum += *(data_buf + i);
#if   SUM_CHECK_HOLD
    if (!(sum == *(data_buf + num - 1)))       return; //sum
#endif
    if (!(*(data_buf) == 0xAA && *(data_buf + 1) == 0xAF))     return; //
    if (*(data_buf + 2) == 0X10)                        //PID1
    {
        AbsoluteOpticalEncoder_VAL = *(data_buf + 4);
    }
		if (*(data_buf + 2) == 0X11)                        //PID1
    {
			if((*(data_buf + 4)<8)&&(*(data_buf + 4)>0))
        AbsoluteOpticalEncoder_Apart[*(data_buf + 4)] = *(data_buf + 5);
    }
    Ex_Anl();
}

void Data_Exchange(void)
{
#ifdef DATA_TRANSFER_USE_SPI_NRF
    Nrf_Check_Event();
    u8 sta = Nrf_Get_FIFOSta();
    if ((sta & (1 << 4)) == 0)
        return;
#endif
}

