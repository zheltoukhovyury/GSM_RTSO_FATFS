extern void StartArchTask(void);
extern void ARCH_Task(void);

typedef enum 
{
	ArchState_NotInited,
	ArchState_Init,
	ArchState_Connecting,
	ArchState_Authoriztion,
	ArchState_Idle,
	ArchState_TestState,
	
} ArchState;



typedef enum 
{
	ArchAuth_NotInited,
	ArchAuth_Authoriztion_1,
} ArchAuthorizationState;


typedef struct 
{
	UINT32 RouterNumber;
	unsigned char DevType;
	unsigned char DevSubType;
	unsigned char ProtocolVerison[2];
	unsigned char Verison[4];
	unsigned char SessionId[8];
	unsigned char crypt;
	unsigned char timeout;
	UINT32 MRU;
	
}TArchParamters;

extern void ComminicationTaskLoop(void);
extern void StartArchTask(void);
