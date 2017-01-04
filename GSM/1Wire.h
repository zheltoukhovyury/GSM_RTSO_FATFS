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

void SetDirection_Tochka8(OWDirection dir);
void drive_OW_low_Tochka8 (void);
void drive_OW_high_Tochka8 (void);


void SetDirection_Tochka4(OWDirection dir);
void drive_OW_low_Tochka4 (void);
void drive_OW_high_Tochka4 (void);


void SetDirection_Orbita(OWDirection dir);
void drive_OW_low_Orbita (void);
void drive_OW_high_Orbita (void);

void SetDirection_STM32(OWDirection dir);
void drive_OW_low_STM32 (void);
void drive_OW_high_STM32 (void);
unsigned char read_OW_STM32 (void);

unsigned char OW_wait_HIGH_PIC(uint32_t  time);
void wait_PIC(unsigned int time);

unsigned char OW_wait_HIGH_STM32(uint32_t  time);
void wait_STM32(unsigned int time);



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