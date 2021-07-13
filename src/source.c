/*
* Author: Fernandez K. A. 
* Date: 2021-03-23
*/
#include "inc.h"
void UART_Config(void)
{
  /*BAUDRATE 9600 kbps, MASTER CLOCKING = 8 MHz*/
  CLK->PCKENR1 |= CLK_PCKENR1_UART1; /*ENABLE UART CLOCKING*/
  UART1->BRR1 = 0x34;
  UART1->BRR2 = 0x01;
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
void CLK_HSE_Config(void)
{
  CLK->ECKR|=CLK_ECKR_HSEEN;
  CLK->SWCR|=CLK_SWCR_SWEN;
  while(CLK->ECKR&CLK_ECKR_HSERDY!=CLK_ECKR_HSERDY) {asm("nop");} //Ждем готовности источника тактирования
  CLK->CKDIVR = 0x00;                    // Предделитель равен нулю
  CLK->SWR = 0xB4;                    // Выбираем HSE источником тактовой частоты
  while (CLK->SWCR&CLK_SWCR_SWIF!=CLK_SWCR_SWIF){asm("nop");}   // Ждем готовности переключения 
}
void CLK_HSI_Config(void){
   CLK->CKDIVR=(1U<<3);/*Set prescaler Frc/2*/
}
void GPIO_Config(void)
{
  /*config pin for sampling*/
  GPIOC->CR1 |= (1U << 7); /*SET INPUT WITH PULL-UP*/
  /*This strings using for testing frequency of sampling*/
  //GPIOD->DDR|=(1U<<2);
  //GPIOD->CR1|=(1<<2);
}

void TIM1_Config(void)
{
  CLK->PCKENR2 |= CLK_PCKENR1_TIM1; /*ENABLE TIM1 clocking*/
  TIM1->CR1 |= TIM1_CR1_ARPE;/*enable autoreload*/
  TIM1->PSCRH = 32000 >> 8;/*set prescaler*/
  TIM1->PSCRL = 32000 & 0xFF;  /*PRESCALER 32000*/
  TIM1->ARRH = 1000U >> 8; /*16 MHz/32000/1000 = 0,5 Hz*/
  TIM1->ARRL = 1000U & 0xFF;
  TIM1->IER|=TIM1_IER_UIE;
  TIM1->BKR |= TIM1_BKR_MOE;
}
void TIM2_Config(void){
  /*THIS TIMER WE USING FOR GENERATE BLINKING LED
  *FOR GENERATE USING LOW FREQUENCY PWM WITH 50% DUTY*/
  CLK->PCKENR1|=CLK_PCKENR1_TIM2;/*ENABLE CLOCKING*/
  TIM2->CR1|=TIM2_CR1_ARPE;
  TIM2->PSCR|=(1U<<3|1U<<2|1U<<1);/*SET PRESCALER 2^15*/
  TIM2->ARRH = 488U>>8;
  TIM2->ARRL = 488U&0xFFU;
  TIM2->CCR1H = 244U>>8;
  TIM2->CCR1L = 244U&0xFFU;
  TIM2->CCR2H = 244U>>8;
  TIM2->CCR2L = 244U&0xFFU;
  TIM2->CCR3H = 244U>>8;
  TIM2->CCR3L = 244U&0xFFU;
  TIM2->CCER1|=TIM2_CCER1_CC2E;/*ENABLE CAPTURE/COMPARE FOR CHANNEL 2 AND 3*/
  TIM2->CCER2|=TIM2_CCER2_CC3E;
  TIM2->CCER2|=TIM2_CCER2_CC3P;/*UNPHASED BLINKING*/
  TIM2->CCMR1|=(1U<<6|1U<<5|1U<<3);/*MODE 1 WITH OUTPUT COMPARE PRELOAD*/
  TIM2->CCMR2|=(1U<<6|1U<<5|1U<<3);/*MODE 1 WITH OUTPUT COMPARE PRELOAD*/
  TIM2->CCMR3|=(1U<<6|1U<<5|1U<<3);/*MODE 1 WITH OUTPUT COMPARE PRELOAD*/
  TIM2->CR1|=TIM2_CR1_CEN;/*RUN TIM2*/
}
void TIM4_Config(void)
{
  /*This timer using for definition frequency of sampling*/
  CLK->PCKENR1 |= CLK_PCKENR1_TIM4;
  TIM4->IER |= TIM4_IER_UIE; /*ENABLE INTERRUPT UPDATE*/
  TIM4->IER |= TIM4_IER_RESET_VALUE;
  TIM4->PSCR = (0x00U); /*SET PRESCALER = 2^0 = 1*/
  TIM4->ARR = 0x4FU;
  ITC->ISPR6 &= 0;
  ITC->ISPR6 |= 0x03U;
  TIM4->SR1 = ~TIM4_SR1_UIF; /*clear uif bit at SREG for correct working*/
  TIM4->CR1 |= TIM4_CR1_CEN;
}
/*************************************Block of Interrupt***********************/
/*INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
  if (TIM4->SR1 & TIM4_SR1_UIF == TIM4_SR1_UIF)
  {
    TIM4->SR1 = (uint8_t) ~(TIM4_SR1_UIF);//Clear status register for out from IRQ
    
    if(index < size)
    {
      ++index;
      volatile unsigned char temp = GPIOC->IDR;//read value at input port
      ((temp&0x80U) == 0x80U)? (++ones_temp) : (ones_temp = ones_temp);
    }
    ///////////////////////////////////////////////////////////////////////////
    if(index == size)
    {
      //This block for indicate of difference
      if(ones_temp != last_ones)
      {
        different = true;
        TIM2->PSCR=(1U<<3|1U<<2);//EDIT FREQ BLINKING AT 2
        TIM1->CR1|=TIM1_CR1_CEN; //Begin count at stop
      }else
      {
        different = false;
      }
      
      last_ones = ones_temp;//Write new value of fill from old value
      volatile uint8_t norming_value = (uint8_t) (last_ones/0xEBU);//norming value is 60000/255 = 235
      UART_Send(norming_value);
      ones_temp = 0x00U;
      count_send = 1U;
      UART1->CR2 |= UART1_CR2_TCIEN; //ENABLE TRANSMIT COMPLETE INTERRUPT
    }
  }
  
}*/
  /*At this IRQ we answering state of pin, and write into variable*/
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
 if (TIM4->SR1 & TIM4_SR1_UIF == TIM4_SR1_UIF)
  {
    
    TIM4->SR1 = (uint8_t) ~(TIM4_SR1_UIF);//Clear status register for out from IRQ
    if (index < size)
    {
      ++index;
      volatile unsigned char temp = GPIOC->IDR;//read value at input port
      if ((temp & 0x80U) == 0x80U)
      {
        ++ones_temp;
        //GPIOD->ODR ^= (1U << 2); //string for testing frequency sampling

      }
      return;
    }  
    if (index == size)
    {
      index = 0x00U;
      if (ones_temp != last_ones)
      {
        different = true;
        //TIM2->CCER2|=TIM2_CCER2_CC3E;//INDICATED DEFFERENSE BETWEN SAMPLES
        //We using TIM1 for definition time of fast - blinking
        TIM2->PSCR=(1U<<3|1U<<2);//EDIT FREQ BLINKING AT 2
        TIM1->CR1|=TIM1_CR1_CEN;
      }
      else
      {
        different = false;
        //TIM2->CCER2&=~TIM2_CCER2_CC3E;//NOT BLINKING BECAUSE SAMPLES IS EQUAL//
      }
      last_ones = ones_temp;
      ones = ones_temp;
      ones_temp = 0x00U;
      count_send = 0x00U;
      //DISABLE INTERRUPT BECAUSE WE SENT 1 BYTE
      volatile uint8_t norming_value = (uint8_t) (last_ones/0xEBU);//norming value is 60000/255 = 235
      UART_Send(norming_value);
      ++count_send;
      //For next sending bytes we using IRQ at TXE
      UART1->CR2 |= UART1_CR2_TCIEN; //ENABLE TRANSMIT COMPLETE INTERRUPT
      return;
    }
  }
} 
/*******************************************************************************/
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
/*******************************************************************************/

INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
  /*At this IRQ we disable fast - blinking of led, edit freq of TIM2*/
    TIM1->SR1=~TIM1_SR1_UIF;/*CLEAR EVENT FLAG*/
    TIM1->CR1&=~TIM1_CR1_CEN;/*DISABLE COUNTER*/
    TIM2->PSCR|=(1U<<3|1U<<2|1U<<1);/*SET PRESCALER 2^15*/
}