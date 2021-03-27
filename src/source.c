/*
* Author: Fernandez K. A. 
* Date: 2021-03-23
* This programm asking state of GPIO, configuring PWM at two channel
* Indicate with led state of the device
*/
#include "inc.h"
 void UART_Config(void)
{
  /*BAUDRATE 115.108 kbps, BRR = 0x8B, MASTER CLOCKING = 16 MHz*/
  CLK->PCKENR1 |= CLK_PCKENR1_UART1; /*ENABLE UART CLOCKING*/
  UART1->BRR1 = 0x68;
  UART1->BRR2 = 0x03;
  UART1->CR2 |= UART1_CR2_TEN; /*ENABLE TRANSMITTER*/
}

 void UART_Send(const uint32_t data)
{
  while (UART1->SR & UART1_SR_TXE == 0x00U)
  {
    asm("nop");
  }; /*WAIT TRANSMIT DATA*/
  UART1->DR = data << 24;
}

 void CLK_Config(void)
{
  CLK->CKDIVR = 0x00U; /*WITHOUT PRESCALE*/
}

 void GPIO_Config(void)
{
  GPIOD->DDR |= (1U << 2); /*SET GPIOD2 TO OUPUT AT LED*/
  GPIOD->CR1 |= (1U << 2);
  /*config pin for sampling*/
  GPIOC->CR1|=(1U<<7);/*SET INPUT WITH PULL-UP*/
  /*CONFIGURE LED_1*/
  GPIOC->DDR|=(1U<<6);/*OUTPUT*/
  GPIOC->CR1|=(1U<<6);
  }

 void TIM1_Config(void)
{
  CLK->PCKENR2 |= CLK_PCKENR1_TIM1; /*ENABLE TIM1 clocking*/
  TIM1->CR1|=TIM1_CR1_ARPE;
  TIM1->PSCRH = 15 >> 8;
  TIM1->PSCRL = 15 & 0xFF; /*PRESCALER 1600*/
  TIM1->ARRH = 10000U >> 8;    /*16 MHz/1600/100 = 100 Hz*/
  TIM1->ARRL = 10000U & 0xFF;

  TIM1->BKR|=TIM1_BKR_MOE;
  TIM1->CCMR3|=(1U<<6|1U<<5|1U<<3);/*MODE 1 AND ENABLE OUTPUT COMPARE PRELOAD ENABLE*/
  TIM1->CCMR4|=(1U<<6|1U<<5|1U<<3);
  TIM1->CCER2|=TIM1_CCER2_CC3E;/*Capture/compare 3 output enable*/
  TIM1->CCER2|=TIM1_CCER2_CC4E;
  TIM1->CCER2|=TIM1_CCER2_CC4P;/*INVERTING POLARITY FOR CH4*/
  TIM1->CCR3H = 500U>>8;/*default value PWM with 50% duty*/
  TIM1->CCR3L = 500&0xFFU;
  TIM1->CCR4H = 500U>>8;/*default value PWM with 50% duty*/
  TIM1->CCR4L = 500U&0xFFU;
  TIM1->CR1 |= TIM1_CR1_CEN; /*RUN TIMER*/
}


 void TIM4_Config(void)
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

/*  */ void PWM(const uint16_t value){
  TIM1->CCR3H = TIM1->CCR4H = (uint16_t) value>>8;
  TIM1->CCR3L = TIM1->CCR4L = (uint16_t) value&0xFFU;
  return;
}

/*************************************Block of Interrupt***********************/

INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
  if (TIM4->SR1 & TIM4_SR1_UIF == TIM4_SR1_UIF)
  {
    TIM4->SR1 = (uint8_t) ~(TIM4_SR1_UIF);

    GPIOD->ODR ^= (1U << 2);/*string for testing frequency sampling*/
    if (index < 100000U)
    {
/*       if(different){
        if(index%1000 == 0x00U ){
          GPIOC->ODR^=(1U<<6);//LED_1 INDICATE NOT EQUALS VECTOR OF SAMPLES
        }
      } *///This block of code using many times
      ++index;
      volatile unsigned char temp = GPIOC->IDR;
      if ((temp & 0x80U) == 0x80U)
      {
        ++ones_temp;
      }
    }
    else if(index == 100000U)
    {
      index = 0x00U;
       if(ones_temp != last_ones){
        different = true;
        pwm_index = (uint16_t) (ones_temp/10U);
        PWM(pwm_index);
      }
      else{
        different = false;
      } 
      last_ones = ones_temp;
      ones = ones_temp;
      ones_temp = 0x00U;
      ++count_send;
      UART_Send((ones>>16)&0xFF);
      //UART_Send((ones >> 8) & 0xFFU);
      //UART_Send((ones) & 0xFFU);
      //unsigned char temp = ones >>16;
      //UART1->DR = (temp)&0xFFU;
      UART1->CR2 |= UART1_CR2_TCIEN; /*ENABLE TRANSMIT COMPLETE INTERRUPT*/
    }
  }
}
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{
  asm("sim");/*disable interrupt*/
  switch (count_send)
  {
  case 1:
    UART1->DR = (ones >> 8) & 0xFFU;
    ++count_send;
    UART1->SR &= ~UART1_SR_TXE;
    asm("rim");
    return;
    break;
  case 2:
    UART1->DR = (ones) & 0xFFU;
    ++count_send;
    UART1->SR &= ~UART1_SR_TXE;
    ones = 0x00U;
    count_send = 0U;
    UART1->SR &= ~UART1_SR_TXE;
    UART1->CR2 &= ~UART1_CR2_TCIEN; /*ENABLE TRANSMIT COMPLETE INTERRUPT*/
    asm("rim");
    return;
    break;
  default:
    UART1->SR &= ~UART1_SR_TXE;
    UART1->CR2 &= ~UART1_CR2_TCIEN; /*ENABLE TRANSMIT COMPLETE INTERRUPT*/
    asm("rim");
    return;
    break;
  }
}