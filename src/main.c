#include "inc.h"
inline void SysConfig(void){
  CLK_Config();
  UART_Config();
}
void main(void)
{
  SysConfig();
	while (1){
          for(int i = 0; i<100; ++i){
                UART_Send(i, i+1);
          }
  }
}
