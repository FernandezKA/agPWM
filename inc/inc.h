#pragma once 
#include "stm8s_conf.h"
static volatile uint32_t index;
static volatile uint32_t ones_temp;
static volatile uint32_t ones;
static volatile uint32_t last_ones;
static volatile uint32_t size = 100000U;
static volatile uint8_t count_send = 0X00U;
static volatile bool different = FALSE;
void SysConfig(void);
void UART_Send(const uint32_t data);/*FUNCTION SEND 1 BYTE AT UART*/
void UART_Config(void);/*AT THIS FUNCTION WE CONFIGURED UART PARAMETERS, BAUDRATE*/
void CLK_Config(void);/*AT THIS FUNCTION SETTONG CLOCKING FOR CPU, Fmaster, ENABLE PERIPH CLOCKING*/
void GPIO_Config(void);/*SET GPIO PARAMETERS FOR ALL PIN*/
void TIM1_Config(void);/*TIM2 USING FOR PWM GENERATION*/
void TIM4_Config(void);/*TIM4 USING FOR GPIO STATE ASKING*/