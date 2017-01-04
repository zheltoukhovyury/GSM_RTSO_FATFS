#include "gsm.h"
#include "SMS.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TSMS_Machine_State SMS_Machine_State = SMS_Machine_Idle;
char* SMS_SendText = NULL;



TSMS_Receive SMS_Receive = {SMS_Receive_NoMessage, NULL, NULL};


void SMS_Send(char* number, char* Text)
{
	if(SMS_SendText == NULL)
		SMS_SendText = (char*)malloc(strlen(Text));
		
	if(SMS_Machine_State != SMS_Machine_Idle)
		return;
		
	strncpy(SMS_SendText, Text, strlen(Text));
		

}

static TGSM_ATCommandResult result_l = {FALSE, NULL, NONE};


void SMS_Machine(TGSM_ATCommandResult result)
{
	static int SMS_ReceiveText_pointer = 0;
	if(result_l.ResultText == NULL)
		result_l.ResultText = (unsigned char*)malloc(50);

	if(SMS_Receive.SMS_ReceiveStatus == SMS_Receive_Processed)
	{
		if(SMS_Receive.message != NULL)
			free(SMS_Receive.message);
		if(SMS_Receive.number != NULL)
			free(SMS_Receive.number);
	}


	
	if(result.Ready == TRUE)
	{
		switch(result.answerType)
		{
			case CMGR_ANSW:{
				char* pointer = strchr((char*)result.ResultText,',');
				SMS_ReceiveText_pointer = 0;
				if(SMS_Receive.number == NULL)
					SMS_Receive.number = (char*)malloc(11);
				if(SMS_Receive.message == NULL)
					SMS_Receive.message = (char*)malloc(141);
				strncpy(SMS_Receive.number, pointer + 2,10);
				break;
			}
			case CMTI_ANSW:{
				int smsIndex = atoi(strchr((char*)result.ResultText,',') + 1);
				GSM_ExecuteATCommand("AT+CMGF=1\r", &result_l, TRUE);
				GSM_ExecuteATCommand("AT+CSCS=\"GSM\"\r", &result_l, TRUE);
				char command[20];
				sprintf(command,"AT+CMGR=%u,0\r",smsIndex);
				GSM_ExecuteATCommand(command, &result_l, TRUE);

			break;}	
			case SMSSymbolRECEIVED:{
				if(result.ExtraArgument)
					SMS_Receive.message[SMS_ReceiveText_pointer++] = result.ResultText[0];
				else
				{
					SMS_Receive.SMS_ReceiveStatus = SMS_Receive_NewMessage;
				}	
			}	
			
			default:
			break;
		}
	}		
	
	
	switch(SMS_Machine_State)
	{
		case SMS_Machine_Idle:
			
		break;	
	}
} 
