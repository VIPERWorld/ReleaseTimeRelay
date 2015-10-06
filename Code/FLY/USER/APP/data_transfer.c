#include "data_transfer.h"
#include "sys.h"
#include "usr_usart.h"
#include "bak.h"
/*C/C++*/
#include "stdlib.h"
#include "string.h"
/*USAR*/
#include "minos_delay.h"
#include "aes.h"

/*Define*/

#define DATA_TRANSFER_USE_USART
#define SUM_CHECK_HOLD 0//1 OPEN 0 CLOSE
#define SIMPLEUNLOCK 0
/*Static Fun*/
//static void Data_Send_Check(u16 check);
/*Variable*/
//struct DATA_TRANSFER_SWITCH Ex_ON_OFF, Send;

int AbsoluteOpticalEncoder_VAL     = 0;//绝对式光电编码器
//s16 AbsoluteOpticalEncoder_LastVAL = 0;//绝对式光电编码器
u8 RelayStata[AbsoluteOpticalEncoderNUM] = {0}; //继电器状态
_Uu32u8 TimeUnlock;//锁定时间

int TimeUnlockFlag = 0;// 时间解锁标志
int WifiConfigFlag = 0;//wifi配置命令标志
int WifiRESTFlag   = 0;//wifi重置命令标志
int breakENTMFlag  = 0;//跳出透明传输标志

u16 AbsoluteOpticalEncoder_Apart[AbsoluteOpticalEncoderNUM][2] =//继电器阈值
{
    {0  , 30 },
    {30 , 60 },
    {60 , 90 },
    {90 , 120},
    {120, 150},
    {150, 180},
    {180, 210},
    {210, 360},
    {210, 360}
};
union
{
    u8 u8[12];
    u32 u32[3];
} ChipUniqueID;
#include "rtc.h"
void Ex_Anl(u8 *data_buf)
{
    u8 LastUnlockKey[16];
    static u8 KeyUnlockFlag = 0;//控制解锁标志
    switch (*(data_buf + 2))
    {
    case 0X10://输入编码器角度
    {
        AbsoluteOpticalEncoder_VAL = *(data_buf + 4);
        break;
    }
    case 0X11://修改单个继电器阈值
    {
        if (*(data_buf + 4) < 16)
            AbsoluteOpticalEncoder_Apart[*(data_buf + 4) / 2][*(data_buf + 4) % 2] = *(data_buf + 5);
        Sys_Printf(DEBUG_UARTNUM, "\r\nAbsoluteOpticalEncoder_Apart:\r\n");
        for (int i = 0; i < 8; i++)Sys_Printf(Printf_USART, " %d", AbsoluteOpticalEncoder_Apart[i]);
        break;
    }
    case 0X12://设定TimeUnlock锁定时间
    {
#if SIMPLEUNLOCK
        if (KeyUnlockFlag)
#endif
        {
            KeyUnlockFlag = 0;
            TimeUnlock.u8[3] = *(data_buf + 4);
            TimeUnlock.u8[2] = *(data_buf + 5);
            TimeUnlock.u8[1] = *(data_buf + 6);
            TimeUnlock.u8[0] = *(data_buf + 7);
        }
        Data_Save(3);
        Sys_Printf(DEBUG_UARTNUM, "\r\nTimeUnlock:%d", TimeUnlock.u32);
        break;
    }
    case 0X13://得到解锁初始密钥
    {
        srand(SysTick_Clock());
        union
        {
            u8 u8[2];
            u16 u16;
        } TimeRand;
        TimeRand.u16 = rand();
        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n%d", TimeRand.u16);
        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n%d", TimeRand.u8[0]);
        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n%d", TimeRand.u8[1]);

        unsigned char chainCipherBlock[16], dat[16] = {0};
        for (int i = 0; i < 12; ++i)dat[i] = ChipUniqueID.u8[i];
        for (int i = 0; i < 2; ++i)dat[i + 12] = TimeRand.u8[i];
        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n");
        for (int i = 0; i < 16; ++i)Sys_Printf(UARTWIFIUARTNUM, (char *)"%2X ", dat[i]);

        for (int i = 0; i < 32; i++) AES_Key_Table[i] = i; //做运算之前先要设置好密钥,这里只是设置密钥的DEMO

        memset(chainCipherBlock, 0x00, sizeof(chainCipherBlock));
        aesEncInit();//在执行加密初始化之前可以为AES_Key_Table赋值有效的密码数据.
        aesEncrypt(dat, chainCipherBlock);//AES加密,数组dat里面的新内容就是加密后的数据.
        for (int i = 0; i < 16; ++i)LastUnlockKey[i] = dat[i];

        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n"); for (int i = 0; i < 16; ++i)Sys_Printf(UARTWIFIUARTNUM, (char *)"%2X ", dat[i]);

        memset(chainCipherBlock, 0x00, sizeof(chainCipherBlock)); //这里要重新初始化清空
        aesDecInit();//在执行解密初始化之前可以为AES_Key_Table赋值有效的密码数据
        aesDecrypt(dat, chainCipherBlock);//AES解密，密文数据存放在dat里面，经解密就能得到之前的明文.

        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n"); for (int i = 0; i < 16; ++i)Sys_Printf(UARTWIFIUARTNUM, (char *)"%2X ", dat[i]);
        break;
    }
    case 0X14://得到IC ID
    {
        ChipUniqueID.u32[2] = *(__IO u32 *)(0X1FFFF7E8); // 低字节
        ChipUniqueID.u32[1] = *(__IO u32 *)(0X1FFFF7EC); //
        ChipUniqueID.u32[0] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\nChipUniqueID: %8X %8X %8X", ChipUniqueID.u32[0], ChipUniqueID.u32[1], ChipUniqueID.u32[2]);
        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\nChipUniqueID: %2X%2X%2X%2X %2X%2X%2X%2X %2X%2X%2X%2X", ChipUniqueID.u8[0], ChipUniqueID.u8[1], ChipUniqueID.u8[2], ChipUniqueID.u8[3], ChipUniqueID.u8[4], ChipUniqueID.u8[5], ChipUniqueID.u8[6], ChipUniqueID.u8[7], ChipUniqueID.u8[8], ChipUniqueID.u8[9], ChipUniqueID.u8[10], ChipUniqueID.u8[11]);
        break;
    }
    case 0X15://输入解锁密钥 解锁控制权
    {
        unsigned char chainCipherBlock[16], dat[16] = {0};
        for (int i = 0; i < 16; ++i)dat[i] = *(data_buf + 4 + i);
        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n"); for (int i = 0; i < 16; ++i)Sys_Printf(UARTWIFIUARTNUM, (char *)"%2X ", dat[i]);

        for (int i = 0; i < 32; i++) AES_Key_Table[i] = i; //做运算之前先要设置好密钥,这里只是设置密钥的DEMO

        memset(chainCipherBlock, 0x00, sizeof(chainCipherBlock)); //这里要重新初始化清空
        aesDecInit();//在执行解密初始化之前可以为AES_Key_Table赋值有效的密码数据
        aesDecrypt(dat, chainCipherBlock);//AES解密，密文数据存放在dat里面，经解密就能得到之前的明文.

        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n"); for (int i = 0; i < 16; ++i)Sys_Printf(UARTWIFIUARTNUM, (char *)"%2X ", dat[i]);
        Sys_Printf(UARTWIFIUARTNUM, (char *)"\r\n"); for (int i = 0; i < 16; ++i)Sys_Printf(UARTWIFIUARTNUM, (char *)"%2X ", LastUnlockKey[i]);
        {
            int i;
            for ( i = 0; i < 16; ++i)
                if (dat[i] != LastUnlockKey[i])
                    break;
            if (i == 16)
                KeyUnlockFlag = 1;
            else
                KeyUnlockFlag = 0;
        }
        break;
    }
		case 0x16:
		{
			RTC_Set(
			2000+*(data_buf + 4), 
			*(data_buf + 5),
			*(data_buf + 6),
			*(data_buf + 7),
			*(data_buf + 8),
			00);
		}
    }
}

void Data_Receive_Anl(u8 *data_buf, u8 num)
{
    vs16 rc_value_temp;
    u8 sum = 0;
    //Sys_sPrintf(Printf_USART, data_buf, num);
    for (u8 i = 0; i < (num - 1); i++)
        sum += *(data_buf + i);
#if   SUM_CHECK_HOLD
    if (!(sum == *(data_buf + num - 1)))       return; //sum
#endif
    if (!(*(data_buf) == 0xAA && *(data_buf + 1) == 0xAF))     return; //
    Ex_Anl(data_buf);
}
#define FRAMEHEADER1 0x5A//双帧头
#define FRAMEHEADER2 0xA5//次帧头
void Data_Send_EncoderApartStatus(int i, int j)
{
    u8 _cnt = 0;
    u8 data_to_send[16];
    data_to_send[_cnt++] = FRAMEHEADER1;//0
    data_to_send[_cnt++] = FRAMEHEADER2;//1
    data_to_send[_cnt++] = 5;//长度//2
    data_to_send[_cnt++] = 0x82;//数据下发//3
    vs16 _temp;
    for (int k = 0; k < 2; k++)
    {
            _cnt = 5;
					  data_to_send[4] = j;//4
            data_to_send[_cnt++] = ((i + 1) << 4) + k; //5
            _temp = AbsoluteOpticalEncoder_Apart[i][k];
            data_to_send[_cnt++] = BYTE1(_temp); 
			      data_to_send[_cnt++] = BYTE0(_temp);
            Sys_sPrintf(USARTSCREEN, data_to_send, _cnt);
    }
}
void Data_Send_VAL(u16 addr, u16 val)
{
    u8 _cnt = 0;
    u8 data_to_send[16];
    data_to_send[_cnt++] = FRAMEHEADER1;//0
    data_to_send[_cnt++] = FRAMEHEADER2;//1
    data_to_send[_cnt++] = 5;//长度//2
    data_to_send[_cnt++] = 0x82;//数据下发//3
    vs16 _temp;
    _temp = addr;
    data_to_send[_cnt++] = BYTE1(_temp);;
    data_to_send[_cnt++] = BYTE0(_temp);; //5
    _temp = val;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    Sys_sPrintf(USARTSCREEN, data_to_send, _cnt);
}
void Data_Send_VAL64(u16 addr, u32 val)
{
    u8 _cnt = 0;
    u8 data_to_send[16];
    data_to_send[_cnt++] = FRAMEHEADER1;//0
    data_to_send[_cnt++] = FRAMEHEADER2;//1
    data_to_send[_cnt++] = 7;//长度//2
    data_to_send[_cnt++] = 0x82;//数据下发//3
    vs16 _temp;
    _temp = addr;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp); //5
    vs32 _temp64;
	  _temp64= val;
//	  data_to_send[_cnt++] = BYTE7(_temp64);
//    data_to_send[_cnt++] = BYTE6(_temp64);
//	  data_to_send[_cnt++] = BYTE5(_temp64);
//    data_to_send[_cnt++] = BYTE4(_temp64);
	  data_to_send[_cnt++] = BYTE3(_temp64);
    data_to_send[_cnt++] = BYTE2(_temp64);
	  data_to_send[_cnt++] = BYTE1(_temp64);
    data_to_send[_cnt++] = BYTE0(_temp64);
	
    Sys_sPrintf(USARTSCREEN, data_to_send, _cnt);
}
int turn(int in, int around)
{
    if (in < 0)
        return around + in;
    else
        return in;
}
void UsrtScreenAnl(u8 *data_buf)
{
    switch ((((vs16)(*(data_buf + 4)) << 8) & 0x0f) | *(data_buf + 5))
    {
    case 0x0010: {AbsoluteOpticalEncoder_Apart[0][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(0, 1); break;}
    case 0x0011: {AbsoluteOpticalEncoder_Apart[0][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(0, 1); break;}
    case 0x0020: {AbsoluteOpticalEncoder_Apart[1][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(1, 1); break;}
    case 0x0021: {AbsoluteOpticalEncoder_Apart[1][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(1, 1); break;}
    case 0x0030: {AbsoluteOpticalEncoder_Apart[2][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(2, 1); break;}
    case 0x0031: {AbsoluteOpticalEncoder_Apart[2][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(2, 1); break;}
    case 0x0040: {AbsoluteOpticalEncoder_Apart[3][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(3, 1); break;}
    case 0x0041: {AbsoluteOpticalEncoder_Apart[3][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(3, 1); break;}
    case 0x0050: {AbsoluteOpticalEncoder_Apart[4][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(4, 1); break;}
    case 0x0051: {AbsoluteOpticalEncoder_Apart[4][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(4, 1); break;}
    case 0x0060: {AbsoluteOpticalEncoder_Apart[5][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(5, 1); break;}
    case 0x0061: {AbsoluteOpticalEncoder_Apart[5][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(5, 1); break;}
    case 0x0070: {AbsoluteOpticalEncoder_Apart[6][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(6, 1); break;}
    case 0x0071: {AbsoluteOpticalEncoder_Apart[6][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(6, 1); break;}
    case 0x0080: {AbsoluteOpticalEncoder_Apart[7][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(7, 1); break;}
    case 0x0081: {AbsoluteOpticalEncoder_Apart[7][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(7, 1); break;}
    case 0x0090: {AbsoluteOpticalEncoder_Apart[8][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(8, 1); break;}
    case 0x0091: {AbsoluteOpticalEncoder_Apart[8][1] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8)); Data_Send_EncoderApartStatus(8, 1); break;}
    case 0x0202: WifiRESTFlag = 1; break;
    case 0x0203: WifiConfigFlag = 1; break;
    default: break;
    }
    Data_Save(2);

    Sys_Printf(USART2, "\r\n %d", ((((vs16)(*(data_buf + 4)) << 8)&0xff)  | *(data_buf + 5)));
    //Sys_Printf(USART2, "\r\n %d", *(data_buf + 5));

    return;
}
// void UsrtScreenAnl(u8 *data_buf)
// {
//     switch ((u16)((vs16)(*(data_buf + 4)) << 8) | *(data_buf + 5))
//     {
//     case 0x0001:
//         AbsoluteOpticalEncoder_Apart[0][1] = AbsoluteOpticalEncoder_Apart[1][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8));
//         //if (AbsoluteOpticalEncoder_Apart[1][0] > AbsoluteOpticalEncoder_Apart[1][1]) AbsoluteOpticalEncoder_Apart[0][1] = AbsoluteOpticalEncoder_Apart[1][0] = AbsoluteOpticalEncoder_Apart[1][1];
//         break;
//     case 0x0002: AbsoluteOpticalEncoder_Apart[1][1] = AbsoluteOpticalEncoder_Apart[2][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8));
//         if (AbsoluteOpticalEncoder_Apart[2][0] > AbsoluteOpticalEncoder_Apart[2][1])AbsoluteOpticalEncoder_Apart[1][1] = AbsoluteOpticalEncoder_Apart[2][0] = AbsoluteOpticalEncoder_Apart[2][1];
//         if (AbsoluteOpticalEncoder_Apart[1][0] > AbsoluteOpticalEncoder_Apart[1][1])AbsoluteOpticalEncoder_Apart[1][1] = AbsoluteOpticalEncoder_Apart[2][0] = AbsoluteOpticalEncoder_Apart[0][1];
//         break;
//     case 0x0003: AbsoluteOpticalEncoder_Apart[2][1] = AbsoluteOpticalEncoder_Apart[3][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8));
//         if (AbsoluteOpticalEncoder_Apart[3][0] > AbsoluteOpticalEncoder_Apart[3][1])AbsoluteOpticalEncoder_Apart[2][1] = AbsoluteOpticalEncoder_Apart[3][0] = AbsoluteOpticalEncoder_Apart[3][1];
//         if (AbsoluteOpticalEncoder_Apart[2][0] > AbsoluteOpticalEncoder_Apart[2][1])AbsoluteOpticalEncoder_Apart[2][1] = AbsoluteOpticalEncoder_Apart[3][0] = AbsoluteOpticalEncoder_Apart[1][1];
//         break;
//     case 0x0004: AbsoluteOpticalEncoder_Apart[3][1] = AbsoluteOpticalEncoder_Apart[4][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8));
//         if (AbsoluteOpticalEncoder_Apart[4][0] > AbsoluteOpticalEncoder_Apart[4][1])AbsoluteOpticalEncoder_Apart[3][1] = AbsoluteOpticalEncoder_Apart[4][0] = AbsoluteOpticalEncoder_Apart[4][1];
//         if (AbsoluteOpticalEncoder_Apart[3][0] > AbsoluteOpticalEncoder_Apart[3][1])AbsoluteOpticalEncoder_Apart[3][1] = AbsoluteOpticalEncoder_Apart[4][0] = AbsoluteOpticalEncoder_Apart[2][1];
//         break;
//     case 0x0005: AbsoluteOpticalEncoder_Apart[4][1] = AbsoluteOpticalEncoder_Apart[5][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8));
//         if (AbsoluteOpticalEncoder_Apart[5][0] > AbsoluteOpticalEncoder_Apart[5][1])AbsoluteOpticalEncoder_Apart[4][1] = AbsoluteOpticalEncoder_Apart[5][0] = AbsoluteOpticalEncoder_Apart[5][1];
//         if (AbsoluteOpticalEncoder_Apart[4][0] > AbsoluteOpticalEncoder_Apart[4][1])AbsoluteOpticalEncoder_Apart[4][1] = AbsoluteOpticalEncoder_Apart[5][0] = AbsoluteOpticalEncoder_Apart[3][1];
//         break;
//     case 0x0006: AbsoluteOpticalEncoder_Apart[5][1] = AbsoluteOpticalEncoder_Apart[6][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8));
//         if (AbsoluteOpticalEncoder_Apart[6][0] > AbsoluteOpticalEncoder_Apart[6][1])AbsoluteOpticalEncoder_Apart[5][1] = AbsoluteOpticalEncoder_Apart[6][0] = AbsoluteOpticalEncoder_Apart[6][1];
//         if (AbsoluteOpticalEncoder_Apart[5][0] > AbsoluteOpticalEncoder_Apart[5][1])AbsoluteOpticalEncoder_Apart[5][1] = AbsoluteOpticalEncoder_Apart[6][0] = AbsoluteOpticalEncoder_Apart[4][1];
//         break;
//     case 0x0007: AbsoluteOpticalEncoder_Apart[6][1] = AbsoluteOpticalEncoder_Apart[7][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8));
//         if (AbsoluteOpticalEncoder_Apart[7][0] > AbsoluteOpticalEncoder_Apart[7][1])AbsoluteOpticalEncoder_Apart[6][1] = AbsoluteOpticalEncoder_Apart[7][0] = AbsoluteOpticalEncoder_Apart[7][1];
//         if (AbsoluteOpticalEncoder_Apart[6][0] > AbsoluteOpticalEncoder_Apart[6][1])AbsoluteOpticalEncoder_Apart[6][1] = AbsoluteOpticalEncoder_Apart[7][0] = AbsoluteOpticalEncoder_Apart[5][1];
//         break;
//     case 0x0008: AbsoluteOpticalEncoder_Apart[7][1] = AbsoluteOpticalEncoder_Apart[0][0] = ((u16)((vs16)(*(data_buf + 7)) << 8) | *(data_buf + 8));
//         //if (AbsoluteOpticalEncoder_Apart[0][0] > AbsoluteOpticalEncoder_Apart[0][1])AbsoluteOpticalEncoder_Apart[7][1] = AbsoluteOpticalEncoder_Apart[0][0] = AbsoluteOpticalEncoder_Apart[0][1];
//         if (AbsoluteOpticalEncoder_Apart[7][0] > AbsoluteOpticalEncoder_Apart[7][1])AbsoluteOpticalEncoder_Apart[7][1] = AbsoluteOpticalEncoder_Apart[0][0] = AbsoluteOpticalEncoder_Apart[6][1];

// break;
//     case 0x0050: WifiRESTFlag = 1; break;
//     case 0x0051: WifiConfigFlag = 1; break;
//     default: break;
//     }
//      Data_Save(2);
//     Data_Send_EncoderApartStatus();
//     Sys_Printf(USART2, "\r\n %d", ((u16)((vs16)(*(data_buf + 4)) << 8) | *(data_buf + 5)));
//     return;
// }
void Data_Exchange(void)
{
#ifdef DATA_TRANSFER_USE_SPI_NRF
    Nrf_Check_Event();
    u8 sta = Nrf_Get_FIFOSta();
    if ((sta & (1 << 4)) == 0)
        return;
#endif
}

