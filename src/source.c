/*
* Author: Fernandez K. A. 
* Date: 2021-03-23
* This programm asking state of GPIO, configuring PWM at two channel
* Indicate with led state of the device
*/ 
#include"inc.h"
inline void UART_Config(void){
    /*BAUDRATE 115.108 kbps, BRR = 0x8B, MASTER CLOCKING = 16 MHz*/
    UART1->BRR1=0x08;
    UART1->BRR2=0x0B;
    UART1->CR2|=UART1_CR2_TEN;/*ENABLE TRANSMITTER*/
}
inline void UART_Send(const uint8_t data_first, const uint8_t data_second){
    while(UART1->SR&UART1_SR_TXE == UART1_SR_TXE);/*WAIT TRANSMIT DATA*/
    UART1->DR = data_first;
    while(UART1->SR&UART1_SR_TXE == UART1_SR_TXE);/*WAIT TRANSMIT DATA*/
    UART1->DR = data_second;
    while(UART1->SR&UART1_SR_TXE == UART1_SR_TXE);/*WAIT TRANSMIT DATA*/
    return;
}
inline void CLK_Config(void){
    CLK->CKDIVR = 0x00U;/*WITHOUT PRESCALE*/
    CLK->PCKENR1|=CLK_PCKENR1_TIM1;
    CLK->PCKENR1|=CLK_PCKENR1_UART1;/*ENABLE TIM1&UART CLOCKING*/
}
inline void GPIO_Config(void){
  
}