#if !defined ONEWIRE_H

#define ONEWIRE_H

#include "GenericTypeDefs.h"
#include "stdint.h"

typedef enum
{
  OWDirection_Output,
  OWDirection_Input,
}OWDirection;

extern void OnrWireTaskLoop(void);




void CheckKey(void);
void OW_write_bit (unsigned char write_data);
unsigned char OW_read_bit (void);
unsigned char OW_reset_pulse(void);
void OW_write_byte (unsigned char write_data);
unsigned char OW_read_byte (void);
unsigned char CRC8(unsigned char *buf,unsigned char n);

#define	HIGH	1
#define	LOW		0




#define DELAY_6Us	6
#define DELAY_9Us	9
#define DELAY_10Us	10
#define DELAY_55Us	55
#define DELAY_60Us	60
#define DELAY_64Us	64
#define DELAY_70Us	70
#define DELAY_205Us	205			// DELAY_410Us = DELAY_205Us + DELAY_205Us
#define DELAY_240Us	240			// DELAY_480Us = DELAY_240Us + DELAY_240Us // since the variable is declared as unsigned char




#endif