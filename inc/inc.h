#pragma once 
#include "stm8s_conf.h"
void SysConfig(void);
/*UART_BLOCK*/
void UART_Send(const uint8_t data);
void UART_Config(void);
void CLK_Config(void);
void GPIO_Config(void);
void TIM1_Config(void);
void TIM4_Config(void);