

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_gpio.h"
#include "gpio.h"



/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
void Init_GPIO(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;    
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_POWERKEY_Pin | GSM_POWERPIN_Pin | GSM_SIM_Switcher_Pin | LD6_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
}





void GSM_POWERKEY_Set(uint8_t newState)
{
  newState == 1 ? GPIO_WriteBit(GPIOD, GSM_POWERKEY_Pin, Bit_SET) : GPIO_WriteBit(GPIOD, GSM_POWERKEY_Pin, Bit_RESET);
}

void GSM_POWERPIN_Set(uint8_t newState)
{
  newState == 1 ? GPIO_WriteBit(GPIOD, GSM_POWERPIN_Pin, Bit_SET) : GPIO_WriteBit(GPIOD, GSM_POWERPIN_Pin, Bit_RESET);
}

void GSM_SIM_Switcher_Set(uint8_t newState)
{
    newState == 1 ? GPIO_WriteBit(GPIOD, GSM_SIM_Switcher_Pin, Bit_SET) : GPIO_WriteBit(GPIOD, GSM_SIM_Switcher_Pin, Bit_RESET);
}

uint8_t GSM_SIM_Switcher_Get(void)
{
  return 0;
}


void SetDirection_STM32(OWDirection dir)
{
				
}

void drive_OW_low_STM32 (void)
{

}

void drive_OW_high_STM32 (void)
{

}


unsigned char read_OW_STM32 (void)
{
  return 1;
}

unsigned char OW_wait_HIGH_STM32(uint32_t time)
{
  /*
  HAL_TIM_StateTypeDef timState;
  HAL_TIM_Base_Start(&htim2);
  timState = HAL_TIM_Base_GetState(&htim2);
  */

}



void wait_STM32(unsigned int time)
{
  /*
  volatile HAL_TIM_StateTypeDef timState;
  HAL_TIM_Base_Start(&htim2);
  timState = HAL_TIM_Base_GetState(&htim2);
  timState = timState;
  */
  

}//
