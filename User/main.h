#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32f10x.h"
#include "uart.h"
#include "GPIO.h"
#include "Timer.h"
#include "EEPROM_I2C.h"
#include "RTC.h"
#include "ADC.h"


#define SystemResetSoft()			*((uint32_t *)0xe000ed0c)=0x05fa0004; //实现系统软件复位

#define LED0_ON()    GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define LED0_OFF()   GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define LED1_ON()    GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define LED1_OFF()   GPIO_SetBits(GPIOA,GPIO_Pin_6)

#define LED2_ON()    GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define LED2_OFF()   GPIO_SetBits(GPIOA,GPIO_Pin_7)


extern volatile unsigned int SysTick_Count;
extern volatile unsigned int TimingDelay;

void Delay(volatile unsigned int nTime);


#endif
