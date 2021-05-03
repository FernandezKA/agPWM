/*
* Author: Fernandez K. A. 
* Date: 2021-03-23
* This programm asking state of GPIO, configuring PWM at two channel
* Indicate with led state of the device
*/
#include "inc.h"
void UART_Config(void)
{
  /*BAUDRATE 9600 kbps, MASTER CLOCKING = 16 MHz*/
  CLK->PCKENR1 |= CLK_PCKENR1_UART1; /*ENABLE UART CLOCKING*/
  /*old value is 0X68, 0X03(1, 2)*/
  UART1->BRR1 = 0x68;
  UART1->BRR2 = 0x02;
  UART1->CR2 |= UART1_CR2_TEN; /*ENABLE TRANSMITTER*/
}

void UART_Send(const unsigned char data)
{
  while (UART1->SR & UART1_SR_TXE == 0x00U)
  {
    asm("nop");
  }; //WAIT TRANSMIT DATA
  UART1->DR = data;
}
void CLK_Config(void)
{
  CLK->CKDIVR = 0x00U; /*WITHOUT PRESCALE*/
}

void GPIO_Config(void)
{
  /*config pin for sampling*/
  GPIOC->CR1 |= (1U << 7); /*SET INPUT WITH PULL-UP*/
  /*Configure LED*/
  GPIOA->DDR|=(1U<<3);
  GPIOA->CR1|=(1U<<3);
  
  GPIOD->DDR|=(1U<<3);
  GPIOD->CR1|=(1U<<3);
}

void TIM1_Config(void)
{
  CLK->PCKENR2 |= CLK_PCKENR1_TIM1; /*ENABLE TIM1 clocking*/
  TIM1->CR1 |= TIM1_CR1_ARPE;
  TIM1->PSCRH = 15 >> 8;
  TIM1->PSCRL = 15 & 0xFF;  /*PRESCALER 16*/
  TIM1->ARRH = 9999U >> 8; /*16 MHz/16/10000 = 100 Hz*/
  TIM1->ARRL = 9999U & 0xFF;
  TIM1->BKR |= TIM1_BKR_MOE;
  TIM1->CCMR3 |= (1U << 6 | 1U << 5 | 1U << 3); /*MODE 1 AND ENABLE OUTPUT COMPARE PRELOAD ENABLE*/
  TIM1->CCMR4 |= (1U << 6 | 1U << 5 | 1U << 3);
  TIM1->CCER2 |= TIM1_CCER2_CC3E; /*Capture/compare 3 output enable*/
  TIM1->CCER2 |= TIM1_CCER2_CC4E;
  TIM1->CCER2 |= TIM1_CCER2_CC4P; /*INVERTING POLARITY FOR CH4*/
  TIM1->CCR3H = 5000U >> 8;        /*default value PWM with 50% duty*/
  TIM1->CCR3L = 5000U & 0xFFU;
  TIM1->CCR4H = 5000U >> 8; /*default value PWM with 50% duty*/
  TIM1->CCR4L = 5000U & 0xFFU;
  TIM1->CR1 |= TIM1_CR1_CEN; /*RUN TIMER*/
}
void TIM2_Config(void){
  /*THIS TIMER WE USING FOR GENERATE BLINKING LED, LED1 - CH2, LED2 - CH3
  *FOR GENERATE USING LOW FREQUENCY PWM WITH 50% DUTY*/
  CLK->PCKENR1|=CLK_PCKENR1_TIM2;/*ENABLE CLOCKING*/
  TIM2->CR1|=TIM2_CR1_ARPE;
  TIM2->PSCR|=(1U<<3|1U<<2|1U<<1|1U<<0);/*SET PRESCALER 2^15, Fps = 488 Hz*/
  TIM2->ARRH = 488U>>8;
  TIM2->ARRL = 488U&0xFFU;
  TIM2->CCR2H = 244U>>8;
  TIM2->CCR2L = 244U&0xFFU;
  TIM2->CCR3H = 244U>>8;
  TIM2->CCR3L = 244U&0xFFU;
  //TIM2->CCER1|=TIM2_CCER1_CC2E;/*ENABLE CAPTURE/COMPARE FOR CHANNEL 2 AND 3*/
  //TIM2->CCER2|=TIM2_CCER2_CC3E;
  //TIM2->CCER2|=TIM2_CCER2_CC3P;/*UNPHASED BLINKING*/
  TIM2->CCMR2|=(1U<<6|1U<<5|1U<<3);/*MODE 1 WITH OUTPUT COMPARE PRELOAD*/
  TIM2->CCMR3|= (1U<<6|1U<<5|1U<<3);
  TIM2->CR1|=TIM2_CR1_CEN;/*RUN TIM2*/
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
/*************************************Block of Interrupt***********************/
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
  if (TIM4->SR1 & TIM4_SR1_UIF == TIM4_SR1_UIF)
  {
    TIM4->SR1 = (uint8_t) ~(TIM4_SR1_UIF);
    //GPIOD->ODR ^= (1U << 2); /*string for testing frequency sampling*/
    if (index < 100000U)
    {
      ++index;
      volatile unsigned char temp = GPIOC->IDR;
      if ((temp & 0x80U) == 0x80U)
      {
        ++ones_temp;
      }
      return;
    }
    else if (index == 100000U)
    {
      index = 0x00U;
      if (ones_temp != last_ones)
      {
        different = true;
        TIM2->CCER2|=TIM2_CCER2_CC3E;/*INDICATED DEFFERENSE BETWEN SAMPLES*/
      }
      else
      {
        different = false;
        TIM2->CCER2&=~TIM2_CCER2_CC3E;/*NOT BLINKING BECAUSE SAMPLES IS EQUAL*/
      }
      last_ones = ones_temp;
      ones = ones_temp;
      ones_temp = 0x00U;
      count_send = 0x00U;
      /*DISABLE INTERRUPT BECAUSE WE SENT 1 BYTE*/
      volatile uint8_t norming_value = (uint8_t) (last_ones/0x188U);/*norming value is 100000/255 = 392*/
      UART_Send(norming_value);
      ++count_send;
      UART1->CR2 |= UART1_CR2_TCIEN; /*ENABLE TRANSMIT COMPLETE INTERRUPT*/
      return;
    }
  }
} 
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{
  switch (count_send)
  {
  case 1:
    UART1->SR &= ~UART1_SR_TXE;
    UART_Send(0x0D);
    ++count_send;
    break;
  case 2:
    UART1->SR &= ~UART1_SR_TXE;
    UART_Send(0x0A);
    UART1->CR2 &= ~UART1_CR2_TCIEN; /*DISABLE TRANSMIT COMPLETE INTERRUPT*/
    ++count_send;
    break;
  default:
    UART1->SR &= ~UART1_SR_TXE;
    break;
  }
}