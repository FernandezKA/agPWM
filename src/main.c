#include "inc.h"
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
}
