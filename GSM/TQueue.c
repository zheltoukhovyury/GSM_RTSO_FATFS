#include "GenericTypeDefs.h"
#include "TQueue.h"
#include <stdlib.h>

void QueueCreate(TQueue* handle)
{
	handle->TxQueue = (unsigned char*)malloc(200);
	handle->TxQueueLen = 200;
	handle->TxQueueRead = 0;
	handle->TxQueueWrite = 0;
	handle->locked = FALSE;
}

void QueueDestroy(TQueue* handle)
{
	if(handle != NULL && handle->TxQueue != NULL)
	free(handle->TxQueue);
}


BOOL QueuePutByte(TQueue* handle, unsigned char b)
{
	if(handle == NULL)return FALSE;
	while(handle->locked);
	handle->locked = TRUE;
	if (handle->TxQueueWrite >= handle->TxQueueLen)
		handle->TxQueueWrite = 0;

	handle->TxQueue[handle->TxQueueWrite++] = b;
	if (handle->TxQueueWrite == handle->TxQueueRead)
		handle->TxQueueRead++;
	handle->locked = FALSE;
}//


BOOL QueueGetByte(TQueue* handle, unsigned char* dest)
{
	if(handle == NULL)return FALSE;
	
	while(handle->locked);
	handle->locked = TRUE;
	
	if (handle->TxQueueWrite == handle->TxQueueRead)
	{
		handle->locked = FALSE;
		return FALSE;
	}	

	if (handle->TxQueueRead >= handle->TxQueueLen)
	{
		handle->TxQueueRead = 0;
		handle->locked = FALSE;
		return FALSE;
	}
	*dest = handle->TxQueue[handle->TxQueueRead++];
	handle->locked = FALSE;
	return TRUE;
}//


int QueueHasData(TQueue* handle)
{
	if(handle == NULL || handle->TxQueue == NULL)return 0;	
	
	if (handle->TxQueueWrite >= handle->TxQueueRead)
		return handle->TxQueueWrite - handle->TxQueueRead;
	else if (handle->TxQueueWrite < handle->TxQueueRead)
		return (handle->TxQueueWrite + (handle->TxQueueLen - handle->TxQueueRead));
}


int QueueGetFree(TQueue* handle)
{
	if(handle == NULL)return 0;	
	if (handle->TxQueueWrite >= handle->TxQueueRead)
		return (handle->TxQueueRead + (handle->TxQueueLen - handle->TxQueueWrite));
	else if (handle->TxQueueWrite < handle->TxQueueRead)
		return handle->TxQueueRead - handle->TxQueueWrite;
}
