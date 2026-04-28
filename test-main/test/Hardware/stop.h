#ifndef __STOP_H
#define __STOP_H
#include "stdio.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_pwr.h"
#include "stm32l4xx_hal_pwr_ex.h"

extern UART_HandleTypeDef hlpuart1;
extern RTC_HandleTypeDef hrtc;
extern uint8_t lpuart_buf;

void Enter_Sleep(void);
void Enter_Stop2(void);
HAL_StatusTypeDef RTC_Set_Alarm(uint32_t sec);


#endif

