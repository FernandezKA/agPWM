#include "stm8s_conf.h"
static  uint32_t index;
static  uint32_t ones_temp;
static  uint32_t ones;
static  uint32_t last_ones;
static  uint32_t size = 100000U;
static  uint32_t norming_size = 1000000000U;
static  uint8_t count_send = 0X00U;
static  uint16_t pwm_index = 0x01U;
static  bool different = false;
static  bool completed = false;
void SysConfig(void);
void UART_Send(const uint32_t data);/*FUNCTION SEND 1 BYTE AT UART*/
void UART_Config(void);/*AT THIS FUNCTION WE CONFIGURED UART PARAMETERS, BAUDRATE*/
void CLK_Config(void);/*AT THIS FUNCTION SETTONG CLOCKING FOR CPU, Fmaster, ENABLE PERIPH CLOCKING*/
void GPIO_Config(void);/*SET GPIO PARAMETERS FOR ALL PIN*/
void TIM1_Config(void);/*TIM1 USING FOR PWM GENERATION*/
//void TIM2_Config(void);/*TIM2 USING FOR LED INDICATING*/
void TIM4_Config(void);/*TIM4 USING FOR GPIO STATE ASKING*/
void PWM(const uint16_t value);/*positive*/