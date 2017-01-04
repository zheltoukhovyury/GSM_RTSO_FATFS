#include "gsm.h"
#include "GenericTypeDefs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tick.h"
#include "TcpIp.h"
#include "SMS.h"
#include "VoiceCall.h"
#include "ComPort.h"
#include <stdlib.h>
#include <string.h>
#include "gpio.h"
#include "stm32f4xx_iwdg.h"
#include "tm_stm32f4_usb_vcp.h"

#define MAX_SIM 1

TSIM SIMstate[MAX_SIM];
int CurrentSimNum = 0;

TGSM_GeneralState GSM_GeneralState = GSM_NotInited;
TGSM_PowerCycle GSM_PowerCycle = GSM_PowerCycle_Idle;
GSM_Task_InitParam parmeters;
ComPortHandle* GSM_Port = NULL;

char* ReceivedMessage = NULL;



TGSM_ATCommandResult __result;//глобальна€ что б было видно при отладке
unsigned char __resulBuff[100];

static TGSM_ATCommandResult result_l;


void GSM_Task_Loop(void)
{

  __result.Ready = FALSE;
  //__result.ResultText = malloc(100);
  __result.ResultText = __resulBuff;
  
  result_l.Ready = FALSE;
  result_l.ResultText = (unsigned char*)malloc(100);

  
  GSM_Port = OpenPort(1,115200);
  
  VoiceCallInit();
  
  while(1)
  {
    GetNextResult(&__result);
    GSM_Task(__result);
    GSMPowerCycle();
    TCPPStack(__result);
    SMS_Machine(__result);
    VoiceCall_Machine(__result);
    IWDG_ReloadCounter();

    
  }
}//



UINT32 testTick = 0;





void StartGSM(void)
{



}


void StopGSM(void)
{
	if(GSM_GeneralState != GSM_TurnedOFF)
		GSM_GeneralState = GSM_TurningOFF;
}//



void GSM_Task(TGSM_ATCommandResult result)
{
	static UINT32 time=0;
	
	switch(GSM_GeneralState)
	{
		case GSM_NotInited:
		


			GSM_GeneralState = GSM_Inited;
	
                        int i;
                        for(i = 0; i < MAX_SIM; i++)
                        {
                          SIMstate[i].SIMstate = NotChecked;
                          SIMstate[i].SIMOperatorName = UNKNOWN;
                        }
		break;
		case GSM_Inited:{
			GSM_GeneralState = GSM_TurningON;
			FlushTx(GSM_Port);
			FlushRx(GSM_Port);
		break;}
		case GSM_TurningON:{
			BOOL moduleActive = FALSE;
			
			FlushTx(GSM_Port);
			FlushRx(GSM_Port);
			
			while(GSMPowerTurnOn() == FALSE)
				GSMPowerCycle();
			
                        GSM_SendATCommand("AT+CPIN?\r\n", strlen("AT+CPIN?\r\n"));

                        
			int i;
			for(i = 0; i < 5; i++)
			{
                          IWDG_ReloadCounter();
                          GSM_WaitAnswer(NONE, &result_l, TICK_SECOND*1);
                          if(result_l.Ready == TRUE && result_l.answerType == CPIN_ANSW)
                          {
                            moduleActive = TRUE;
                            if(!strncmp((char*)&result_l.ResultText[7], "READY", 5))
                              SIMstate[CurrentSimNum].SIMstate = Inserted;
                            else
                              SIMstate[CurrentSimNum].SIMstate = NotInserted;
                          }
                          if(result_l.Ready == TRUE && !strncmp((char*)result_l.ResultText, "Call Ready",10))
                          {
                            moduleActive = TRUE;
                            break;				
                          }
			}
                        
                        GSM_ExecuteATCommand("AT+CPIN?\r\n", &result_l, TRUE);
                        

                        if(!moduleActive)
                        {
                          FlushTx(GSM_Port);
                          FlushRx(GSM_Port);
                          
                          uint32_t baudRateList[]={115200, 9600};
                          int i;
                          BOOL speedSet = FALSE;
                          for(i = 0; i < 1 && !speedSet; i++)
                          {
                            ClosePort(GSM_Port);
                            GSM_Port = OpenPort(1,baudRateList[i]);
                            
                            GSM_SendATCommand("AT\r\n", strlen("AT\r\n"));
                            int t;
                            for(t = 0; t < 3; t++)
                            {
                              GSM_WaitAnswer(NONE, &result_l, TICK_SECOND/2);
                              if(result_l.Ready == TRUE && result_l.answerType == OK)
                              {
                                speedSet = TRUE;
                                break;
                              }
                            }
                          }
                          IWDG_ReloadCounter();
                          GSM_ExecuteATCommand("ATE0\r\n", &result_l, TRUE);
                          GSM_ExecuteATCommand("AT+IFC?\r\x00", &result_l, TRUE);
                          GSM_ExecuteATCommand("AT+IFC=0,0\r\x00", &result_l, TRUE);
                          GSM_ExecuteATCommand("AT+CSMINS?\r\x00", &result_l, TRUE);
                          GSM_ExecuteATCommand("AT+CSMINS=1\r\x00", &result_l, TRUE);
                          GSM_ExecuteATCommand("AT&W\r\x00", &result_l, TRUE); 
                          IWDG_ReloadCounter();              
                          
                          while(GSMPowerTurnOff() == FALSE)
                                  GSMPowerCycle();
                          //перезапуск модул€
                          break;
			}
                        
                        BOOL NoSim = TRUE;
                        for(i = 0; i < MAX_SIM; i++)
                        {
                          
                          if(SIMstate[i].SIMstate == Inserted)
                            NoSim = FALSE;
                          if(SIMstate[i].SIMstate == NotChecked)
                          {
                                while(GSMPowerTurnOff() == FALSE)
                                        GSMPowerCycle();
                                        
                                SelectSIM(SIMselectin_Togle);
                                //перезапуск модул€
                                break;			
                          }
                        }
                        if(NoSim)
                        {
                                while(GSMPowerTurnOff() == FALSE)
                                        GSMPowerCycle();
                                        
                                SelectSIM(SIMselectin_Togle);
                                break;
                                //перезапуск модул€
                        }
/*
			//если вставлена 1-€ симка переключене на нее
			if(GSM_SIM_Switcher_Get() == GSM_SIM_Switcher_SIM2 && SIMstate[0].SIMstate == Inserted)
			{
				while(GSMPowerTurnOff() == FALSE)
					GSMPowerCycle();
				SelectSIM(SIMselectin_SIM1);
				
				//перезапуск модул€
				break;
			}
			
			//переключене на вставленную сим-ку
			if((GSM_SIM_Switcher_Get() == GSM_SIM_Switcher_SIM1 && SIMstate[0].SIMstate != Inserted) || (GSM_SIM_Switcher_Get() == GSM_SIM_Switcher_SIM2 && SIMstate[1].SIMstate != Inserted))
			{
				while(GSMPowerTurnOff() == FALSE)
					GSMPowerCycle();
					
				SelectSIM(SIMselectin_Togle);
				break;
			}
*/

			SIMstate[CurrentSimNum].SIMstate = NetworkSearch;
			
			GSM_GeneralState = GSM_NetworkSearching;
			break;
			
		case GSM_NetworkSearching:

			FlushTx(GSM_Port);
			FlushRx(GSM_Port);
			GSM_ExecuteATCommand("AT+CREG?\r", &result_l, TRUE);
			if(result_l.Ready == TRUE && result_l.answerType == CREG)
			{
				if(result_l.ResultText[7] == (unsigned char)'0' && result_l.ResultText[9] == (unsigned char)'1')
				{
					GSM_GeneralState = GSM_SetiingUp;
					break;
				}
			}
			break;
		case GSM_SetiingUp:	
		
			SetGPRS_MuxMode();
			GSM_ExecuteATCommand("AT+DDET=1\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+IFC=2,2\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CLTS=1\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CMEE=0\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+AUTEST=0,1\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CDNSCFG=\"8.8.8.8\",\"8.8.0.0\"\r", &result_l, TRUE);
			GSM_ExecuteATCommand("ATE0\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+DDET=1\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CGMR\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CBUZZERRING=1\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CRSL=100\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CLVL=10\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+VTD=1\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CHF=0,0\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CHF=1,1\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT+CMIC=1,9\r", &result_l, TRUE);	
			GSM_ExecuteATCommand("AT+ECHO=1,7,6\r", &result_l, TRUE);	
			GSM_ExecuteATCommand("AT+CMGF=1\r", &result_l, TRUE);	
			
			//GSM_ExecuteATCommand("AT+CMGDA=\"DEL ALL\"\r", &result_l, TRUE);			
			
			GSM_ExecuteATCommand("AT+CIPSHUT\r", &result_l, TRUE);	
			GSM_ExecuteATCommand("AT+CIPHEAD=1\r", &result_l, TRUE);
			
			extern BOOL TransparentModeEnable;
			extern BOOL TransparentModeEnabled;
			if(TransparentModeEnable)	
			{
				GSM_ExecuteATCommand("AT+CIPMUX=0\r", &result_l, TRUE);	
				GSM_ExecuteATCommand("AT+CIPMODE=1\r", &result_l, TRUE);
				TransparentModeEnabled = TRUE;
			}	
			else
			{
				GSM_ExecuteATCommand("AT+CIPMUX=1\r", &result_l, TRUE);				
				GSM_ExecuteATCommand("AT+CIPMODE=0\r", &result_l, TRUE);
				TransparentModeEnabled = FALSE;
			}	
				
			GSM_ExecuteATCommand("AT+CIPSRIP=0\r", &result_l, TRUE);
			GSM_ExecuteATCommand("AT&W\r", &result_l, TRUE);			
			
			GSM_ExecuteATCommand("AT+COPS?\r", &result_l, TRUE);
			if(result_l.Ready == TRUE && result_l.answerType == COPS)
			{
				int i;
				int commaCnt = 0;
				
				for(i = 0; i < strlen((char*)result_l.ResultText); i++)
				{
					if((char)result_l.ResultText[i] == ',')
						commaCnt++;
					if(commaCnt == 2)
					{
						if(strncmp((char*)&result_l.ResultText[i + 2], "MTS", 3) == 0)
							SIMstate[CurrentSimNum].SIMOperatorName = MTS;
						else if(strncmp((char*)&result_l.ResultText[i + 2], "TELE2", 5) == 0)
							SIMstate[CurrentSimNum].SIMOperatorName = TELE2;
						else if(strncmp((char*)&result_l.ResultText[i + 2], "Beeline", 7) == 0)
							SIMstate[CurrentSimNum].SIMOperatorName = BeeLine;
						else if(strncmp((char*)&result_l.ResultText[i + 2], "MegaFon", 5) == 0)
							SIMstate[CurrentSimNum].SIMOperatorName = MegaFon;
						else if(strncmp((char*)&result_l.ResultText[i + 2], "TELE2", 5) == 0)
							SIMstate[CurrentSimNum].SIMOperatorName = TELE2;
						break;
					}
				}
			}	
			
			

			GSM_WaitAnswer(OK, &result_l, TICK_SECOND*1);
			SIMstate[CurrentSimNum].SIMstate = GPRSInitilizing;
			GSM_GeneralState = GSM_GPRSInit;
			//GSM_GeneralState = GSM_TurnedON;
			
			//GSM_ExecuteATCommand("AT+CMGR=1,1\r", &result_l, FALSE);				
			//VoiceCall_Dial("9601346889");

		break;
		case GSM_GPRSInit:
			time = TickGet();
			while(1)
			{
				if(TickGet() - time > TICK_SECOND*90)
					break;
				switch (SIMstate[CurrentSimNum].SIMOperatorName)
				{
					case TELE2:
						GSM_ExecuteATCommand("AT+CSTT=\"internet.tele2.ru\",\"tele2\",\"tele2\"\r", &result_l, TRUE);	
						break;
					case MTS:
						GSM_ExecuteATCommand("AT+CSTT=\"internet.mts.ru\",\"mts\",\"mts\"\r", &result_l, TRUE);	
						break;
					case MegaFon:
						GSM_ExecuteATCommand("AT+CSTT=\"internet\",\"\",\"\"\r", &result_l, TRUE);	
						break;
					case BeeLine:
						GSM_ExecuteATCommand("AT+CSTT=\"internet.beeline.ru\",\"beeline\",\"beeline\"\r", &result_l, TRUE);	
						break;
					case Kolibri:
						GSM_ExecuteATCommand("AT+CSTT=\"internet.mts.ru\",\"mts\",\"mts\"\r", &result_l, TRUE);	
						break;
					default:
						GSM_ExecuteATCommand("AT+CSTT=\"internet.mts.ru\",\"mts\",\"mts\"\r", &result_l, TRUE);	
						break;
				}
				if(result_l.Ready == TRUE && result_l.answerType == OK)
				{
					//CurrentGSMNetworkError = FALSE;
					break;
				}
			}	
			
			if(1)
			{

				time = TickGet();
				while(1)
				{
					if(TickGet() - time > TICK_SECOND*90)
						break;
					GSM_ExecuteATCommand("AT+CIICR\r", &result_l, TRUE);	
					if(result_l.Ready == TRUE && result_l.answerType == OK)
					{

						break;
					}
				}
			}	
			
			GSM_ExecuteATCommand("AT+CIFSR\r", &result_l, TRUE);	

			
			if(1)
				SIMstate[CurrentSimNum].SIMstate = NoGPRS;
			else
				SIMstate[CurrentSimNum].SIMstate = ActiveGPRS;
				
			GSM_GeneralState = GSM_TurnedON;


		break;}
		case GSM_TurnedON:{
			//StopGSM();
		break;}
		
		
		case GSM_TurningOFF:{
			while(GSMPowerTurnOff() == FALSE)
			GSMPowerCycle();
			GSM_GeneralState = GSM_TurnedOFF;
		break;}
				
		case GSM_TurnedOFF:{
			
		break;}
	}
}//



static UINT32 PowerHandleTick = 0;

BOOL GSMPowerTurnOn(void)
{
	if(GSM_PowerCycle == GSM_PowerCycle_TurnedON)
		return TRUE;
	switch(GSM_PowerCycle)
	{
		case GSM_PowerCycle_Idle:
		case GSM_PowerCycle_TurnedOFF:
			GSM_PowerCycle = GSM_PowerCycle_TurnON_0;
		break;
		default:
		break;
	}
	return FALSE;
}

BOOL GSMPowerTurnOff(void)
{
	if(GSM_PowerCycle == GSM_PowerCycle_TurnedOFF)
		return TRUE;
	switch(GSM_PowerCycle)
	{
		case GSM_PowerCycle_Idle:
		case GSM_PowerCycle_TurnedON:
			GSM_PowerCycle = GSM_PowerCycle_TurnOFF_0;
		break;
		default:
		break;
	}
	return FALSE;
}


void SelectSIM(unsigned char selection)
{
	if(selection == SIMselectin_SIM1)
		GSM_SIM_Switcher_Set(GSM_SIM_Switcher_SIM1);
	if(selection == SIMselectin_SIM2)		
		GSM_SIM_Switcher_Set(GSM_SIM_Switcher_SIM2);	
	else
	{
		if(GSM_SIM_Switcher_Get() == GSM_SIM_Switcher_SIM1)
			GSM_SIM_Switcher_Set(GSM_SIM_Switcher_SIM2);
		else
			GSM_SIM_Switcher_Set(GSM_SIM_Switcher_SIM1);
	}		


}


void GSMPowerCycle(void)
{
	switch(GSM_PowerCycle)
	{
		case GSM_PowerCycle_Idle:
		case GSM_PowerCycle_TurnedON:
		case GSM_PowerCycle_TurnedOFF:
			break;	
			
		//включение	
		case GSM_PowerCycle_TurnON_0:
			GSM_POWERPIN_Set(0);
			GSM_POWERKEY_Set(0);
			PowerHandleTick = TickGet();
			GSM_PowerCycle = GSM_PowerCycle_TurnON_1;
		break;
		case GSM_PowerCycle_TurnON_1:
			if(TickGet() - PowerHandleTick < TICK_SECOND)break;
			GSM_POWERKEY_Set(1);
			PowerHandleTick = TickGet();
			GSM_PowerCycle = GSM_PowerCycle_TurnON_2;	
			break;			
		case GSM_PowerCycle_TurnON_2:
			if(TickGet() - PowerHandleTick < TICK_SECOND*1.5)break;
			GSM_POWERKEY_Set(0);
			PowerHandleTick = TickGet();
			GSM_PowerCycle = GSM_PowerCycle_TurnON_3;
		case GSM_PowerCycle_TurnON_3:
			if(TickGet() - PowerHandleTick < TICK_SECOND*2)break;
			GSM_PowerCycle = GSM_PowerCycle_TurnedON;
			break;

			
		//выключение
		case GSM_PowerCycle_TurnOFF_0:
			GSM_SendATCommand("AT+CPOWD=1\r\0x00", strlen("AT+CPOWD=1\r\0x00"));
			GSM_POWERPIN_Set(0);
			//GSM_POWERKEY_Set(1);
			PowerHandleTick = TickGet();
			GSM_PowerCycle = GSM_PowerCycle_TurnOFF_1;
		break;
		case GSM_PowerCycle_TurnOFF_1:
			if(TickGet() - PowerHandleTick < TICK_SECOND)break;
			//GSM_POWERKEY_Set(1);
			PowerHandleTick = TickGet();
			GSM_PowerCycle = GSM_PowerCycle_TurnOFF_2;	
			break;			
		case GSM_PowerCycle_TurnOFF_2:
			if(TickGet() - PowerHandleTick < TICK_SECOND)break;
			//GSM_POWERKEY_Set(0);
			GSM_POWERPIN_Set(1);
			PowerHandleTick = TickGet();
			GSM_PowerCycle = GSM_PowerCycle_TurnOFF_3;
		case GSM_PowerCycle_TurnOFF_3:
			if(TickGet() - PowerHandleTick < TICK_SECOND)break;

			GSM_PowerCycle = GSM_PowerCycle_TurnedOFF;
			break;
	}
}


unsigned char answer_cpy[100];


void GSM_SendATCommand(char* command, int len)
{
	int i;
	for(i = 0; i < len; i++)
		PutByte(GSM_Port, (unsigned char)command[i]);
}//


unsigned char CommandBuffer[100];
void GetNextResult(TGSM_ATCommandResult* result)
{
	static int i = 0;
	static int a = 0;

	char* answer = (char*)(result->ResultText);
	static unsigned char terminator[2];
	static int TCPIPRceivingLen = 0;
	static unsigned char TCPIPRceivingConnectionNum = 0;
	volatile static BOOL SMSReceiving = FALSE;
	static char prevSmsSymbol;
	
	
	result->answerType = NO_ANSWER;
	result->Ready = FALSE;
	
	while(1)
	{
		//прием и передача байтика по GPRS
		if(TCPIPRceivingLen && GetByte(GSM_Port, (unsigned char*)&answer[0]))
		{
			TCPIPRceivingLen--;
			result->ExtraArgument = TCPIPRceivingConnectionNum;
			result->answerType = TCPIPDATARECEIVED;
			result->Ready = TRUE;
			return;
		}
		else if(TCPIPRceivingLen)
			return;
	
			
			
		//прием и передача символа —ћ—
		if(SMSReceiving  && GetByte(GSM_Port, (unsigned char*)&answer[0]))
		{
			
			if(prevSmsSymbol == '\r' && (char)answer[0] == '\n')
			{
				result->ExtraArgument = 0;
				SMSReceiving = FALSE;
			}
			else
				result->ExtraArgument = 1;
			prevSmsSymbol = answer[0];
			
			result->answerType = SMSSymbolRECEIVED;
			result->Ready = TRUE;
			return;		
		}
		else if(SMSReceiving)
			return;		
			
		extern BOOL TransparentModeEnabled;
		extern TTcpSocket* socket_List[];
		if(TransparentModeEnabled && socket_List[0] != NULL && socket_List[0]->state == TCPSocketState_Connected)
		{
			//при открытом соединении и прозрачном режиме байтик €вл€етс€ байтиком от сервера
			if(GetByte(GSM_Port, (unsigned char*)&answer[0]))
			{
				result->ExtraArgument = TCPIPRceivingConnectionNum;
				result->answerType = TCPIPDATARECEIVED;
				result->Ready = TRUE;

				if(a < 100)
					answer_cpy[a++] = answer[0];

				terminator[0] = terminator[1];
				terminator[1] = answer[0];
				//и так же провер€етс€ не €вл€етс€ ли байтик чать команды модул€
				
				CommandBuffer[i] = answer[0];
				i++;
			}
			else
				return;	
		}		
		else if(GetByte(GSM_Port, (unsigned char*)&CommandBuffer[i]))
		{
			if(a < 100)
				answer_cpy[a++] = CommandBuffer[i];
				
			terminator[0] = terminator[1];
			terminator[1] = CommandBuffer[i];
			i++;
		}
		else
			return;
			
			
		if(i == 1 &&  CommandBuffer[i - 1] == '>' && !TransparentModeEnabled)
		{
			result->answerType = RIGHT_ARROW;
			result->Ready = TRUE;
			return;
		}
		else if(i > 1 && strncmp((char*)terminator, "\r\n", 2) == 0)
		{

			if(i == 2)
			{
				i = 0;
				a = 0;
				continue;
			}
			else
			{
				
				a = 0;
				if(strncmp((char*)CommandBuffer, "+RECEIVE", 8) == 0)
				{
					TCPIPRceivingLen = atoi((const char*)&CommandBuffer[11]);
					TCPIPRceivingConnectionNum= atoi((const char*)&CommandBuffer[9]);
				}
				if(strncmp((char*)CommandBuffer, "+CMGR:", 6) == 0)
				{
					char* pointer;
					pointer = strchr((char*)CommandBuffer, ',');
					SMSReceiving = TRUE;
				}				
				
				result->answerType = GetResultType((unsigned char*)CommandBuffer);
				memcpy(answer, CommandBuffer, i);
				result->Ready = TRUE;
				
				i = 0;
				return;
			}
		}
		else if (result->answerType == TCPIPDATARECEIVED && result->Ready == TRUE)
			return;

	}	
	

}//


void GSM_WaitAnswer(TGSM_ATCommandAnswer waited_answer, TGSM_ATCommandResult* result, UINT32 timeout)
{
	result->Ready = FALSE;
	unsigned char* answer = result->ResultText;
	int i = 0;
	int a = 0;
	unsigned char terminator[2];
	UINT32 time = TickGet();

	while(1)
	{
		if(TickGet() - time > timeout)
		{
			result->answerType = ANSWER_TIMEOUT;
			return;
		}
		if(GetByte(GSM_Port, &answer[i]))
		{
			if(a < 100)
				answer_cpy[a++] = answer[i];
				
			terminator[0] = terminator[1];
			terminator[1] = answer[i];
			i++;
		}
		if(i > 1 && strncmp((char*)terminator, "\r\n", 2) == 0)
		{
			if(i == 2)
			{
				i = 0;
				continue;
			}
			else if(waited_answer != NONE)
			{
				if(waited_answer == GetResultType(answer))
				{
					result->answerType = GetResultType(answer);
					result->Ready = TRUE;
					break;
				}
			}
			else
				break;
		}
	}

	
	result->Ready = TRUE;
	result->answerType = GetResultType(answer);
	return;
}


void GSM_ExecuteATCommand(char* command, TGSM_ATCommandResult* result, BOOL Blocking)
{
	GSM_ExecuteATCommand_withLength(command, result, strlen(command), Blocking);
}//


volatile unsigned char terminator[2];
unsigned char rb;
void GSM_ExecuteATCommand_withLength(char* command, TGSM_ATCommandResult* result, int len, BOOL Blocking)
{
	int i;
	int a = 0;
	UINT32 time = 0;
	
	extern BOOL TransparentModeEnabled;
	extern TTcpSocket* socket_List[];
	if(TransparentModeEnabled && socket_List[0] != NULL && socket_List[0]->state == TCPSocketState_Connected)	
	{
		result->Ready = TRUE;
		result->answerType = ANSWER_TIMEOUT;
		return;
	}	
	
	
	result->Ready = FALSE;
	result->answerType = NO_ANSWER;
	
	GSM_SendATCommand(command, len);
	
	if(Blocking)
	{
		result->Ready = FALSE;
		unsigned char* answer = result->ResultText;
		
		i = 0;

		time = TickGet();

		while(1)
		{
			if(TickGet() - time > 3 * TICK_SECOND)
			{
				result->Ready = TRUE;
				result->answerType = ANSWER_TIMEOUT;
				return;
			}

			if(GetByte(GSM_Port, &rb))
			{
                          answer[i] = rb;
				if(a < 100)
					answer_cpy[a++] = rb;
					
				terminator[0] = terminator[1];
				terminator[1] = rb;
				i++;
			}
			if(i > 1 && strncmp((char*)terminator, "\r\n", 2) == 0)
			{
				if(i == 2)
				{
					i = 0;
					a = 0;
					continue;
				}
				else
				{
					result->Ready = TRUE;
					result->answerType = GetResultType(answer);
					return;
				}
			}
		}

	}
}//


//RS238486171CN


char lastRes[20];

TGSM_ATCommandAnswer GetResultType(unsigned char* str)
{
	memcpy(lastRes,str,20);
	
	char* string = (char*)str;
	if(!strncmp(string, "AT\r\n", 4))
		return AT;
	else if(!strncmp(string, "OK",2))
		return OK;
	else if(!strncmp(string, "ERROR",2))
		return ERROR_ANSWER;
	else if(!strncmp(string, "+CPIN", 5))
		return CPIN_ANSW;
	else if(!strncmp(string, "+CREG:",6))
		return CREG;
	else if(!strncmp(string, "+COPS:",6))
		return COPS;
	else if(!strncmp(&string[3], "CONNECT OK",10))
		return MUX_CONNECT_OK;
	else if(!strncmp(&string[3], "CLOSED",6))
		return MUX_CLOSED;
	else if(!strncmp(string, "+CMTI:",6))
		return CMTI_ANSW;
	else if(!strncmp(string, "+CMGR:",6))
		return CMGR_ANSW;
	else if(!strncmp(string, "+CLCC:",6))
		return CLCC_ANSW;
	else if(!strncmp(string, "+CPAS:",6))
		return CPAS_ANSW;
	else if(!strncmp(string, "NO CARRIER",10))
		return NO_CARRIER_ANSW;
	else if(!strncmp(string, "+DTMF:",6))		
		return DTMF_ANSW;
	else if(!strncmp(string, "RING",4))		
		return RING_ANSW;		
	else if(!strncmp(string, "CONNECT",6))		
		return TRNPT_CONNECT_OK;				
	else if(!strncmp(string, "CLOSED",5))		
		return TRNPT_CLOSED;						
	else if(!strncmp(string, "CONNECT FAIL",12))		
		return TRNPT_CONNECT_FAIL;
	else if(!strncmp(string, "+CIPSEND:",9))		
		return CIPSEND_CAPACITY;
	else
	{
		return UNKNOWN_ANSW;
	}	
}




