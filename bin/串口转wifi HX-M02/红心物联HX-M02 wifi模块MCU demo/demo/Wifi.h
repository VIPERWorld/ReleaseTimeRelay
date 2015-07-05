#ifndef __SIMPLEWIFI_H__
#define __SIMPLEWIFI_H__

typedef struct _TATCP_COMMAND{
	char *CmdName;
	INT    Flag;
	char * Cmdbuf;
	INT  *Cmdbuflen;
} TATCP_CMD_SIMPLEWIFI;

typedef struct _TJION_RESULT_SIMPLEWIFI{
	unsigned char BSSID[12];
	unsigned char Type;    ///0 infra   1---adhoc
	unsigned char  channel;
	unsigned char   encry;
	char ssid[32];
	int    rssi;
}TJION_RESULT_SIMPLEWIFI;

typedef	struct _WIFIKEY_SIMPLEWIFI{
		unsigned char PSK[64];
		unsigned char KeyLength; 
		unsigned char KeyIndex;
		unsigned char KeyFormat;
}WIFIKEY_SIMPLEWIFI;

typedef	struct _NETWORK_SIMPLEWIFI{
		unsigned char BSSID[12];
		unsigned char Type; 
		unsigned char Channel;
		unsigned char encry;
		char    ssid[32];
		int       rssi;
}NETWORK_SIMPLEWIFI;
NETWORK_SIMPLEWIFI Scanresult[32];

unsigned char AtCmdSending;
unsigned char ReciveAtCmdrep;
unsigned char Atcmdtimeout;

unsigned char AtcmdTimerId;


#endif //__SIMPLEWIFI_H__
