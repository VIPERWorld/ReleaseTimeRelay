void staCom::sendencodeKeyString(char * KeyStr,SOCKET sockClient,SOCKADDR * sockAdr)
{
  int i=0;
  unsigned char tempdata;
  int Dataoffset=593;
  unsigned char prevNibble = 0;
  unsigned char currentIndex = 0;
  unsigned char lowNibble;
  unsigned char highNibble;
  int Keylen=strlen(KeyStr);
  unsigned char Keycheck=0;
  memset(DataSend,0,256);
  for(i=0;i<Keylen;i++)
  {
     tempdata=*(KeyStr+i);  
     lowNibble = tempdata & 0xf;     /////保留低4位
     highNibble = tempdata >> 4;    /////保留高4位
     DataSend[2*i]=(((prevNibble ^ currentIndex++) << 4 | highNibble));
     sendto(sockClient,Databuf,DataSend[2*i]+ Dataoffset,0,sockAdr,sizeof(SOCKADDR));
     prevNibble = highNibble;
     DataSend[2*i+1]=(((prevNibble ^ currentIndex++) << 4 | lowNibble));
    sendto(sockClient,Databuf,DataSend[2*i+1]+Dataoffset,0,sockAdr,sizeof(SOCKADDR));
     prevNibble = lowNibble;
     currentIndex &= 0xf;
  }
  for(i=0;i<(Keylen);i++)
  {
	  Keycheck+=*(KeyStr+i); 
  }
   ////Send check///////
     lowNibble = Keycheck & 0xf;     /////保留低4位
     highNibble = Keycheck >> 4;    /////保留高4位
     DataSend[2*Keylen]=(((prevNibble ^ currentIndex++) << 4 | highNibble));
     sendto(sockClient,Databuf,DataSend[2*Keylen]+ Dataoffset,0,sockAdr,sizeof(SOCKADDR));
     currentIndex &= 0xf;
     prevNibble = highNibble;
     DataSend[2*Keylen+1]=(((prevNibble ^ currentIndex++) << 4 | lowNibble));
     sendto(sockClient,Databuf,DataSend[2*Keylen+1]+Dataoffset,0,sockAdr,sizeof(SOCKADDR));
     prevNibble = lowNibble;
     currentIndex &= 0xf;
}
void staCom::sendencodeSsidString(char * Ssid,SOCKET sockClient,SOCKADDR * sockAdr)
{
  int i=0;
  unsigned char tempdata;
//  unsigned char Datatemp;
  int Dataoffset=593;
  unsigned char prevNibble = 0;
  unsigned char currentIndex = 0;
  unsigned char lowNibble;
  unsigned char highNibble;
  int ssidlen=strlen(Ssid);
  unsigned char ssidcheck=0;
  memset(DataSend,0,256);
  for(i=0;i<ssidlen;i++)
  {tempdata=*(Ssid+i);  
     lowNibble = tempdata & 0xf;     /////保留低4位
     highNibble = tempdata >> 4;    /////保留高4位
	 DataSend[2*i]=(((prevNibble ^ currentIndex++) << 4 | highNibble));
	 sendto(sockClient,Databuf,DataSend[2*i]+ Dataoffset,0,sockAdr,sizeof(SOCKADDR));
     prevNibble = highNibble;
	 DataSend[2*i+1]=(((prevNibble ^ currentIndex++) << 4 | lowNibble));
	 sendto(sockClient,Databuf,DataSend[2*i+1]+Dataoffset,0,sockAdr,sizeof(SOCKADDR));
     prevNibble = lowNibble;
     currentIndex &= 0xf;
  }
  for(i=0;i<(ssidlen);i++)
  {
	  ssidcheck+=*(Ssid+i); 
  }
   ////Send check///////
     lowNibble = ssidcheck & 0xf;     /////保留低4位
     highNibble = ssidcheck >> 4;    /////保留高4位
	 DataSend[2*ssidlen]=(((prevNibble ^ currentIndex++) << 4 | highNibble));
	 sendto(sockClient,Databuf,DataSend[2*ssidlen]+ Dataoffset,0,sockAdr,sizeof(SOCKADDR));
     currentIndex &= 0xf;
     prevNibble = highNibble;
	 DataSend[2*ssidlen+1]=(((prevNibble ^ currentIndex++) << 4 | lowNibble));
	 sendto(sockClient,Databuf,DataSend[2*ssidlen+1]+Dataoffset,0,sockAdr,sizeof(SOCKADDR));
     prevNibble = lowNibble;
     currentIndex &= 0xf;
}
void staCom::sendencodefromwifi(SOCKET sockClient,SOCKADDR * sockAdr,int len)
{
	int i=0;
	for(i=0;i<2;i++)
	{
	   sendto(sockClient,Databuf,len,0,sockAdr,sizeof(SOCKADDR));
	}
}

int staCom::staWirelessOpen(char * Dest_ip, char * Ssid, char * Config_key)
{
	int err;
	int SendNum=0;
    //char Ssid[512];
	unsigned char  ssidlen;
	unsigned char  ssidlentemp1;
	unsigned char  ssidlentemp2;
	unsigned char  ssidlensend1;
	unsigned char  ssidlensend2;


    //char KeyStr[65];
	unsigned char  Keylen;
	unsigned char  Keylentemp1;
	unsigned char  Keylentemp2;
	unsigned char  Keylensend1;
	unsigned char  Keylensend2;

	wVersionRequested = MAKEWORD(2,0);
	err = WSAStartup(wVersionRequested,&wsaData);
	if(err != 0)
	{
		return DUT_NO_VALID_CON;
	}
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
	{

		WSACleanup();
		return DUT_NO_VALID_CON;
	}


	//创建socket
	ScansockClient = socket(AF_INET,SOCK_DGRAM,0);
	SOCKADDR_IN sockAdr;
	sockAdr.sin_addr.S_un.S_addr = inet_addr(Dest_ip);
	sockAdr.sin_family = AF_INET;
	sockAdr.sin_port = htons(5858);

    ssidlen=strlen((char*)Ssid)+1+27;////1 wei check
	ssidlentemp1=(ssidlen&0xF0)>>4;
	ssidlentemp2=(ssidlen&0x0F);

    Keylen=strlen(Config_key)+1+27;/////1 wei check
	Keylentemp1=(Keylen&0xF0)>>4;
	Keylentemp2=(Keylen&0x0F);
 
    for(SendNum=0;SendNum<350;SendNum++)
    {
		memset(DataSend,0,256);
		sendto(ScansockClient,"abcd",strlen("abcd"),0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
		sendto(ScansockClient,"abcd",strlen("abcd"),0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
		sendto(ScansockClient,"abcdefghijklmnopqrstuvw",strlen("abcdefghijklmnopqrstuvw"),0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
		sendto(ScansockClient,"abcdefghijklmnopqrstuvw",strlen("abcdefghijklmnopqrstuvw"),0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
        sendencodefromwifi(ScansockClient,(SOCKADDR*)&sockAdr,1399);
		if((ssidlentemp1+ssidlentemp2)%2==0)
		{
           ssidlensend1=ssidlentemp1|0x70;
           ssidlensend2=ssidlentemp2|0x70;
		}
		else
		{
           ssidlensend1=ssidlentemp1|0x30;
           ssidlensend2=ssidlentemp2|0x30;
		}
		/*send ssidlen*/
		sendto(ScansockClient,Databuf,(ssidlensend1+593),0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
		sendto(ScansockClient,Databuf,(ssidlensend1+593),0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
        ssidlensend2 |=0x80;////ssid len index is bit7
		sendto(ScansockClient,Databuf,(ssidlensend2+593),0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
		sendencodeSsidString(Ssid,ScansockClient,(SOCKADDR*)&sockAdr);


		/* send midle*/
        sendencodefromwifi(ScansockClient,(SOCKADDR*)&sockAdr,1299);
		if((Keylentemp1+Keylentemp2)%2==0)
		{
           Keylensend1=Keylentemp1|0x70;
           Keylensend2=Keylentemp2|0x70;
		}
		else
		{
           Keylensend1=Keylentemp1|0x30;
           Keylensend2=Keylentemp2|0x30;
		}

		/*2 times*/
		sendto(ScansockClient,Databuf,Keylensend1,0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
		sendto(ScansockClient,Databuf,Keylensend1,0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
        Keylensend2 |=0x80;////ssid len index is bit7

		/*1 times*/
		sendto(ScansockClient,Databuf,Keylensend2,0,(SOCKADDR*)&sockAdr,sizeof(SOCKADDR));
		sendencodeKeyString(Config_key,ScansockClient,(SOCKADDR*)&sockAdr);
		Sleep(80);
	}
	closesocket(ScansockClient);
	staSendType(1);
	return DUT_ERR_OK;
}
