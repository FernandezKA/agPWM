/*
*UART_TX = PD5 
*LED_1 = TIM2_CH2 = GPIOD_3
*ANSW_PIN = GPIOC_7
*PA1 - clocking(only for HSE, 8 MHz
*/
#include "inc.h"
inline void SysConfig(void)
{
  //CLK_HSI_Config();
  CLK_HSE_Config();
  UART_Config();
  GPIO_Config();
  TIM1_Config();/*using for edit freq of pwm tim_2*/
  TIM2_Config();/*using for indicate activity*/
  TIM4_Config();/*using for definition frequency of sampling*/
  TIM2->CCER1|=TIM2_CCER1_CC2E;
}
void main(void)
{
  SysConfig();
  asm("RIM"); /*enable global interrupt*/
  asm("WFI"); 
}
