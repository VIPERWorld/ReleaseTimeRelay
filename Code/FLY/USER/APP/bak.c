/*
小容量产品是指闪存存储器容量在16K至32K字节之间的STM32F101xx、STM32F102xx和
STM32F103xx微控制器。
中容量产品是指闪存存储器容量在64K至128K字节之间的STM32F101xx、STM32F102xx和
STM32F103xx微控制器。
大容量产品是指闪存存储器容量在256K至512K字节之间的STM32F101xx和STM32F103xx微控
制器。
互联型产品是指STM32F105xx和STM32F107xx微控制器。

BKP特性
● 20字节数据后备寄存器(中容量和小容量产品)，或84字节数据后备寄存器(大容量和互联型
产品)

BK1用作保留字段 作为数据有没有丢失的判断字节
在RTC中也用了此字段
default:BKP1==0x5050
*/
#include "bak.h"
#include "stmflash.h"
#include "control.h"
#include "data_transfer.h"
#include "usr_usart.h"
void DataSaveInit(void)
{
    RCC->APB1ENR |= 1 << 28;//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC->APB1ENR |= 1 << 27;//RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
    PWR->CR |= 1 << 8;      //DISENABLE写保护
    return;
}
void Data_Save(u8 select)
{
    switch (select)
    {
    case 1:
        Data_Send_VAL(0x0001, AbsoluteOpticalEncoder_VAL);
        BKP_WriteBackupRegister(BKP_DR10 , (u16)(AbsoluteOpticalEncoder_VAL    ));
        break;
    case 2:
        break;
    case 3:
        BKP_WriteBackupRegister(BKP_DR8, (u16)(TimeUnlock.u16[0]));
        BKP_WriteBackupRegister(BKP_DR9, (u16)(TimeUnlock.u16[1]));
    }
    //BKP_WriteBackupRegister(BKP_DR1, 0x5050);
    return;
}
int Data_Read(void)
{
    if (0x5050 == ((u16)(BKP_ReadBackupRegister(BKP_DR1))))
    {
        AbsoluteOpticalEncoder_VAL         =  ((s16)(BKP_ReadBackupRegister(BKP_DR10)));
        TimeUnlock.u16[0]                  =  ((u16)(BKP_ReadBackupRegister(BKP_DR8)));
        TimeUnlock.u16[1]                  =  ((u16)(BKP_ReadBackupRegister(BKP_DR9)));
    }
    else
        return 1;
    return 0;
};

// #define SIZE 18
#define SIZE (22)
#define FLASH_SAVE_ADDR   0X08019200      //设置FLASH 保存地址(必须为偶数)
#define FLASH_SAVE_ADDR2  0X08019600      //设置FLASH 保存地址(必须为偶数)
#define FLASH_SAVE_ADDR3  0X08018800      //设置FLASH 保存地址(必须为偶数)


int CurAddr = 0;
void StmFlash_Save(u8 Page)
{
    u16 StmFlash_Buffer[SIZE] = {0};
    if (1 == Page)//时间
    {
        int i = 1;
        StmFlash_Buffer[i++] = ZipTime.u16[0];
        StmFlash_Buffer[i++] = ZipTime.u16[1];
        u16 SumCheck = 0;
        for (int j = 1; j < SIZE; ++j)
        {
            SumCheck += StmFlash_Buffer[j];
        }
        StmFlash_Buffer[0] = SumCheck;
        STMFLASH_Write(FLASH_SAVE_ADDR, (u16 *)StmFlash_Buffer, SIZE);
    }
    if (2 == Page)//参数
    {
        int i = 1;
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[0][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[0][1]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[1][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[1][1]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[2][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[2][1]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[3][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[3][1]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[4][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[4][1]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[5][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[5][1]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[6][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[6][1]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[7][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[7][1]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[8][0]);
        StmFlash_Buffer[i++] = (u16)(AbsoluteOpticalEncoder_Apart[8][1]);
        u16 SumCheck = 0;
        for (int j = 1; j < SIZE; ++j)
        {
            SumCheck += StmFlash_Buffer[j];
        }
        StmFlash_Buffer[0] = SumCheck;
        STMFLASH_Write(FLASH_SAVE_ADDR2, (u16 *)StmFlash_Buffer, SIZE);
    }
    if (3 == Page)//保存密钥
    {
        int i = 1;
        for (int j = 0; j < KEYLISTLEN; ++j)
        {
            StmFlash_Buffer[i++] = (u16)(WORD0(TimeKeyWordList[j]));
            StmFlash_Buffer[i++] = (u16)(WORD1(TimeKeyWordList[j]));
        }
        u16 SumCheck = 0;
        for (int j = 1; j < SIZE; ++j)
        {
            SumCheck += StmFlash_Buffer[j];
        }
        StmFlash_Buffer[0] = SumCheck;
        STMFLASH_Write(FLASH_SAVE_ADDR3, (u16 *)StmFlash_Buffer, SIZE);
    }
}
int StmFlash_Read(void)
{
    u16 StmFlash_Buffer[SIZE];
    STMFLASH_Read(FLASH_SAVE_ADDR, (u16 *)StmFlash_Buffer, SIZE);
    u16 SumCheck = 0;
    for (int j = 1; j < SIZE; ++j)
    {
        SumCheck += StmFlash_Buffer[j];
    }
    if (SumCheck == StmFlash_Buffer[0])//在读取前怎加和校验
    {
        int i = 1;
        ZipTime.u16[0] = ((u16)(StmFlash_Buffer[i++]));
        ZipTime.u16[1] = ((u16)(StmFlash_Buffer[i++]));
        TimeUnZip(ZipTime.u32);
    }
		
		STMFLASH_Read(FLASH_SAVE_ADDR2, (u16 *)StmFlash_Buffer, SIZE);
    SumCheck = 0;
    for (int j = 1; j < SIZE; ++j)
    {
        SumCheck += StmFlash_Buffer[j];
    }
    if (SumCheck == StmFlash_Buffer[0])//在读取前怎加和校验
    {
        int i = 1;
        AbsoluteOpticalEncoder_Apart[0][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[0][1] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[1][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[1][1] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[2][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[2][1] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[3][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[3][1] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[4][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[4][1] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[5][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[5][1] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[6][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[6][1] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[7][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[7][1] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[8][0] = ((u16)(StmFlash_Buffer[i++]));
        AbsoluteOpticalEncoder_Apart[8][1] = ((u16)(StmFlash_Buffer[i++]));
    }
		
		STMFLASH_Read(FLASH_SAVE_ADDR3, (u16 *)StmFlash_Buffer, SIZE);
    SumCheck = 0;
    for (int j = 1; j < SIZE; ++j)
    {
        SumCheck += StmFlash_Buffer[j];
    }
    if (SumCheck == StmFlash_Buffer[0])//在读取前怎加和校验
    {
        int i = 1;
        for (int j = 0; j < KEYLISTLEN; ++j)
        {
					   WORD0(TimeKeyWordList[j]) = ((u16)(StmFlash_Buffer[i++]));
             WORD1(TimeKeyWordList[j]) = ((u16)(StmFlash_Buffer[i++]));
        }
    }
    return 1;
}

