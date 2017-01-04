#if !defined VOICECALL_H
#define VOICECALL_H
#include "GenericTypeDefs.h"
#include "gsm.h"
#include "TQueue.h"

typedef enum
{
	VoiceCallState_Idle,
	VoiceCallState_Incoming,
	VoiceCallState_StartDial,
	VoiceCallState_Dialing,
	VoiceCallState_Connected,
	VoiceCallState_StartClose,
        VoiceCallState_Applied,

}TVoiceCallState;



typedef struct
{
	char* number;
}TVoiceCall;


extern TQueue DTMF_RxQueue;
extern TQueue DTMF_TxQueue;


extern void VoiceCallInit(void);
extern void VoiceCall_Machine(TGSM_ATCommandResult result);
extern void VoiceCall_Accept(BOOL answer);
extern void VoiceCall_Dial(char* number);
extern void SendDTMF(char symbol);

extern TVoiceCallState VoiceCallState;



#endif
