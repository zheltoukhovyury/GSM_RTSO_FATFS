#include "gsm.h"
#include "tick.h"
#include "VoiceCall.h"
#include "TQueue.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



TVoiceCallState VoiceCallState;
TVoiceCall VoiceCall;
TQueue DTMF_RxQueue;
TQueue DTMF_TxQueue;

char GSMDialPrefix[]="8\x00";
static TGSM_ATCommandResult result_l;
void SendDTMFMaschine(void);




void VoiceCallInit(void)
{
  VoiceCallState = VoiceCallState_Idle;
  VoiceCall.number =(char*) malloc(13);;
  result_l.Ready = FALSE;
  result_l.ResultText = (unsigned char*)malloc(50);
  
  QueueCreate(&DTMF_RxQueue);
  QueueCreate(&DTMF_TxQueue);
}

void VoiceCall_Machine(TGSM_ATCommandResult result)
{
  SendDTMFMaschine();

	

      switch(VoiceCallState)
      {
        case VoiceCallState_Incoming:	

      break;
        case VoiceCallState_Applied:
          GSM_ExecuteATCommand("ATA\r", &result_l, FALSE);
          VoiceCallState = VoiceCallState_Connected;
          break;
                
        case VoiceCallState_Idle:
                if(result.Ready == TRUE && result.answerType == RING_ANSW)
                {
                        GSM_ExecuteATCommand("AT+CLCC\r", &result_l, FALSE);
                }
                if(result.Ready == TRUE && result.answerType == CLCC_ANSW)
                {
                        VoiceCallState = VoiceCallState_Incoming;
                }	
                
        break;
                
		case VoiceCallState_StartDial:{
                  char command[20];
                  sprintf(command, "ATD%s%s;\r", GSMDialPrefix, VoiceCall.number);
                  GSM_ExecuteATCommand(command, &result_l, TRUE);
                  VoiceCallState = VoiceCallState_Dialing;
		break;}
                
		case VoiceCallState_Connected:
		case VoiceCallState_Dialing:{
			static UINT32 cllcRequestTick = 0;
			static int tryCnt = 0;
			if(result.Ready == TRUE && result.answerType == DTMF_ANSW)
			{
        result.Ready = result.Ready;
        QueuePutByte(&DTMF_RxQueue, result.ResultText[7]);

			
			}			
			
			
			if(result.Ready == TRUE && result.answerType == NO_CARRIER_ANSW)
			{
				VoiceCallState = VoiceCallState_Idle;
				break;
			}
			if(result.Ready == TRUE && result.answerType == CPAS_ANSW)
			{
				
				cllcRequestTick = TickGet();
				switch(result.ResultText[7])
				{
					case '4':
						VoiceCallState = VoiceCallState_Connected;
					break;
					case '0':
						VoiceCallState = VoiceCallState_Idle;
					break;
					default:
			 		break;
				}
			}
			if(result.Ready == TRUE && result.answerType == CLCC_ANSW)
			{
				tryCnt = 0;
				switch(result.ResultText[11])
				{
					case '2':
					case '3':
					case '4':
					case '6':
					default:
						cllcRequestTick = TickGet();
					break;
					case '0':
						GSM_ExecuteATCommand("AT+CPAS\r", &result_l, FALSE);
						cllcRequestTick = TickGet();
					break;
				}
			}
			if(TickGet() - cllcRequestTick > TICK_SECOND/2)
			{
				if(tryCnt++ > 4)
				{
					tryCnt = 0;
					VoiceCallState = VoiceCallState_StartClose;
					break;
				}
				GSM_ExecuteATCommand("AT+CLCC\r", &result_l, FALSE);
				cllcRequestTick = TickGet();
			}	
					
		break;}
		case VoiceCallState_StartClose:
			GSM_ExecuteATCommand("ATH\r", &result_l, FALSE);
			VoiceCallState = VoiceCallState_Idle;
		break;
	}
}//



void VoiceCall_Accept(BOOL answer)
{
  if(VoiceCallState == VoiceCallState_Incoming && answer == TRUE)
  {
    VoiceCallState = VoiceCallState_Applied;
  }
  else if(VoiceCallState == VoiceCallState_Incoming && answer == FALSE)
  {
    VoiceCallState = VoiceCallState_StartClose;
  
  }


}


void VoiceCall_Dial(char* number)
{
	VoiceCallState = VoiceCallState_StartDial;
	VoiceCall.number = (char*)malloc(13);
		
	memset(VoiceCall.number, 0x00, 13);
	strncpy(VoiceCall.number, number, 12);
}//



void SendDTMF(char symbol)
{
  QueuePutByte(&DTMF_TxQueue, symbol);
}//


void SendDTMFMaschine(void)
{
  if(QueueHasData(&DTMF_TxQueue))
  {
    unsigned char b;
    QueueGetByte(&DTMF_TxQueue, &b);
    
    char command[20];
    sprintf(command, "AT+VTS=\"%c\"\r", b);
    GSM_ExecuteATCommand(command, &result_l, FALSE);
  }






}
