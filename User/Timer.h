#ifndef __TIMER_H
#define __TIMER_H

void TIM6_Configuration(void);
void NVIC_TIM6_Configuration(void);
void TIM6_Reconfiguration(volatile unsigned int nTime);
void TIM6_Disable(void);
void PWM_GPIO();
void PWM_Output(void);
void TIM4_CH4_PWM(void);

extern uint16_t CCR4_Val;
extern uint16_t CCR4_Val_Add;//ÆµÂÊÖµ
extern uint16_t PrescalerValue;
extern volatile unsigned short int P_Val;
extern volatile unsigned char P_Val_bit;


#endif

