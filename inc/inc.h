#include "stm8s_conf.h"

void SysConfig(void);
void UART_Send(const unsigned char data);/*FUNCTION SEND 1 BYTE AT UART*/
void UART_Config(void);/*AT THIS FUNCTION WE CONFIGURED UART PARAMETERS, BAUDRATE*/
void CLK_HSE_Config(void);/*Define clocking at external generator*/
void CLK_HSI_Config(void);/*Define clocking at internal RC - circuilt*/
void GPIO_Config(void);/*SET GPIO PARAMETERS FOR ALL PIN*/
void TIM1_Config(void);/*TIM1 USING FOR PWM GENERATION*/
void TIM2_Config(void);/*TIM2 USING FOR LED INDICATING*/
void TIM4_Config(void);/*TIM4 USING FOR GPIO STATE ASKING*/
/*uart -> send 1 byte*/
/*max value 0xFF*/
/**********************************Variables************************************/
static  uint32_t index;
static  uint32_t ones_temp;
static  uint32_t ones;
static  uint32_t last_ones;
static  uint32_t size = 60000U;
static  uint8_t count_send = 0X00U;
static  bool different = false;
static  bool completed = false;