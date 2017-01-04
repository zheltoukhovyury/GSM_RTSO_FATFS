
#ifndef __TICK_H
#define __TICK_H

#include "GenericTypeDefs.h"



#define TICKS_PER_SECOND		(1000ull)	// Internal core clock drives timer with 1:256 prescaler



#define TICK_SECOND				((UINT32)TICKS_PER_SECOND)

#define TICK_MINUTE				((UINT32)TICKS_PER_SECOND*60ull)

#define TICK_HOUR				((UINT32)TICKS_PER_SECOND*3600ull)


void TickInit(void);
UINT32 TickGet(void);
UINT32 TickConvertToMilliseconds(UINT32 dwTickValue);


#endif
