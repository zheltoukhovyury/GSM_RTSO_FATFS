
typedef enum
{
	SMS_Machine_Idle,
}TSMS_Machine_State;


typedef enum 
{
	SMS_Receive_NoMessage,
	SMS_Receive_NewMessage,
	SMS_Receive_Processed,
}TSMS_ReceiveStatus;

typedef struct 
{
	TSMS_ReceiveStatus SMS_ReceiveStatus;
	char* message;
	char* number;
}TSMS_Receive;


extern void SMS_Machine(TGSM_ATCommandResult result);
extern void SMS_Send(char* number, char* Text);
