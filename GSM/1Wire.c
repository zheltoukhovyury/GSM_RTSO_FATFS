#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "hardwareProfile.h"
#include "GenericTypeDefs.h"
#include "1Wire.h"
#include "gpio.h"
#include "tick.h"
#include "hardwareProfile.h"




//указатели на функции.чтениязаписи ног 
void (*SetDirection)(OWDirection dir);
void (*drive_OW_low) (void);
void (*drive_OW_high) (void);
unsigned char (*read_OW) (void);
unsigned char (*OW_wait_HIGH) (uint32_t time);
void (*wait) (unsigned int time);



volatile UINT32 checkTick = 0;


void OnrWireTaskLoop(void)
{
  if(HardwareType == HrdwareType_STM32)
  {
          SetDirection = &SetDirection_STM32;
          drive_OW_low = &drive_OW_low_STM32;
          drive_OW_high = &drive_OW_high_STM32;
          read_OW = &read_OW_STM32;
          OW_wait_HIGH = OW_wait_HIGH_STM32;
          wait = wait_STM32;
  }
          
  while(1)
  {
    if(TickGet() - checkTick > TICK_SECOND/10)
    {
      CheckKey();
      checkTick = TickGet();
    }	
  }
}


	
void CheckKey(void)
{
	if(OW_reset_pulse())
	{
		taskENTER_CRITICAL();
		
		OW_write_byte (0x33);
		wait(10);
		unsigned char i;
		unsigned char data[10];
		
		for(i = 0; i<10; i++)
			data[i] = OW_read_byte();	
			
		taskEXIT_CRITICAL();
	
		if(CRC8(data,8) == 0 && data[0]!= 0x00 /*&& data[0]!= 0xFF*/ && data[0]!= 0x28)
		{
			//это ключ !
			//обнаружение ключа может произойти в прерывании. что бы не наршуть очередность обнаружение-обработки ключенй.. вводится этот промежуточный флаг
			return;
		}
	}
}//--------------------------------------------------------------------



/**********************************************************************
* Function:        void drive_OW_low (void)
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   Configure the OW_PIN as Output and drive the OW_PIN LOW.	
***********************************************************************/

unsigned char Epsilon=0;


/**********************************************************************
* Function:        unsigned char OW_reset_pulse(void)
* PreCondition:    None
* Input:		   None	
* Output:		   Return the Presense Pulse from the slave.	
* Overview:		   Initialization sequence start with reset pulse.
*				   This code generates reset sequence as per the protocol
***********************************************************************/
unsigned char OW_reset_pulse(void)
{
	unsigned char presence_detect = 1;

  	drive_OW_low(); 				// Drive the bus low

 	wait(DELAY_240Us);	  			// delay 480 microsecond (us)
	wait(DELAY_240Us);		
	
	taskENTER_CRITICAL();
	
 	drive_OW_high ();  				// Release the bus

	volatile unsigned char t;

	t = OW_wait_HIGH(75);

	Epsilon=t;
	if(Epsilon > 75)
		Epsilon = 75;
	wait(75 - Epsilon);				// delay 70 microsecond (us)

	presence_detect = read_OW();	//Sample for presence pulse from slave
	
	taskEXIT_CRITICAL();
	
 	wait(DELAY_205Us);	  			// delay 410 microsecond (us)
	wait(DELAY_205Us);		

	drive_OW_high ();		    	// Release the bus

	return !presence_detect;
}	

/**********************************************************************
* Function:        void OW_write_bit (unsigned char write_data)
* PreCondition:    None
* Input:		   Write a bit to 1-wire slave device.
* Output:		   None
* Overview:		   This function used to transmit a single bit to slave device.
*				   
***********************************************************************/
void OW_write_bit (unsigned char write_bit)
{
	if (write_bit)
	{
		//writing a bit '1'
		drive_OW_low(); 				// Drive the bus low
		wait(DELAY_6Us + Epsilon);				// delay 6 microsecond (us)
		drive_OW_high ();  				// Release the bus
		wait(DELAY_64Us + Epsilon);				// delay 64 microsecond (us)
	}
	else
	{
		//writing a bit '0'
		drive_OW_low(); 				// Drive the bus low
		wait(DELAY_60Us);				// delay 60 microsecond (us)   
		drive_OW_high ();  				// Release the bus
		wait(DELAY_10Us + Epsilon);				// delay 10 microsecond for recovery (us)
	}
}	


/**********************************************************************
* Function:        unsigned char OW_read_bit (void)
* PreCondition:    None
* Input:		   None
* Output:		   Return the status of the OW PIN
* Overview:		   This function used to read a single bit from the slave device.
*				   
***********************************************************************/

unsigned char OW_read_bit (void)
{
	unsigned char read_data; 
	//reading a bit 
	drive_OW_low(); 						// Drive the bus low
	wait(DELAY_6Us + Epsilon);						// delay 6 microsecond (us)
	drive_OW_high ();  						// Release the bus
	wait(DELAY_9Us + Epsilon);				// delay 9 microsecond (us)

	read_data = read_OW();					//Read the status of OW_PIN

	wait(DELAY_55Us);						// delay 55 microsecond (us)	
	return read_data;
}

/**********************************************************************
* Function:        void OW_write_byte (unsigned char write_data)
* PreCondition:    None
* Input:		   Send byte to 1-wire slave device
* Output:		   None
* Overview:		   This function used to transmit a complete byte to slave device.
*				   
***********************************************************************/
void OW_write_byte (unsigned char write_data)
{
	unsigned char loop;
	
	for (loop = 0; loop < 8; loop++)
	{
		OW_write_bit(write_data & 0x01); 	//Sending LS-bit first
		write_data >>= 1;					// shift the data byte for the next bit to send
	}	
}	

/**********************************************************************
* Function:        unsigned char OW_read_byte (void)
* PreCondition:    None
* Input:		   None
* Output:		   Return the read byte from slave device
* Overview:		   This function used to read a complete byte from the slave device.
*				   
***********************************************************************/

unsigned char OW_read_byte (void)
{
	unsigned char loop, result=0;
	
	for (loop = 0; loop < 8; loop++)
	{
		
		result >>= 1; 				// shift the result to get it ready for the next bit to receive
		if (OW_read_bit())
		result |= 0x80;				// if result is one, then set MS-bit
	}
	return result;					
}	




static unsigned char dscrc_table[] = {
0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};
//--------------------------------------------------------------------------
// Calculate the CRC8 of the byte value provided with the current
// global 'crc8' value.
// Returns current global crc8 value


unsigned char CRC8(unsigned char *buf,unsigned char n)
{
    unsigned char  crc = 0x00, c, i, j;
    for (i = 0; i < n; i++)
    {
        c = buf[i];
        for(j = 0; j < 8; j++)
        {
                if ((c ^ crc) & 1) crc = ((crc ^ 0x18) >> 1) | 0x80;
                else crc >>= 1;
                c >>= 1;
        }
    }
    return crc;
}
//------------------------------------------------------------------------------------
