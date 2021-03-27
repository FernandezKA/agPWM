/***************************
*UART_TX = PD5 
*PWM_1 = TIM1_CH3 = PC3
*PWM_2 = TIM1_CH4 = PC4
*LED_1 = GPIOD_2
*LED_2 = GPIOD_3
*ANSW_PIN = GPIOC_7
*/
#include "inc.h"
volatile uint32_t index = 0x00U;
inline void SysConfig(void){
  CLK_Config();
  UART_Config();
  GPIO_Config();
  TIM1_Config();
  TIM4_Config();
}
void main(void)
{
  SysConfig();
  asm("RIM");/*enable global interrupt*/
  while(1){
  }
  //asm("WFI");
}
