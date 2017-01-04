#if !defined GSM_H
#define GSM_H

#include "GenericTypeDefs.h"

#define GSM_SIM_Switcher_SIM1		1	
#define GSM_SIM_Switcher_SIM2		0


typedef struct 
{
  int a;

}GSM_Task_InitParam;



typedef enum 
{
	TGSMstate_TurnOff,
	TGSMstate_TurnOn,
}TGSMstate;


typedef enum
{
	GSM_NotInited,
	GSM_Inited,
	GSM_TurningON,
	GSM_TurnedON,
	GSM_NetworkSearching,
	GSM_SetiingUp,
	GSM_GPRSInit,
	GSM_TurningOFF,
	GSM_TurnedOFF,
}TGSM_GeneralState;



typedef enum
{
	GSM_PowerCycle_Idle,
	GSM_PowerCycle_TurnON_0,
	GSM_PowerCycle_TurnON_1,
	GSM_PowerCycle_TurnON_2,
	GSM_PowerCycle_TurnON_3,
	GSM_PowerCycle_TurnedON,
	GSM_PowerCycle_TurnOFF_0,
	GSM_PowerCycle_TurnOFF_1,
	GSM_PowerCycle_TurnOFF_2,
	GSM_PowerCycle_TurnOFF_3,
	GSM_PowerCycle_TurnedOFF,	
}TGSM_PowerCycle;


typedef enum {
	NO_ANSWER,
	NEW_ANSWER,
	OK,
	ERROR_ANSWER,
	CREG,
	AT,
        AT_ANSW,
	COPS,
	STATE,
	CONNECTION_N,
	MUX_CONNECT_OK,
	MUX_CLOSED,
	TRNPT_CONNECT_OK,
	TRNPT_CLOSED,
	TRNPT_CONNECT_FAIL,
	CONNECT_FAIL_ANS,
	SEND_OK_ANS,
	SEND_FAIL_ANS,
	CIPSEND_CAPACITY,
	RIGHT_ARROW,
	UNKNOWN_ANSW,
	ALREADY_CONNECT_ANSW,
	CLOSE_OK_ANSW,
	CLOSED_ANSW,
	RECEIVE_ANSW,
	RING_ANSW,
	CLCC_ANSW,
	CONNECT_9600,
	NO_CARRIER_ANSW,
	BUSY_ANSW,
	CPAS_ANSW,
	CSQ_ANSW,
	CUSD_ANSW,
	CMTI_ANSW,
	CMGL_ANSW,
	CMGR_ANSW,
	SHUT_OK_ANSW,
	CMGS_ANSW,
	NOANSWER_ANSW,
	CPIN_ANSW,
	CFUN_ANSW,
	StatusRequest,
	StatusAnswer,
	StartConnection,
	CloseConnection,
	SendData,
	RequestNewData,
	AnswerNewData,
	AnswerTime,
	IMEI,
	CSMINS_ANSW,
	SJDR_ANSW,
	CDNSGIP_ANSW,
	DTMF_ANSW,
	TCPIPDATARECEIVED,
	SMSSymbolRECEIVED,
	NONE,
	ANSWER_TIMEOUT,
} TGSM_ATCommandAnswer;


typedef enum
{
	NotChecked,
	Inserted,
	NotInserted,
	NetworkSearch,
	GPRSInitilizing,
	ActiveGPRS,
	NoGPRS,
}TSIMstate;

typedef enum {
	TELE2 = 1,
	BeeLine = 2,
	MTS = 3,
	MegaFon = 4,
	Kolibri = 5,
	UNKNOWN = 0xFF,
} TSIM_OperatorName;

typedef struct
{
	TSIMstate SIMstate;
	TSIM_OperatorName SIMOperatorName;
}TSIM;



typedef struct 
{
	BOOL Ready;
	unsigned char* ResultText;
	TGSM_ATCommandAnswer answerType;
	unsigned char ExtraArgument;
} TGSM_ATCommandResult;


extern void GSM_Task_Loop(void);

extern void setGsmState(TGSMstate state);
extern void GSM_Task(TGSM_ATCommandResult result);
extern BOOL GSMPowerTurnOn(void);
extern BOOL GSMPowerTurnOff(void);
extern void GSMPowerCycle(void);
extern void SelectSIM(unsigned char selection);
extern void GetNextResult(TGSM_ATCommandResult* result);
extern void GSMUnsolicitedProcessing(char ch);
extern void GSM_WaitAnswer(TGSM_ATCommandAnswer waited_answer, TGSM_ATCommandResult* result, UINT32 timeout);
extern void GSM_ExecuteATCommand(char* command, TGSM_ATCommandResult* result, BOOL Blocking);
extern void GSM_ExecuteATCommand_withLength(char* command, TGSM_ATCommandResult* result, int len, BOOL Blocking);
extern void GSM_SendATCommand(char* command, int len);
extern TGSM_ATCommandAnswer GetResultType(unsigned char* str);
extern TSIM SIMstate[];
extern void StopGSM(void);

//system ticks
extern UINT32 TickGet(void);
extern UINT32 sysTick;




extern TGSM_GeneralState GSM_GeneralState;


#define SIMselectin_SIM1	1
#define SIMselectin_SIM2	2
#define SIMselectin_Togle	3

#endif
