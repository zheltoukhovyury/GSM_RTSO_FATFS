#include "ComPort.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include <stdlib.h>


#include "misc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "tm_stm32f4_usb_vcp.h"

#include "tick.h"

ComPortHandle COMPORT1 = {0, NULL};
ComPortHandle COMPORT2 = {0, NULL};
unsigned char rxTestBuff[200];
unsigned char txTestBuff[200];

unsigned char rxTestBuff2[200];



ComPortHandle* OpenPort(unsigned char PortNumber, UINT32 Baudrate)
{
  switch(PortNumber)
  {
    case 1:{
  
      
      USART_DeInit(UART4);
      
      GPIO_InitTypeDef GPIO_InitStructure;   
      USART_InitTypeDef  USART_InitStructure; 
  
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
      GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4); 
      GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4); 
     
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;   
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;   
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
      GPIO_Init(GPIOC, &GPIO_InitStructure); 
     
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);      
      USART_InitStructure.USART_BaudRate = Baudrate;
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      USART_InitStructure.USART_StopBits = USART_StopBits_1;   
      USART_InitStructure.USART_Parity = USART_Parity_No;   
      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
      USART_Init(UART4, &USART_InitStructure); 
     
      USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
      
      NVIC_InitTypeDef nvicStructure;
      nvicStructure.NVIC_IRQChannel = UART4_IRQn;
      nvicStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&nvicStructure);
      
      USART_Cmd(UART4, ENABLE);  
      

      COMPORT1.PortNumber = PortNumber;
      COMPORT1.RxBuff = (unsigned char*)malloc(200);
      COMPORT1.RxBufferLen = 200;
      COMPORT1.RxBufferRead = 0;
      COMPORT1.RxBufferWrite = 0;

      COMPORT1.TxQueue = (unsigned char*)malloc(200);
      COMPORT1.TxQueueLen = 200;
      COMPORT1.TxQueueRead = 0;
      COMPORT1.TxQueueWrite = 0;

      
      free(COMPORT1.RxBuff);
      free(COMPORT1.TxQueue);
      COMPORT1.RxBuff = rxTestBuff;
      COMPORT1.TxQueue = txTestBuff;

      return &COMPORT1;
    break;}
  

  case 2:{

    TM_USB_VCP_Init();
    COMPORT2.PortNumber = PortNumber;
    COMPORT2.RxBuff = (unsigned char*)malloc(200);
    COMPORT2.RxBufferLen = 200;
    COMPORT2.RxBufferRead = 0;
    COMPORT2.RxBufferWrite = 0;

    COMPORT2.TxQueue = (unsigned char*)malloc(200);
    COMPORT2.TxQueueLen = 200;
    COMPORT2.TxQueueRead = 0;
    COMPORT2.TxQueueWrite = 0;

    COMPORT2.RxBuff = rxTestBuff2;
    //COMPORT2.TxQueue = txTestBuff;

    return &COMPORT2;
  break;}
  
  
	}
  return NULL;
}





int received = 0;
void UART4_IRQHandler( void )
{
  
  if((UART4->SR & 1<<5) != 0)
  {
    UINT32 data = USART_ReceiveData(UART4);
    USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    received++;
    PutByteToRxbuffer(&COMPORT1, data);
  }
}



void ClosePort(ComPortHandle* handle)
{
	if(handle == NULL)return;
	
	if(handle->TxQueue != NULL && handle->TxQueue != txTestBuff)
		free(handle->TxQueue);
		
	if(handle->RxBuff != NULL && handle->RxBuff != rxTestBuff)
		free(handle->RxBuff);
}//



void PutByte(ComPortHandle* handle, unsigned char b)
{
	if(handle == NULL)return;
	
	if (handle->TxQueueWrite >= handle->TxQueueLen)
		handle->TxQueueWrite = 0;

	handle->TxQueue[handle->TxQueueWrite++] = b;
	if (handle->TxQueueWrite == handle->TxQueueRead)
		handle->TxQueueRead++;
}//


BOOL GetByteForTx(ComPortHandle* handle, unsigned char* dest)
{
	if(handle == NULL)return FALSE;
	
	if (handle->TxQueueWrite == handle->TxQueueRead)return FALSE;

	if (handle->TxQueueRead >= handle->TxQueueLen)
	{
		handle->TxQueueRead = 0;
		return FALSE;
	}
	*dest = handle->TxQueue[handle->TxQueueRead++];
	return TRUE;
}//




BOOL GetByte(ComPortHandle* handle, unsigned char* dest)
{
	if(handle == NULL)return FALSE;
	
	if (handle->RxBufferWrite == handle->RxBufferRead)return FALSE;

	if (handle->RxBufferRead >= handle->RxBufferLen)
	{
		handle->RxBufferRead = 0;
		return FALSE;
	}

	*dest = handle->RxBuff[handle->RxBufferRead++];
	return TRUE;
}//


void PutByteToRxbuffer(ComPortHandle* handle, unsigned char b)
{

	if (handle->RxBufferWrite >= handle->RxBufferLen)
		handle->RxBufferWrite = 0;

	handle->RxBuff[handle->RxBufferWrite++] = b;
	
	if (handle->RxBufferWrite == handle->RxBufferRead)
		handle->RxBufferRead++;

}//


void ComPortCycle(void);


void ComPort_Task_Loop(void)
{
  unsigned char c;

  while(1)
  {

    if (TM_USB_VCP_GetStatus() == TM_USB_VCP_CONNECTED) {
      GPIO_WriteBit(GPIOD, LD6_Pin, Bit_SET);
    }
    else 
    {
      GPIO_WriteBit(GPIOD, LD6_Pin, Bit_RESET);
    }

    ComPortCycle();
    ClearUARTErrors();
  }
}//

void ClearUARTErrors(void)
{

}//




volatile int index = 0;


void ComPortCycle(void)
{
  if(COMPORT1.PortNumber != 0)
  {
    unsigned char b;
    while(GetByteForTx(&COMPORT1, (unsigned char *)&b))
    {
      while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
      USART_SendData(UART4, b);
      while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
    }
  }
  
  
  if(COMPORT2.PortNumber != 0)
  {
    unsigned char b;
    while(GetByteForTx(&COMPORT2, (unsigned char *)&b))
    {
      TM_USB_VCP_Putc(b);
    }
    
    if(TM_USB_VCP_Getc(&b) != TM_USB_VCP_DATA_EMPTY)
    {
      PutByteToRxbuffer(&COMPORT2, b);
    }
  }
  
  
}//


    
      
      
void FlushRx(ComPortHandle* handle)
{
	handle->RxBufferWrite = 0;
	handle->RxBufferRead = 0;
}//


void FlushTx(ComPortHandle* handle)
{
	handle->TxQueueWrite = 0;
	handle->TxQueueRead = 0;
}//
