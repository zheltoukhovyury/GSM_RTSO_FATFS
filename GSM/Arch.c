#include "gsm.h"
#include "GenericTypeDefs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tick.h"
#include <stdlib.h>
#include <string.h>
#include "Arch.h"
#include "TcpIp.h"
#include "VoiceCall.h"



int ArchAuthorization(void);
void ARCH_Task(void);
void CallTask(void);


void ComminicationTaskLoop(void)
{
  while(1)
  {
    //ARCH_Task();
    CallTask();
  }
}





void CallTask(void)
{
  if(VoiceCallState == VoiceCallState_Incoming)
    VoiceCall_Accept(TRUE);
    
  if(QueueHasData(&DTMF_RxQueue))
  {
    unsigned char b;
    QueueGetByte(&DTMF_RxQueue, &b);
    SendDTMF(b);
  }


}







BOOL testFlag = FALSE;


ArchState archState = ArchState_Init;//ArchState_NotInited;
ArchAuthorizationState archAuthorizationState = ArchAuth_NotInited;
TTcpSocket socket_Arch;
TArchParamters archParam;

volatile int AuthResult;

void ARCH_Task(void)
{
	
	/*
	switch((unsigned char)(TickGet()&0x3)) 
	{
		case 0:
			ArchivePutString(&archiveHandler, "azazaza!!!1");
			break;
		case 1:
			ArchiveGetString(&archiveHandler, tesetArchive);
			break;
		case 2:
			ArchiveDiscardGet(&archiveHandler, TRUE);
			break;
			ArchiveDiscardGet(&archiveHandler, FALSE);
		case 3:
			break;
	}
	
	*/
	

	
	switch(archState)
	{
		case ArchState_NotInited:		
		
		break;		
		case ArchState_Init:
			archState = ArchState_Connecting;
			archParam.RouterNumber = 555;
			archParam.DevType = 5;
			archParam.DevSubType = 1;
			archParam.crypt = 0;
			archParam.MRU = 500;
			archParam.ProtocolVerison[0] = 0;
			archParam.ProtocolVerison[1] = 2;
			archParam.timeout = 10;
			archParam.Verison[0] = 16;
			archParam.Verison[1] = 11;
			archParam.Verison[2] = 22;
			archParam.Verison[3] = 1;	
			//memset(&archParam.SessionId[0], 0x00, sizeof(SessionId));

		break;
		case ArchState_Connecting:

			//OpenSocket("80.82.49.51", "2021", &socket_Arch, TRUE);
			if(socket_Arch.state == TCPSocketState_Closed)
			{
				CloseSocket(&socket_Arch);
				socket_Arch.state = TCPSocketState_NotInited;
			}	
			if(socket_Arch.state == TCPSocketState_NotInited)
			{
				OpenSocket("auto.stadis.pro", "2021", &socket_Arch, TRUE);
			}
			else if(Connected(&socket_Arch))
			{
				archState = ArchState_Authoriztion;
				archAuthorizationState = ArchAuth_NotInited;
			}
		break;
		
		case ArchState_Authoriztion:
			if(!Connected(&socket_Arch))
				archState = ArchState_Connecting;
				
			AuthResult = ArchAuthorization();
				
			if(AuthResult == 0)
			{
				archState = ArchState_Idle;
			}	
			else if(AuthResult > 0)
			{
				//ошибка авторизации
				archState = ArchState_NotInited;
			}
		break;
		
		case ArchState_Idle:
			if(!Connected(&socket_Arch))
			{
				archState = ArchState_Connecting;
				break;
			}	
			
				if(GetFreeBytes(&socket_Arch)< 170)
					break;
			
				static UINT32 addr = 0;
				int len = 12 + 150 + 2;
				static UINT16 Number = 0;

				SendByte(0x55, &socket_Arch);
				SendByte(0x03, &socket_Arch);

				SendByte(((len)& 0xFF00) >> 8, &socket_Arch);
				SendByte(((len)& 0x00FF), &socket_Arch);
			
			
				SendByte(((Number & 0x3F00) >> 8)| 0x80, &socket_Arch);
				SendByte((Number & 0x00FF), &socket_Arch);
				Number++;
			
				SendByte(0x05, &socket_Arch);
				SendByte(0x01, &socket_Arch);
				SendByte((archParam.RouterNumber & 0xFF00) >> 8, &socket_Arch);
				SendByte(archParam.RouterNumber, &socket_Arch);
				SendByte(0x00, &socket_Arch);
				SendByte(0x01, &socket_Arch);
				SendByte(addr >> 24, &socket_Arch);
				SendByte(addr >> 16, &socket_Arch);
				SendByte(addr >> 8, &socket_Arch);
				SendByte(addr, &socket_Arch);
				SendByte(150 >> 8, &socket_Arch);
				SendByte(150, &socket_Arch);
				addr += 150;

				int i;
				for(i = 0; i < 150; i++)
				{
					if(i == 0x55)
						SendByte(i+2, &socket_Arch);					
					else
						SendByte(i, &socket_Arch);					
				}	
				SendByte(0, &socket_Arch);
				SendByte(0, &socket_Arch);
				
				
				unsigned char tempD;
				while(HasData(&socket_Arch) < 10 && Connected(&socket_Arch));
				if(!Connected(&socket_Arch))
					return ;
				for(i = 0; i < 10; i++)
					GetData(&tempD, &socket_Arch);

				archState = archState;
		break;
		
		case ArchState_TestState:
		break;
		
	}
}//                                                                                                                                                                                          ..+RECEIVE,0,18:..............yyacAc





int Parser(unsigned char b)
{
	
	
	
	return 0;
}//






volatile int test;

int ArchAuthorization(void)
{
	switch(archAuthorizationState)
	{
		case ArchAuth_NotInited:
			archAuthorizationState = ArchAuth_Authoriztion_1;
			return -1;
			break;
		case ArchAuth_Authoriztion_1:{
			unsigned char ServerAnswer[24];
		
		
			testFlag = TRUE;
		
			SendByte(archParam.DevType, &socket_Arch);
			SendByte(archParam.DevSubType, &socket_Arch);
			SendByte(archParam.RouterNumber>>8, &socket_Arch);
			SendByte(archParam.RouterNumber, &socket_Arch);
			SendByte(archParam.ProtocolVerison[0], &socket_Arch);
			SendByte(archParam.ProtocolVerison[1], &socket_Arch);
			SendByte(archParam.Verison[0], &socket_Arch);
			SendByte(archParam.Verison[1], &socket_Arch);
			SendByte(archParam.Verison[2], &socket_Arch);
			SendByte(archParam.Verison[3], &socket_Arch);
			SendByte(archParam.crypt, &socket_Arch);
			SendByte(archParam.timeout, &socket_Arch);
			SendByte(archParam.MRU>>8, &socket_Arch);
			SendByte(archParam.MRU, &socket_Arch);
			int i;
			for(i=0;i<4;i++)
				SendByte(i, &socket_Arch);
			

			while(HasData(&socket_Arch) < 1 && Connected(&socket_Arch));
			if(!Connected(&socket_Arch))
				return -1;

			QueueGetByte(&socket_Arch.rxQueue, &ServerAnswer[0]);
			if(ServerAnswer[1] != 0)
				return (int)ServerAnswer[0];
				
			while(HasData(&socket_Arch) < 18 && Connected(&socket_Arch));
			if(!Connected(&socket_Arch))
				return -1;
			for(i = 0; i < 18; i++)
				GetData(&ServerAnswer[i], &socket_Arch);
			SendByte(0x00, &socket_Arch);

			while(HasData(&socket_Arch) < 1 && Connected(&socket_Arch));
			if(!Connected(&socket_Arch))
				return -1;
			QueueGetByte(&socket_Arch.rxQueue, &ServerAnswer[0]);
test = (int)ServerAnswer[0];
			return (int)ServerAnswer[0];
		}
		default:
			return -1;	
	}

}
