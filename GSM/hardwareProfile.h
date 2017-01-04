#if !defined HARDWAREPROFILE_H

#define HARDWAREPROFILE_H
typedef enum 
{
	HrdwareType_Undefined = 0,
	HrdwareType_TOCHKA_8,
	HrdwareType_TOCHKA_4,
	HrdwareType_OrbitaOhrana,
        HrdwareType_STM32,
}THardwareType;

extern THardwareType HardwareType;


#endif