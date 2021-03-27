/*
* Author: Fernandez K. A. 
* Date: 2021-03-23
* This programm asking state of GPIO, configuring PWM at two channel
* Indicate with led state of the device
*/
#include "inc.h"
inline void UART_Config(void)
{
  /*BAUDRATE 115.108 kbps, BRR = 0x8B, MASTER CLOCKING = 16 MHz*/
  CLK->PCKENR1 |= CLK_PCKENR1_UART1; /*ENABLE UART CLOCKING*/
  UART1->BRR1 = 0x08;
  UART1->BRR2 = 0x0B;
  UART1->CR2 |= UART1_CR2_TEN; /*ENABLE TRANSMITTER*/
}
inline void UART_Send(const uint32_t data)
{
  while (UART1->SR & UART1_SR_TXE == 0x00U)
  {
    asm("nop");
  }; /*WAIT TRANSMIT DATA*/
  UART1->DR = data << 24;
}
inline void CLK_Config(void)
{
  CLK->CKDIVR = 0x00U; /*WITHOUT PRESCALE*/
}
inline void GPIO_Config(void)
{
  GPIOD->DDR |= (1U << 2); /*SET GPIOD2 TO OUPUT AT LED*/
  GPIOD->CR1 |= (1U << 2);
  /*config pin for sampling*/
  GPIOC->CR1|=(1U<<7);/*SET INPUT WITH PULL-UP*/
  /*CONFIGURE LED_1*/
  GPIOC->DDR|=(1U<<6);/*OUTPUT*/
  GPIOC->CR1|=(1U<<6);
  }
inline void TIM2_Config(void)
{
  CLK->PCKENR2 |= CLK_PCKENR1_TIM1; /*ENABLE TIM1*/
  TIM1->PSCRH = 1600 >> 8;
  TIM1->PSCRL = 1600 & 0xFF; /*PRESCALER 1600*/
  TIM1->ARRH = 1000 >> 8;    /*16 MHz/160/10000 = 10 Hz*/
  TIM1->ARRL = 1000 & 0xFF;
  //TIM1->CR1|= TIM1_CR1_URS;/*ENABLE INTERRUPT FOR OVERFLOW*/
  TIM1->EGR |= TIM1_EGR_UG; /*CALL UPDATE EVENT*/
  //TIM1->IER|=TIM1_IER_UIE;/*ALLOW INTERRUPT*/
  TIM1->CR1 |= TIM1_CR1_CEN; /*RUN TIMER*/
}
inline void TIM4_Config(void)
{
  CLK->PCKENR1 |= CLK_PCKENR1_TIM4;
  TIM4->IER |= TIM4_IER_UIE; /*ENABLE INTERRUPT UPDATE*/
  TIM4->IER |= TIM4_IER_RESET_VALUE;
  TIM4->PSCR = (0x00U); /*SET PRESCALER = 2^0 = 1*/
  TIM4->ARR = 0x9FU;
  ITC->ISPR6 &= 0;
  ITC->ISPR6 |= 0x03U;
  TIM4->SR1 = ~TIM4_SR1_UIF; /*clear uif bit at SREG for correct working*/
  TIM4->CR1 |= TIM4_CR1_CEN;
}
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
  if (TIM4->SR1 & TIM4_SR1_UIF == TIM4_SR1_UIF)
  {
    TIM4->SR1 = (uint8_t) ~(TIM4_SR1_UIF);
    GPIOD->ODR ^= (1U << 2);
    if (index < 100000U)
    {
      if(different){
        if(index%100 == 0x00U ){
          GPIOC->ODR^=(1U<<6);/*INVERT STATE OF LED1*/
        }
      }
      ++index;
      volatile unsigned char temp = GPIOC->IDR;
      //temp = temp&0x80;
      if ((temp & 0x80U) == 0x80U)
      {
        ++ones_temp;
      }
    }
    else if(index == 100000U)
    {
      index = 0x00U;
      if(ones_temp != last_ones){
        different = TRUE;
      }
      else{
        different = FALSE;
      }
      last_ones = ones_temp;
      ones = ones_temp;
      ones_temp = 0x00U;
      ++count_send;
      UART1->DR = (ones >> 24)&0xFFU;
      UART1->CR2 |= UART1_CR2_TCIEN; /*ENABLE TRANSMIT COMPLETE INTERRUPT*/
    }
  }
}
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{
  switch (count_send)
  {
  case 1:
    UART1->DR = (ones >> 16) & 0xFFU;
    ++count_send;
    UART1->SR &= ~UART1_SR_TXE;
    return;
    break;
  case 2:
    UART1->DR = (ones >>8) & 0xFFU;
    ++count_send;
    UART1->SR &= ~UART1_SR_TXE;
    return;
    break;
  case 3:
    UART1->DR = (ones & 0xFF);
    ones = 0x00U;
    count_send = 0U;
    UART1->SR &= ~UART1_SR_TXE;
    return;
    break;
  default:
    UART1->SR &= ~UART1_SR_TXE;
    UART1->CR2 &= ~UART1_CR2_TCIEN; /*ENABLE TRANSMIT COMPLETE INTERRUPT*/
    return;
    break;
  }
}