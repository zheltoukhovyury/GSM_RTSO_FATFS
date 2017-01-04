#include "GenericTypeDefs.h"
#include "TQueue.h"
#include "TcpIp.h"
#include "gsm.h"
#include "Tick.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SOCKET_CNT 3

TTcpSocket* socket_List[MAX_SOCKET_CNT];
static TGSM_ATCommandResult result_l;

BOOL TransparentModeEnabled;
BOOL TransparentModeEnable;

void OpenSocket(char* address, char* port,volatile TTcpSocket* socket, BOOL blocking)
{
	if(TransparentModeEnabled)
	{
		int i;
		for(i = 0; i < MAX_SOCKET_CNT; i++)
		{
			if(socket_List[i] != NULL)
			{
				return;
			}
		}
		
		socket_List[0] = (TTcpSocket*)socket;

		QueueCreate((TQueue*)&(socket->txQueue));
		QueueCreate((TQueue*)&(socket->rxQueue));
		socket->blocking = blocking;
		socket->address = (char*)malloc(32);
		socket->port = (char*)malloc(6);
		
		strncpy(socket->address, address, 32);
		strncpy(socket->port, port, 6);
	
		socket->state = TCPSocketState_Closed;
		socket->state = TCPSocketState_StartConnect;
		if(blocking)
		{
			while(!Connected(socket))
			{
			
			}
		}
	}
	else
	{
		QueueCreate((TQueue*)&(socket->txQueue));
		QueueCreate((TQueue*)&(socket->rxQueue));
		socket->blocking = blocking;
		socket->address = (char*)malloc(32);
		socket->port = (char*)malloc(6);
		
		strncpy(socket->address, address, 32);
		strncpy(socket->port, port, 6);
	
		socket->state = TCPSocketState_Closed;
	
		int i;
		for(i = 0; i < MAX_SOCKET_CNT; i++)
		{
			if(socket_List[i] == NULL)
			{
				socket_List[i] = (TTcpSocket*)socket;	
				break;
			}
		}
		
		
		socket->state = TCPSocketState_StartConnect;
		if(blocking)
		{
			while(!Connected(socket))
			{
			
			}
		}
	}
}//


void CloseSocket(volatile TTcpSocket* socket)
{
	if(socket->address != NULL)
		free(socket->address);
	if(socket->port != NULL)
		free(socket->port);
	QueueDestroy((TQueue*)&(socket->txQueue));
	QueueDestroy((TQueue*)&(socket->rxQueue));
	
	int i;
	for(i = 0; i < MAX_SOCKET_CNT; i++)
	{
		if(socket_List[i] == socket)
		{
			socket->state = TCPSocketState_StartClose;
			while(socket->state != TCPSocketState_Closed);
			socket_List[i] = NULL;	
			
			break;
		}
	}		
}//

BOOL Connected(volatile TTcpSocket* socket)
{
	return(BOOL)(socket->state == TCPSocketState_Connected || socket->state == TCPSocketState_DataSending_GettingCapacity|| socket->state == TCPSocketState_DataSending_WaitArrow || socket->state == TCPSocketState_TransparentPause);
}//



BOOL SendByte(unsigned char b,volatile TTcpSocket* socket)
{
	QueuePutByte((TQueue*)&socket->txQueue, b);
	return TRUE;
}//



BOOL GetData(unsigned char* dest, volatile TTcpSocket* socket)
{
	return(QueueGetByte((TQueue*)&(socket->rxQueue), dest));
}//


int HasData(volatile TTcpSocket* socket)
{
	return QueueHasData((TQueue*)&(socket->rxQueue));
}//


int GetFreeBytes(volatile TTcpSocket* socket)
{
	return QueueGetFree((TQueue*)&(socket->txQueue));
}//
				


void SetGPRS_TransparentMode(void)
{
	
	TransparentModeEnable = TRUE;
}//



void SetGPRS_MuxMode(void)
{
	
	TransparentModeEnable = FALSE;	
}//




void TCPPStackInit(void)
{
  result_l.Ready = FALSE;
   
  result_l.ResultText = (unsigned char*)malloc(50);
  
  int i;
  for(i = 0; i < MAX_SOCKET_CNT; i++)
    socket_List[i] = NULL;
  
  TransparentModeEnabled = TRUE;
  TransparentModeEnable = TRUE;
  
}


void TCPPStack(TGSM_ATCommandResult result)
{
	int i;
	int Connection;
	TTcpSocket* socket;

	if(result_l.ResultText == NULL)

		
		
	if(result.Ready == TRUE && result.answerType == TCPIPDATARECEIVED)
	{
		for(i = 0; i < MAX_SOCKET_CNT; i++)
			if(socket_List[i] != NULL &&  result.ExtraArgument == i)
				QueuePutByte((TQueue*)&socket_List[i]->rxQueue, result.ResultText[0]);
	}
	
	
	if(result.Ready == TRUE)
	{
		switch(result.answerType)
		{
			case MUX_CLOSED:{
				int connection = atoi((char*)&result.ResultText[0]);
					if(socket_List[connection] != NULL)
				socket_List[connection]->state = TCPSocketState_Closed;	
			break;}
			case MUX_CONNECT_OK:
			{
				int connection = atoi((char*)&result.ResultText[0]);
				if(socket_List[connection] != NULL)
					socket_List[connection]->state = TCPSocketState_Connected;		
			break;}
			case TRNPT_CONNECT_OK:
				if(TransparentModeEnabled && socket_List[0] != NULL)
					socket_List[0]->state = TCPSocketState_Connected;
			break;
			case TRNPT_CONNECT_FAIL:
			case TRNPT_CLOSED:
				if(TransparentModeEnabled && socket_List[0] != NULL)
					socket_List[0]->state = TCPSocketState_Closed;
                                
                                
                                
                case CIPSEND_CAPACITY:{
                        int con = atoi((char const*)&result.ResultText[10]);
                        int cap = atoi((char const*)&result.ResultText[12]);

                        if(socket_List[con] != NULL)
                                socket_List[con]->sendCapacity = cap;
                        
                        if(socket_List[con]->state == TCPSocketState_DataSending_GettingCapacity)
                          socket_List[con]->state = TCPSocketState_DataSending_BeginSending;
                }
                break;
                                
                                
			break;
			default:
			break;
		
		}
	}
	
	
	if(TransparentModeEnable && !TransparentModeEnabled)
	{
		for(i = 0; i < MAX_SOCKET_CNT; i++)
		{
			if(socket_List[i] != NULL)
			{
				Connection = i;
				socket = socket_List[i];
				char command_string[50];
				sprintf((char *) command_string, "AT+CIPCLOSE=%u,1\r", Connection);
				GSM_ExecuteATCommand(command_string, &result_l, TRUE);	
				socket->state = TCPSocketState_Closed;
			}	
		}	
		GSM_ExecuteATCommand("AT+CIPMUX=0\r", &result_l, TRUE);	
		GSM_ExecuteATCommand("AT+CIPMODE=1\r", &result_l, TRUE);	
		TransparentModeEnabled = TRUE;
	}
	
	
	if(TransparentModeEnable && !TransparentModeEnabled)
	{
		GSM_ExecuteATCommand("AT+CIPCLOSE\r", &result_l, TRUE);	

		GSM_ExecuteATCommand("AT+CIPMUX=1\r", &result_l, TRUE);	
		GSM_ExecuteATCommand("AT+CIPMODE=0\r", &result_l, TRUE);	
		TransparentModeEnabled = TRUE;
	}
	
		
	for(i = 0; i < MAX_SOCKET_CNT; i++)
	{
		if(socket_List[i] != NULL)
		{
			Connection = i;
			socket = socket_List[i];

			switch(socket->state)
			{
				case TCPSocketState_StartConnect:{
					if(GSM_GeneralState != GSM_TurnedON)
						break;	

					socket->state = TCPSocketState_Connecting_SendCommand;
					
					char command_string[50];
					if(TransparentModeEnabled)
					{
						sprintf((char *) command_string, "\r\nAT+CIPSTART=");
						strcat((char *) (command_string), "\"TCP\",\"");
						strncat((char *) (command_string), socket->address, 32);
						strcat((char *) (command_string), "\",\"");
						strncat((char *) (command_string), socket->port, 6);
						strcat((char *) (command_string), "\"\r");
					}	
					else
					{
						sprintf((char *) command_string, "\r\nAT+CIPSTART=");
						sprintf((char *) (command_string + strlen(command_string)), "%u", Connection);
						strcat((char *) (command_string), ",\"TCP\",\"");
						strncat((char *) (command_string), socket->address, 32);
						strcat((char *) (command_string), "\",\"");
						strncat((char *) (command_string), socket->port, 6);
						strcat((char *) (command_string), "\"\r");
					}
					GSM_ExecuteATCommand(command_string, &result_l, TRUE);	
					if(result_l.Ready == TRUE)
					{
						switch(result_l.answerType)
						{
							case ERROR_ANSWER:
								socket->state = TCPSocketState_Closed;
							break;
							case OK:
							case ANSWER_TIMEOUT:
							default:
								socket->state = TCPSocketState_Connecting_WaitAnswer;
							break;
						}
					}	
				break;}
				
				case TCPSocketState_Connecting_WaitAnswer:{

				break;}
				
				case TCPSocketState_Connected:
					if(QueueHasData(&socket->txQueue))
					{
						if(TransparentModeEnabled)
						{
							char command_string[50];
							int i = 0;
							while(QueueHasData(&socket->txQueue) && i < 50)
								QueueGetByte(&socket->txQueue, (unsigned char*)&command_string[i++]);
								
							GSM_SendATCommand(command_string, i);
							socket->state = TCPSocketState_Connected;
						
						}
						else
						{
							GSM_ExecuteATCommand("AT+CIPSEND?\r", &result_l, FALSE);
							socket->state = TCPSocketState_DataSending_GettingCapacity;
						}	
					}
				break;
				
				case TCPSocketState_DataSending_GettingCapacity:

				break;
				
				case TCPSocketState_DataSending_BeginSending:
					if(socket->sendCapacity > 0)
					{
						char command_string[50];							
						if(QueueHasData(&socket->txQueue) < socket->sendCapacity)
							socket->sendCapacity = QueueHasData(&socket->txQueue);
						sprintf((char *) command_string, "AT+CIPSEND=%u,%u\r", Connection, socket->sendCapacity);
						GSM_ExecuteATCommand(command_string, &result_l, FALSE);	
						socket->state = TCPSocketState_DataSending_WaitArrow;				
					}
                                        else
                                        {
                                          socket->state = TCPSocketState_Connected;
                                        
                                        
                                        }
				break;
				
				
				case TCPSocketState_DataSending_WaitArrow:
					if(result.Ready == TRUE && result.answerType == RIGHT_ARROW)
					{
						int i,j;
						for(i = 0; i < socket->sendCapacity; i+= 50)
						{
							for(j = 0; j < 50 && QueueHasData(&socket->txQueue); j++)
								QueueGetByte(&socket->txQueue, &result_l.ResultText[j]);
								
							GSM_ExecuteATCommand_withLength((char*)result_l.ResultText, &result_l, j, FALSE);	
							
						}	
						socket->state = TCPSocketState_Connected;
					}
				break;
				
				
				
				case TCPSocketState_StartClose:{
					char command_string[50];
					sprintf((char *) command_string, "AT+CIPCLOSE=%u,1\r", Connection);
					GSM_ExecuteATCommand(command_string, &result_l, TRUE);	
					socket->state = TCPSocketState_Closed;
				break;}

			}
		}	
	}
}




void ResolveDnsToStr(char* dns, char* dst)
{
	char command[50];
	
	TGSM_ATCommandResult result = {FALSE, NULL, NONE};
	result.ResultText = (unsigned char*)malloc(50);
	
	strncpy((char*)command, "AT+CDNSGIP=", 14);
	strncat((char*)command,dns,30);
	strncat((char*)command,"\r",2);
	GSM_ExecuteATCommand(command, &result, TRUE);
	
	free(result.ResultText);
}//


void GetConnectionStatuses(void)
{
	TGSM_ATCommandResult result = {FALSE, NULL, NONE};
	result.ResultText = (unsigned char*)malloc(50);
	
	GSM_SendATCommand("AT+CIPSTATUS\r\x00", strlen("AT+CIPSTATUS\r\x00"));
	while(1)
	{
		GSM_WaitAnswer(NONE, &result, TICK_SECOND*1);
		if(result.Ready == TRUE && !strncmp((char*)result.ResultText, "C: ", 3))
		{

		}
		if(!result.Ready)
			break;
	}	
	free(result.ResultText);
}
