#include "ComPort.h"
#include <stdlib.h>
#include "ComPort.h"
#include "FreeRTOS.h"
#include "tm_stm32f4_usb_vcp.h"

#include "tick.h"


ComPortHandle* Port = NULL;



typedef enum{
  ProgrammingMaschine_State_NotInited,
  ProgrammingMaschine_State_Inited,


}TProgrammingMaschine_State;


TProgrammingMaschine_State ProgrammingMaschine_State = ProgrammingMaschine_State_NotInited;

void ProgrammingMaschine(void)
{
  switch(ProgrammingMaschine_State)
  {
    case ProgrammingMaschine_State_NotInited:
      Port = OpenPort(2, 115200);
      ProgrammingMaschine_State = ProgrammingMaschine_State_Inited;
      break;
    
  case ProgrammingMaschine_State_Inited:
    break;


  }
}