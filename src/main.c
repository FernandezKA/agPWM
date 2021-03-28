/***************************
*UART_TX = PD5 
*PWM_1 = TIM1_CH3 = PC3 - positive
*PWM_2 = TIM1_CH4 = PC4 - negative
*LED_1 = TIM2_CH2 = GPIOD_3
*LED_2 = TIM2_CH3 = GPIOA_3
*ANSW_PIN = GPIOC_7
*/
#include "inc.h"
inline void SysConfig(void)
{
  CLK_Config();
  UART_Config();
  GPIO_Config();
  TIM1_Config();
  TIM2_Config();
  TIM4_Config();
  TIM2->CCER1|=TIM2_CCER1_CC2E;/*INDICATED OPERABILITY FIRMWARE*/
}
void main(void)
{
  SysConfig();
  asm("RIM"); /*enable global interrupt*/
  while (1)
  {

  }
  //asm("WFI");
}
