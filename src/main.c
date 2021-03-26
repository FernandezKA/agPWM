#include "inc.h"
volatile uint32_t index = 0x00U;
inline void SysConfig(void){
  CLK_Config();
  UART_Config();
  GPIO_Config();
  TIM2_Config();
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
