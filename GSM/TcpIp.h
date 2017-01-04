#if !defined TCPIP_H
#define TCPIP_H

#include "GenericTypeDefs.h"
#include "TQueue.h"
#include "gsm.h"


typedef enum{
	TCPSocketState_NotInited,
	TCPSocketState_Closed,
	TCPSocketState_StartConnect,
	TCPSocketState_Connecting_SendCommand,
	TCPSocketState_Connecting_WaitAnswer,
	TCPSocketState_Connected,
	TCPSocketState_DataSending_GettingCapacity,
	TCPSocketState_DataSending_BeginSending,
	TCPSocketState_DataSending_WaitArrow,
	TCPSocketState_StartClose,
	TCPSocketState_TransparentPause,
}
TTcpSocketState;


typedef struct{
	TQueue txQueue;
	TQueue rxQueue;
	char* address;
	char* port;
	BOOL blocking;
	volatile TTcpSocketState state;
	int sendCapacity;
}TTcpSocket;

extern void TCPPStack(TGSM_ATCommandResult result);
extern void OpenSocket(char* address, char* port,volatile  TTcpSocket* socket, BOOL blocking);
extern BOOL SendByte(unsigned char b,volatile TTcpSocket* socket);
extern BOOL Connected(volatile TTcpSocket* socket);
extern BOOL GetData(unsigned char* dest, volatile TTcpSocket* socket);
extern int HasData(volatile TTcpSocket* socket);
extern int GetFreeBytes(volatile TTcpSocket* socket);
extern void CloseSocket(volatile TTcpSocket* socket);
extern void ResolveDnsToStr(char* dns, char* dst);
extern void GetConnectionStatuses(void);
extern void SetGPRS_TransparentMode(void);
extern void SetGPRS_MuxMode(void);

#endif //TCPIP_H
