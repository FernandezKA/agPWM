/*
* Author: Fernandez K. A. 
* Date: 2021-03-23
* This programm asking state of GPIO, configuring PWM at two channel
* Indicate with led state of the device
*/ 
#include"inc.h"
inline void UART_Config(void){
    /*BAUDRATE 115.108 kbps, BRR = 0x8B, MASTER CLOCKING = 16 MHz*/
  CLK->PCKENR1|=CLK_PCKENR1_UART1;/*ENABLE UART CLOCKING*/
    UART1->BRR1=0x08;
    UART1->BRR2=0x0B;
    UART1->CR2|=UART1_CR2_TEN;/*ENABLE TRANSMITTER*/
}
inline void UART_Send(const uint8_t data){
    while(UART1->SR&UART1_SR_TXE == 0x00U){
      asm("nop");
    };/*WAIT TRANSMIT DATA*/
    UART1->DR = data;
}
inline void CLK_Config(void){
    CLK->CKDIVR=0x00U;/*WITHOUT PRESCALE*/ 
}
inline void GPIO_Config(void){
    GPIOD->DDR|=(1U<<2);/*SET GPIOD2 TO OUPUT AT LED*/
    GPIOD->CR1|=(1U<<2);
}
inline void TIM1_Config(void){
    CLK->PCKENR2|=CLK_PCKENR1_TIM1;/*ENABLE TIM1*/
    TIM1->PSCRH =1600>>8;
    TIM1->PSCRL =1600&0xFF;/*PRESCALER 1600*/
    TIM1->ARRH = 1000>>8;/*16 MHz/160/10000 = 10 Hz*/
    TIM1->ARRL = 1000&0xFF;
    //TIM1->CR1|= TIM1_CR1_URS;/*ENABLE INTERRUPT FOR OVERFLOW*/
    TIM1->EGR|=TIM1_EGR_UG;/*CALL UPDATE EVENT*/
    //TIM1->IER|=TIM1_IER_UIE;/*ALLOW INTERRUPT*/
    TIM1->CR1|=TIM1_CR1_CEN;/*RUN TIMER*/
}
inline uint8_t ANS_GPIO(void){
  return 0;
}
inline void TIM4_Config(void){
    CLK->PCKENR1|=CLK_PCKENR1_TIM4;
    TIM4->IER|=TIM4_IER_UIE;/*ENABLE INTERRUPT UPDATE*/
    TIM4->IER|=TIM4_IER_RESET_VALUE;
    TIM4->PSCR = (0x00U);/*SET PRESCALER = 2^0 = 1*/
    TIM4->ARR = 0x9FU;
    TIM4->SR1 = (uint8_t)(~TIM4_SR1_UIF);
    TIM4->CR1|=TIM4_CR1_CEN;
}