#if !defined QUEUE_H
#define QUEUE_H

#include "GenericTypeDefs.h"


typedef struct 
{
	unsigned char* TxQueue;
	int TxQueueLen;
	int TxQueueWrite;
	int TxQueueRead;
	BOOL locked;
}TQueue;

extern void QueueCreate(TQueue* handle);
extern void QueueDestroy(TQueue* handle);
extern BOOL QueuePutByte(TQueue* handle, unsigned char by);
extern BOOL QueueGetByte(TQueue* handle, unsigned char* dest);
extern int QueueHasData(TQueue* handle);
extern int QueueGetFree(TQueue* handle);

#endif
