/*
* Author: Fernandez K. A. 
* Date: 2021-03-23
* This programm asking state of GPIO, configuring PWM at two channel
* Indicate with led state of the device
*/ 
#include"inc.h"
void UART_Config(void){
    CLK->PCKENR1|=CLK_PCKENR1_UART1;/*ENABLE CLOCKING USART*/
    /*BAUDRATE 115200, BRR = 0x8B, MASTER CLOCKING = 16 MHz*/
    UART1->BRR1=0x08U;
    UART1->BRR2=0x0BU;
    UART1->CR1|=UART1_CR2_TEN;/*ENABLE TRANSMITTER*/
}
static void UART_Send(const uint16_t data){
}