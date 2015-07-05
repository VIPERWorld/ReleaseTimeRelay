/*-------------------------------------------------------------------------
-------------------------------------------------------------------------*/
#include "wifi.h"

extern void ComSendstr(char str);
extern int StartTimer(unsigned int TimeOut);
extern int StopTimer(unsigned char timeid);

#define IN
#define OUT

#define TIMER_ATCMD_TIMEOUT  2000  ///ms
#define MSG_CMD_TIMEOUT_CFM    1

int SimpleWiFi_AtCmdSend(TATCP_CMD_SIMPLEWIFI * AtCmd)
{
    unsigned char AtCmdbuf[512];
    int ret=0;
   ////  
   ////AtCmd_Check();    ///AtCmdCheck 
   /////	
   strcpy(AtCmdbuf, "AT+");
   strcat(AtCmdbuf, AtCmd->Cmdbuf);
   ReciveAtCmdrep=0;
   /*Send AT Command with a timeout*/
   ComSendstr(AtCmdbuf);
   AtcmdTimerId=StartTimer( MSG_CMD_TIMEOUT_CFM,TIMER_ATCMD_TIMEOUT);
   while(ReciveAtCmdrep!=1)
   {
       if(Atcmdtimeout==1)
      {
           ret=-1;
          break;
      }
   }
   StopTimer(AtcmdTimerId);
   return ret;
}
 
void COMM_INT() interrupt 4 using 3
{
    unsigned char	 cBuf;
    if(RI)// 接收数据
   {
      RI = 0;
     cBuf = SBUF;
     SimpleWiFi_ProcessUARTData(cBuf);
   }
   else if (TI)
  {
    TI = 0;
    if(iSendCounter < iUartSendLen)
       SBUF = cpSendBufPtr[iSendCounter++];
   }
}

unsigned char	 	gcWifiCommunState;   	// 通讯接收状态

#define WIFIBUFLEN   2048
typedef struct _SimpleWiFiRecBuffer
{
	unsigned char  	gcATReceData[WIFIBUFLEN];  //
	unsigned int 		gcATReceCount;
} SimpleWiFiRecBuffer;
struct SimpleWiFiRecBuffer  xdata  ATReceBuffer[1];

/////com  recive state
#define WIFI_WAIT_SYN     	0X00    ////串口初始状态
#define WIFI_WAIT_RESP     	0x01   ////串口等待相应字符
#define WIFI_RECEDATA      	0x02   ////等待数据
#define WIFI_WAIT_OVER	   	0x03   ////等待接收结束字符
#define WIFI_WAIT_ERRCODE	   	0x04   ////等待接收结束字符



#define WIFI_SYN          	       0X2B    ////+   收到同步字符状态
unsigned char   AtResp[8];
unsigned char  AtResplen=0;
unsigned char AtRespState;     /////1--OK     0--ERR




void SimpleWiFi_ProcessUARTData(IN unsigned char  UartData)
{
     int i=0;
     switch (gcWifiCommunState)
     {
       case WIFI_WAIT_SYN:		   // 接收等待
             if(UartData==WIFI_SYN)
             {
                   gcWifiCommunState = WIFI_WAIT_RESP;
              }
              break;
       case WIFI_WAIT_RESP:	         
              AtResp[AtResplen++]=UartData;
              if(strcmp(AtResp,"OK")==0)
              {
                  AtRespState=1;    ////response ok
                  gcWifiCommunState = WIFI_RECEDATA;	
                  AtResplen=0;
              }
              else if(strcmp(AtResp,"ERR")
              {
                  AtRespState=0;    ////response ERR
                 gcWifiCommunState=WIFI_WAIT_ERRCODE;
                 AtResplen=0;
              }
              else
             {
                 printf("Recive unknown words\n");
                 AtRespState=0;    ////response ERR
                 gcWifiCommunState=WIFI_WAIT_SYN;
                 AtResplen=0;
              }
	       break;
      case WIFI_RECEDATA:	        
      case WIFI_WAIT_OVER:
      case WIFI_WAIT_ERRCODE:
               ATReceBuffer[0].gcATReceData[ATReceBuffer[0].gcATReceCount]= UartData;
               ATReceBuffer[0].gcATReceCount++;
              if(strstr(ATReceBuffer[0].gcATReceData,"\r\n\r\n")!=NULL)
              {
                   ReciveAtCmdrep=1;
                   gcWifiCommunState = WIFI_WAIT_SYN;	
                   ATReceBuffer[0].gcATReceCount=0;
              }
              else if(ATReceBuffer[0].gcATReceCount==WIFIBUFLEN)
             {
                   printf("At cmd recive error\n");
                   gcWifiCommunState = WIFI_WAIT_SYN;	
             }
              else
             {
                  printf("Wiat next byte\n");
             }
             break;	
        default:
             gcWifiCommunState = WIFI_WAIT_SYN;	
             break;
        }
}
#define ATCP_MAXARGS    64

INT SimleWiFi_parse_line (char *line, char *argv[])
{
	INT nargs = 0;

	while (nargs < ATCP_MAXARGS) {

		/* skip any white space */
		while ((*line == ' ') || (*line == '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return (nargs);
		}

		if (*line == '"'){/* argument such as  "xxxx" */

			line++; /* skip 1st <"> */
			argv[nargs++] = line;	/* begin of argument string	*/

			/* find end of string */
			while (*line && (*line != '"')) {
				++line;
			}

			if (*line == '\0') {	/* end of line, but 2nd <"> is not found */
				argv[nargs] = NULL;
				return (-1);
			}

			*line++ = '\0';		/* terminate current arg	 */

			/* skip any white space */
			while ((*line == ' ') || (*line == '\t')) {
				++line;
			}

			if (*line == ','){
				line++; /* skip <,> */
			}
			else if (*line == '\0'){ /* end of line, no more args	*/
				argv[nargs] = NULL;
				return (nargs); 
			}
			else{ /* unexpected char */
				argv[nargs] = NULL;
				return (-1); 
			}

		}
		else{
			argv[nargs++] = line;	/* begin of argument string	*/

			/* find end of string */
			while (*line && (*line != ',')) {
				++line;
			}

			if (*line == '\0') {	/* end of line, no more args	*/
				argv[nargs] = NULL;
				return (nargs);
			}

			*line++ = '\0';		/* terminate current arg	 */
		}
	}

	return (-1);
}

INT atodec(char Ch)
{
	INT dec = -1;
	if (Ch >= '0' && Ch <= '9')
		dec = Ch - '0';
	return dec;
}

INT strtodec(INT *dec, char *str)
{
	INT i = -1;
	INT dd = 0, sign = 1;

	if (*str == '-'){
		str++;
		sign = -1;
	}
	while(*str){
		i = atodec(*str++);
		if (i < 0) {return -1;}
		dd = dd*10 + i;
	}
	*dec = dd*sign;
	return (i<0?-1:0);
}
INT atohex(char Ch)
{
	INT hex = -1;
	
	if (Ch >= '0' && Ch <= '9')
		hex = Ch - '0';
	else if (Ch >= 'a' && Ch <= 'f')
		hex = Ch - 'a' + 0xa;
	else if (Ch >= 'A' && Ch <= 'F')
		hex = Ch - 'A' + 0xa;

	return hex;
}

INT strtohexarray(INT8U array[], INT cnt, char *str)
{
	INT hex;
	INT8U tmp, *des;

	des = array;
	
	while(cnt-- > 0)	
	{
		hex = atohex(*str++);
		if (hex < 0)
			return -1;
		else{
			tmp = (hex<<4)&0xf0;
		}

		hex = atohex(*str++);
		if (hex < 0)
			return -1;
		else{
			tmp = tmp | (hex & 0x0f);
		}
		
		*des++ = (INT8U) tmp;
	}
	
	return (*str==0?0:-1);
}


INT SimpleWiFi_parse_line_group (char *line, char *argv_group[])
{
	INT nargs = 0;

	while (nargs < ATCP_MAXARGS) {

		/* skip any white space */
		while ((*line == ' ') || (*line == '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv_group[nargs] = NULL;
			return (nargs);
		}

		{

			line++; 
			argv_group[nargs++] = line;	/* begin of argument group	*/
			/* find end of string */
			while (*line!='\r' && (*(line+1) != '\n')) {
				++line;
			}
			if (*line == '\0') {	/* end of line, but 2nd <"> is not found */
				argv_group[nargs] = NULL;
				return (nargs);
			}
			
			*line++ = '\0';		/* terminate current arg group	 */
			*line++ = '\0';	
			
			if (*line=='\r' && (*(line+1) == '\n')) {	
				*line++ = '\0';		/* terminate current arg group	 */
				*line++ = '\0';	
				return (nargs);		
			}
		}
	}
	return (-1);
}



#define  RespParm        1 
#define  NoRespParm     0
#define ATC_CH_SET         '='
#define ATC_CH_SAVE         '!'
#define ATC_CH_QUERY     '?'

void SimpleWiFi_Jion(void)
{
	TATCP_CMD_SIMPLEWIFI WiFiJoin;
	TJION_RESULT_SIMPLEWIFI Jionresp;
	unsigned char ErrorCode=0;
	char *arg = NULL;
	INT    argc = 0;	
	int argpoint=0;
	INT err = 0;
	char *argv[ATCP_MAXARGS] ;	
	char *  ATCmd="WJOIN\n";
	memset(WiFiJoin,0,sizeof(TATCP_CMD_SIMPLEWIFI));
	memset(ATReceBuffer[0].gcATReceData,0,sizeof(SimpleWiFiRecBuffer));
	
	WiFiJoin.Flag=RespParm;
	WiFiJoin.Cmdbuf =ATCmd;
	WiFiJoin.Cmdbuflen=strlen(ATCmd);
	SimpleWiFi_AtCmdSend(WiFiJoin);
	if(AtRespState==1)
	{
		arg = strchr(ATReceBuffer[0].gcATReceData, ATC_CH_SET);
		if(arg)
		{	
			{
				argc = SimleWiFi_parse_line(arg, argv);
				if (argc <= 0)
				{
					printf("Recive Error parm\n");
					return;
				}
			}

			if (argc != 6)
			{
				printf("recvie parm number error\n");
			}
			else
			{
				if (strtohexarray(Jionresp.BSSID, 6, argv[0]) < 0)
				{
					printf("Error Bssid\n");
					return ;
				}				
				if (strtodec(&(Jionresp.Type), argv[1]) < 0)
				{
					return ;
				}	
				if(strtodec(&(Jionresp.channel), argv[2]) < 0)
				{
					return;
				}
				if(strtodec(&(Jionresp.encry), argv[3])<0)
				{
					return;
				}
				strcpy((char *)Jionresp.Ssid, argv[4]);
				if(strtodec(&(Jionresp.rssi), argv[5])<0)
				{
					return;
				}				
			}
		}
	}
	else
	{
		ErrorCode=*(arg+2);
		printf("reciver error At cmd resp ERROR Code:%d",ErrorCode);
	}
}

void SimpleWiFi_SetKey(unsigned char NeedFlash)
{
	WIFIKEY_SIMPLEWIFI KeySet;
	WIFIKEY_SIMPLEWIFI KeyResult;
	TATCP_CMD_SIMPLEWIFI ATWiFiJoin;
	unsigned char ErrorCode=0;
	char *arg = NULL;
	INT    argc = 0;	
       char   WiretFlash;
	int argpoint=0;
	INT err = 0;
	char *argv[ATCP_MAXARGS] ;	
	char *  ATCmd="KEY=";
	char  Atbuf[128];
	memset(KeySet,0,sizeof(WIFIKEY_SIMPLEWIFI));
	memset(ATReceBuffer[0].gcATReceData,0,sizeof(SimpleWiFiRecBuffer));
	memset(Atbuf,0,128);

	strcpy(KeySet.PSK,"12345678");
       KeySet.KeyLength=8;
	KeySet.KeyIndex=1;
	KeySet.KeyFormat=1;
	
	if(NeedFlash)
	{
		WiretFlash='!';
	}
	else
	{
		WiretFlash=NULL:
	}
	sprintf(Atbuf, "%s%c%d,%d,\"%s\"\n",ATCmd, WiretFlash,KeySet.KeyFormat, KeySet.KeyIndex , KeySet.PSK);
	ATWiFiJoin.Flag=1;
	ATWiFiJoin.Cmdbuf=Atbuf;
	ATWiFiJoin.Cmdbuflen=strlen(Atbuf);
	SimpleWiFi_AtCmdSend(ATWiFiJoin);
	if(AtRespState==1)
	{
		arg = strchr(ATReceBuffer[0].gcATReceData, ATC_CH_SET);
		if(arg)
		{	
			{
				argc = SimleWiFi_parse_line(arg, argv);
				if (argc <= 0)
				{
					printf("Recive Error parm\n");
					return;
				}
			}

			if (argc != 3)
			{
				printf("recvie parm number error\n");
			}
			else
			{
				
				if (strtodec(&(KeyResult.KeyFormat), argv[0]) < 0)
				{
					return ;
				}	
				if(strtodec(&(KeyResult.KeyIndex), argv[1]) < 0)
				{
					return;
				}
				strcpy((char *)KeyResult.Psk, argv[2]);			
			}
		}
	}
	else
	{
		ErrorCode=*(arg+2);
		printf("reciver error At cmd resp ERROR Code:%d",ErrorCode);
	}
}

void SimpleWiFiScan(void)
{
	TATCP_CMD_SIMPLEWIFI WiFiScan;
	unsigned char ErrorCode=0;
	char *arggroup = NULL;	
	char *arg = NULL;
	INT    arggroupc = 0;		
	INT    argc = 0;	
	int argpoint=0;
	INT err = 0;
	int i=0;

	char *argv[ATCP_MAXARGS] ;	
	char *arggroupv[ATCP_MAXARGS] ;	
	
	char *  ATCmd="WSCAN\n";
	

	memset(WiFiJoin,0,sizeof(TATCP_CMD_SIMPLEWIFI));	
	memset(ATReceBuffer[0].gcATReceData,0,sizeof(SimpleWiFiRecBuffer));

	WiFiScan.Flag=RespParm;
	WiFiScan.Cmdbuf =ATCmd;
	WiFiScan.Cmdbuflen=strlen(ATCmd);
	SimpleWiFi_AtCmdSend(WiFiScan);
	
	if(AtRespState==1)
	{
		arggroup=strchr(ATReceBuffer[0].gcATReceData, ATC_CH_SET);
		//arg = strchr(ATReceBuffer[0].gcATReceData, ATC_CH_SET);
		if(arggroup)
		{
			{
				arggroupc = SimpleWiFi_parse_line_group(arggroup, arggroupv);
				if (argc <= 0)
				{
					printf("Recive Error parm\n");
					return;
				}
			}
			for(i=0;i<arggroupc;i++)
			{
				argc = SimleWiFi_parse_line(&arggroupv[arggroupc], argv);
				if (argc <= 0)
				{
					printf("Recive Error parm\n");
					return;
				}
				else
				{
					if (strtohexarray(Scanresult[arggroupc].BSSID, 6, argv[0]) < 0)
					{
						printf("Error Bssid\n");
						return ;
					}				
					if (strtodec(&(Scanresult[arggroupc].Type), argv[1]) < 0)
					{
						return ;
					}	
					if(strtodec(&(Scanresult[arggroupc].channel), argv[2]) < 0)
					{
						return;
					}
					if(strtodec(&(Scanresult[arggroupc].encry), argv[3])<0)
					{
						return;
					}
					strcpy((char *)Scanresult[arggroupc].Ssid, argv[4]);
					if(strtodec(&(Scanresult[arggroupc].rssi), argv[5])<0)
					{
						return;
					}				
				}						
			}
		}
	}
	else
	{
		ErrorCode=*(arg+2);
		printf("reciver error At cmd resp ERROR Code:%d",ErrorCode);
	}	

}


