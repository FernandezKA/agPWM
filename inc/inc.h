#pragma once 
#include "stm8s_conf.h"
/*UART_BLOCK*/
void UART_Config(void);
static void UART_Send(const uint16_t data);
