#if !defined COMPORT_H
#define COMPORT_H

#include "GenericTypeDefs.h"

#define serOUTPUT						0
#define serINPUT						1
#define serLOW_SPEED					0
#define serONE_STOP_BIT					0
#define serEIGHT_DATA_BITS_NO_PARITY	0
#define serNORMAL_IDLE_STATE			0
#define serAUTO_BAUD_OFF				0
#define serLOOPBACK_OFF					0
#define serWAKE_UP_DISABLE				0
#define serNO_HARDWARE_FLOW_CONTROL		0
#define serSTANDARD_IO					0
#define serNO_IRDA						0
#define serCONTINUE_IN_IDLE_MODE		0
#define serUART_ENABLED					1
#define serINTERRUPT_ON_SINGLE_CHAR		0
#define serTX_ENABLE					1
#define serINTERRUPT_ENABLE				1
#define serINTERRUPT_DISABLE			0
#define serCLEAR_FLAG					0
#define serSET_FLAG						1




typedef struct{
	unsigned char PortNumber;
	unsigned char* TxQueue;
	int TxQueueLen;
	int TxQueueWrite;
	int TxQueueRead;
	
	unsigned char* RxBuff;
	int RxBufferLen;
	int RxBufferWrite;
	int RxBufferRead;
	BOOL lock;	
}ComPortHandle;


extern void ComPort_Task_Loop(void);

extern ComPortHandle* OpenPort(unsigned char PortNumber, UINT32 Baudrate);
extern void ClosePort(ComPortHandle* handle);
extern void PutByte(ComPortHandle* handle, unsigned char b);
extern BOOL GetByteForTx(ComPortHandle* handle, unsigned char* dest);
extern BOOL GetByte(ComPortHandle* handle, unsigned char* dest);
extern void PutByteToRxbuffer(ComPortHandle* handle, unsigned char b);
extern void ComPortCycle(void);
extern void FlushTx(ComPortHandle* handle);
extern void FlushRx(ComPortHandle* handle);
extern void ClearUARTErrors(void);


#endif
