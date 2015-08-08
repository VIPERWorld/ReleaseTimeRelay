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
//    RCC->APB1ENR |= 1 << 28;//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//    RCC->APB1ENR |= 1 << 27;//RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
//    PWR->CR |= 1 << 8;      //DISENABLE写保护
    switch (select)
    {
    case 1:
        Data_Send_VAL(0x0200, AbsoluteOpticalEncoder_VAL);
        BKP_WriteBackupRegister(BKP_DR10 , (u16)(AbsoluteOpticalEncoder_VAL    ));
        break;
    case 2:
        BKP_WriteBackupRegister(BKP_DR2 , (u16)(AbsoluteOpticalEncoder_Apart[0][0]));
        BKP_WriteBackupRegister(BKP_DR3 , (u16)(AbsoluteOpticalEncoder_Apart[0][1]));
        BKP_WriteBackupRegister(BKP_DR4 , (u16)(AbsoluteOpticalEncoder_Apart[1][0]));
        BKP_WriteBackupRegister(BKP_DR5 , (u16)(AbsoluteOpticalEncoder_Apart[1][1]));
        BKP_WriteBackupRegister(BKP_DR6 , (u16)(AbsoluteOpticalEncoder_Apart[2][0]));
        BKP_WriteBackupRegister(BKP_DR7 , (u16)(AbsoluteOpticalEncoder_Apart[2][1]));
        BKP_WriteBackupRegister(BKP_DR8 , (u16)(AbsoluteOpticalEncoder_Apart[3][0]));
        BKP_WriteBackupRegister(BKP_DR9 , (u16)(AbsoluteOpticalEncoder_Apart[3][1]));
        //BKP_WriteBackupRegister(BKP_DR10, (u16)(AbsoluteOpticalEncoder_Apart[4][0]));
//        BKP_WriteBackupRegister(BKP_DR11, (u16)(AbsoluteOpticalEncoder_Apart[1][1]));
//        BKP_WriteBackupRegister(BKP_DR12, (u16)(AbsoluteOpticalEncoder_Apart[2][1]));
//        BKP_WriteBackupRegister(BKP_DR13, (u16)(AbsoluteOpticalEncoder_Apart[3][1]));
//        BKP_WriteBackupRegister(BKP_DR14, (u16)(AbsoluteOpticalEncoder_Apart[4][1]));
//        BKP_WriteBackupRegister(BKP_DR15, (u16)(AbsoluteOpticalEncoder_Apart[5][1]));
//        BKP_WriteBackupRegister(BKP_DR16, (u16)(AbsoluteOpticalEncoder_Apart[6][1]));
//        BKP_WriteBackupRegister(BKP_DR17, (u16)(AbsoluteOpticalEncoder_Apart[7][1]));
        break;
//    case 3:
//        BKP_WriteBackupRegister(BKP_DR18, (u16)(TimeUnlock.u16[0]));
//        BKP_WriteBackupRegister(BKP_DR19, (u16)(TimeUnlock.u16[1]));
    }
//    BKP_WriteBackupRegister(BKP_DR20, 1);
    return;
}
u16 Data_Read(void)
{
    AbsoluteOpticalEncoder_VAL         =  ((s16)(BKP_ReadBackupRegister(BKP_DR10 )));
    AbsoluteOpticalEncoder_Apart[0][0] =  ((u16)(BKP_ReadBackupRegister(BKP_DR2 )));
    AbsoluteOpticalEncoder_Apart[0][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR3 )));
    AbsoluteOpticalEncoder_Apart[1][0] =  ((u16)(BKP_ReadBackupRegister(BKP_DR4 )));
    AbsoluteOpticalEncoder_Apart[1][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR5 )));
    AbsoluteOpticalEncoder_Apart[2][0] =  ((u16)(BKP_ReadBackupRegister(BKP_DR6 )));
    AbsoluteOpticalEncoder_Apart[2][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR7 )));
    AbsoluteOpticalEncoder_Apart[3][0] =  ((u16)(BKP_ReadBackupRegister(BKP_DR8 )));
    AbsoluteOpticalEncoder_Apart[3][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR9 )));
//    AbsoluteOpticalEncoder_Apart[4][0] =  ((u16)(BKP_ReadBackupRegister(BKP_DR10)));
//    AbsoluteOpticalEncoder_Apart[1][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR11)));
//    AbsoluteOpticalEncoder_Apart[2][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR12)));
//    AbsoluteOpticalEncoder_Apart[3][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR13)));
//    AbsoluteOpticalEncoder_Apart[4][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR14)));
//    AbsoluteOpticalEncoder_Apart[5][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR15)));
//    AbsoluteOpticalEncoder_Apart[6][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR16)));
//    AbsoluteOpticalEncoder_Apart[7][1] =  ((u16)(BKP_ReadBackupRegister(BKP_DR17)));
//    TimeUnlock.u16[0]                  =  ((u16)(BKP_ReadBackupRegister(BKP_DR18)));
//    TimeUnlock.u16[1]                  =  ((u16)(BKP_ReadBackupRegister(BKP_DR19)));
    return 0;//BKP_ReadBackupRegister(BKP_DR20);
};

#define SIZE 21
#define FLASH_SAVE_ADDR  0X08035000      //设置FLASH 保存地址(必须为偶数)

void StmFlash_Save(void)
{
    u16 StmFlash_Buffer[SIZE];
//    int i = 0;
//    StmFlash_Buffer[i++] = (u16)(PID_ROL.P   * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_ROL.I   * 1000);
//    StmFlash_Buffer[i++] = (u16)(PID_ROL.D   * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_PIT.P   * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_PIT.I   * 1000);
//    StmFlash_Buffer[i++] = (u16)(PID_PIT.D   * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_YAW.P   * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_YAW.I   * 1000);
//    StmFlash_Buffer[i++] = (u16)(PID_YAW.D   * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_ALT.P   * 1000);
//    StmFlash_Buffer[i++] = (u16)(PID_ALT.I   * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_ALT.D   * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_PID_1.P * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_PID_1.I * 100 );
//    StmFlash_Buffer[i++] = (u16)(PID_PID_1.D * 100 );
//    StmFlash_Buffer[i++] = (u16)(ACC_OFFSET.x      );
//    StmFlash_Buffer[i++] = (u16)(ACC_OFFSET.y      );
//    StmFlash_Buffer[i++] = (u16)(ACC_OFFSET.z      );
//    StmFlash_Buffer[i++] = (u16)(GYRO_OFFSET.x     );
//    StmFlash_Buffer[i++] = (u16)(GYRO_OFFSET.y     );
//    StmFlash_Buffer[i++] = (u16)(GYRO_OFFSET.z     );

    STMFLASH_Write(FLASH_SAVE_ADDR, (u16 *)StmFlash_Buffer, SIZE);
}
void StmFlash_Read(void)
{
//    u16 StmFlash_Buffer[SIZE];
//    int i = 0;
//    STMFLASH_Read(FLASH_SAVE_ADDR, (u16 *)StmFlash_Buffer, SIZE);
//    PID_ROL.P  =  ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_ROL.I  =  ((float)(StmFlash_Buffer[i++]) / 1000 );
//    PID_ROL.D  =  ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_PIT.P  =  ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_PIT.I  =  ((float)(StmFlash_Buffer[i++]) / 1000 );
//    PID_PIT.D  =  ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_YAW.P  =  ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_YAW.I  =  ((float)(StmFlash_Buffer[i++]) / 1000 );
//    PID_YAW.D  =  ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_ALT.P  =  ((float)(StmFlash_Buffer[i++]) / 1000 );
//    PID_ALT.I  =  ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_ALT.D  =  ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_PID_1.P = ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_PID_1.I = ((float)(StmFlash_Buffer[i++]) / 100  );
//    PID_PID_1.D = ((float)(StmFlash_Buffer[i++]) / 100  );
//    ACC_OFFSET.x  =  ((u8)(StmFlash_Buffer[i++])        );
//    ACC_OFFSET.y  =  ((u8)(StmFlash_Buffer[i++])        );
//    ACC_OFFSET.z  =  ((u8)(StmFlash_Buffer[i++])        );
//    GYRO_OFFSET.x =  ((u8)(StmFlash_Buffer[i++])        );
//    GYRO_OFFSET.y =  ((u8)(StmFlash_Buffer[i++])        );
//    GYRO_OFFSET.z =  ((u8)(StmFlash_Buffer[i++])        );
}

