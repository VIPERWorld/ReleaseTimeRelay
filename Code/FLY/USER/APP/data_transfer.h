#ifndef _DATA_TRANSFER_H_
#define _DATA_TRANSFER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"
#define AbsoluteOpticalEncoderNUM 8
	
typedef union
{
	u8 u8[4];
	u32 u32;
} _Uu32u8; //����ʱ��
extern _Uu32u8 TimeUnlock;
extern int TimeUnlockFlag;
extern u8 RelayStata[AbsoluteOpticalEncoderNUM];
extern int AbsoluteOpticalEncoder_VAL;//����ʽ��������
extern u16 AbsoluteOpticalEncoder_LastVAL;//����ʽ��������
extern u16 AbsoluteOpticalEncoder_Apart[][2];//�̵�����ֵ
extern int TimeUnlockFlag;
extern int WifiConfigFlag;
extern int WifiRESTFlag;
extern int breakENTMFlag;


#ifdef __cplusplus
}
#endif
#endif

