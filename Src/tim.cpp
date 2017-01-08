#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"


uint32_t sysTick = 0;

void InitSysTicks(void)
{
  TIM_TimeBaseInitTypeDef init;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  init.TIM_ClockDivision = 0;
  init.TIM_CounterMode = TIM_CounterMode_Up;
  init.TIM_Period = RCC_Clocks.PCLK2_Frequency/1000000;
  init.TIM_Prescaler = 2000;
  init.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &init);
  TIM_Cmd(TIM1, ENABLE);
  
  NVIC_InitTypeDef nvicStructure;
  nvicStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  nvicStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicStructure);
  
  
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
  
  
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
}


void TIM1_UP_TIM10_IRQHandler(){
  
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
    
    sysTick++;
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
  }
}


uint32_t TickGet(void)
{
  return sysTick;
}