#ifdef __cplusplus
       extern "C" {
#endif
#define LED1_Init     PC13_OUT
#define LED1_Toggle {static char i = 0;PCout(13) = i++&0X01;}
#define LED1_H PCout(13) = 1
#define LED1_L PCout(13) = 0

int task_led(void)
{
    _SS
    LED1_Init;
    LED1_H;
    while (1)
    {
        if (RC_Control.ARMED)
        {
            if (RC_Control.ALT_ON_OFF)
            {
                WaitX(50);
                LED1_Toggle;
            }
            else
            {
                WaitX(100);
                LED1_H;
            }
        }
        else
        {
            WaitX(200);
            LED1_Toggle;
        }
				        

    }
    _EE
}

int loop_led(void)
{
    _SS
    LED1_Init;
    LED1_H;
    _LOOP_SS
    if (RC_Control.ARMED)
        LED1_H;
    else
        LED1_Toggle;
    LoopX(500);
    _EE
}
#ifdef __cplusplus
        }
#endif
