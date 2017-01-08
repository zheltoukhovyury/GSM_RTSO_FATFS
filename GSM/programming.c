#include "ComPort.h"
#include <stdlib.h>
#include "ComPort.h"
#include "FreeRTOS.h"
#include "tm_stm32f4_usb_vcp.h"
#include "programming.h"
#include "tick.h"


ComPortHandle* Port = NULL;



typedef enum{
  ProgrammingMaschine_State_NotInited,
  ProgrammingMaschine_State_Inited,
  ProgrammingMaschine_State_Idle,


}TProgrammingMaschine_State;


TProgrammingMaschine_State ProgrammingMaschine_State = ProgrammingMaschine_State_NotInited;


unsigned char ProgTestBuff[150];


void ProgrammingMaschine(void)
{
  static unsigned char* RcvBuffer = NULL;
  static int RcvP = 0;
  static unsigned char opcode;
  
  switch(ProgrammingMaschine_State)
  {
    case ProgrammingMaschine_State_NotInited:
      Port = OpenPort(2, 115200);
      //RcvBuffer = malloc(150);
      RcvBuffer = ProgTestBuff;
      ProgrammingMaschine_State = ProgrammingMaschine_State_Inited;
      break;
  case ProgrammingMaschine_State_Inited:
    while(GetByte(Port, &RcvBuffer[RcvP]))
    {
      RcvP++;
      if(RcvBuffer[0] != 0x55)
      {
        RcvP = 0;
        continue;
      }
      if(RcvP > 2 && RcvP >= (RcvBuffer[1] + 4))
      {
        RcvP = 0;
        opcode = RcvBuffer[2];
        if(opcode == 1)
        {
          unsigned char packet[10];
          int len = CreatePacket(1, 0, packet, NULL);
          int i;
          for(i = 0; i < len; i++)
            PutByte(Port, packet[i]);
          ProgrammingMaschine_State = ProgrammingMaschine_State_Idle;
          return;
        }
      }
    }
    break;
  case ProgrammingMaschine_State_Idle:
    while(GetByte(Port, &RcvBuffer[RcvP]))
    {
      RcvP++;
      if(RcvBuffer[0] != 0x55)
      {
        RcvP = 0;
        continue;
      }
      if(RcvP > 2 && RcvP >= (RcvBuffer[1] + 4))
      {
        RcvP = 0;
        opcode = RcvBuffer[2];
        switch(opcode)
        {
        default:
        case 1:
        case 2:{
          unsigned char packet[10];
          int len = CreatePacket(1, 0, NULL, packet);
          int i;
          for(i = 0; i < len; i++)
            PutByte(Port, packet[i]);
          break;}
        case 3:{
          unsigned char packet[10];
          int len = CreatePacket(3, 0, NULL, packet);
          int i;
          for(i = 0; i < len; i++)
            PutByte(Port, packet[i]);
          break;}
        }
      }
    }
    break;
  }
}


int CreatePacket(unsigned char Opcode, int dataLen, unsigned char* data, unsigned char* buffer)
{
  buffer[0] = 0x55;
  buffer[1] = dataLen;
  buffer[2] = Opcode;
  memcpy(&buffer[3], data, dataLen);
  buffer[3 + dataLen] = 0xCC;
  return 4 + dataLen;
}