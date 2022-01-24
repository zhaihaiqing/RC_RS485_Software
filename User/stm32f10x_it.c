/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "main.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	SysTick_Count++;
	if(SysTick_Count==1950)LED2_ON();
	else if(SysTick_Count==2000)
	{
		SysTick_Count=0;
		LED2_OFF();
	}
	if(USART1_INTERVAL_TIME)USART1_INTERVAL_TIME--;
	else if(USART1_RX_Len && !Uart1Flag)Uart1Flag = 1;
	if(USART2_INTERVAL_TIME)USART2_INTERVAL_TIME--;
	else if(USART2_RX_Len && !Uart2Flag)Uart2Flag = 1;
	if(USART3_INTERVAL_TIME)USART3_INTERVAL_TIME--;
	else if(USART3_RX_Len && !Uart3Flag)Uart3Flag = 1;
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : ��ʱ��4�жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
	unsigned char PWM_SPEED=1;
	 if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	 { 
			//CCR2_Val_Add+=1;						//����1Hz
			//CCR2_Val=	HSE_VALUE/CCR2_Val_Add - 1; //���¼����ֵ
			//TIM4->ARR = CCR4_Val;					//װ���ֵ
		    //TIM4->CCR4 = CCR4_Val/2;				    //����ռ�ձ�
		  //if(CCR2_Val_Add>=1200) {CCR2_Val_Add=400;TIM_Cmd(TIM4, DISABLE);}
		 //TIM4->ARR    = CCR4_Val;    //�Զ���װ������ֵ,����PWM���Ƶ��
		 //TIM4->CCR4   = CCR4_Val/2;  //��������źŵ�ռ�ձȣ�50%
		 if(P_Val_bit==0)
		 {
			 P_Val=P_Val+PWM_SPEED;
			 if(P_Val>=(CCR4_Val/2 - PWM_SPEED))P_Val_bit=1;
		 }
		 
		  if(P_Val_bit==1)
		 {
			 P_Val=P_Val-PWM_SPEED;
			 if(P_Val<=5)P_Val_bit=0; 
		 }
		 TIM4->CCR4   = P_Val;
		 TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
   }
}

void USART2_IRQHandler(void)
{
	unsigned char 	Uart_Get_Data;	//����2���յ�����
	UART2_RBUF_ST *p = &uart2_rbuf;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Uart_Get_Data = USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		if(!Uart2Flag)
		{
			if((((p->out - p->in) & (UART2_RBUF_SIZE - 1)) == 0) || USART2_INTERVAL_TIME)
			{
				USART2_INTERVAL_TIME=Default_USART2_INTERVAL_TIME;
				if((p->in - p->out)<UART2_RBUF_SIZE)
				{
					p->buf [p->in & (UART2_RBUF_SIZE-1)] = Uart_Get_Data;	
					p->in++;
				}
				USART2_RX_Len  = (p->in - p->out) & (UART2_RBUF_SIZE - 1);//��ȡ���ݳ���
			}
			
		}
	}
}

void USART3_IRQHandler(void)
{
	unsigned char 	Uart_Get_Data;	//����2���յ�����
	UART3_RBUF_ST *p = &uart3_rbuf;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		Uart_Get_Data = USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		if(!Uart3Flag)
		{
			if((((p->out - p->in) & (UART3_RBUF_SIZE - 1)) == 0) || USART3_INTERVAL_TIME)
			{
				USART3_INTERVAL_TIME=Default_USART3_INTERVAL_TIME;
				if((p->in - p->out)<UART3_RBUF_SIZE)
				{
					p->buf [p->in & (UART3_RBUF_SIZE-1)] = Uart_Get_Data;	
					p->in++;
				}
				USART3_RX_Len  = (p->in - p->out) & (UART3_RBUF_SIZE - 1);//��ȡ���ݳ���
			}
			
		}
	}
}

void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC) != RESET )//�����ж�
	{
		RTC_SEC_FLAG=1;
	}
	if(RTC_GetITStatus(RTC_IT_ALR) != RESET )//�������ж�
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);//�������ж�
	}
	RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);//�������ж�
	RTC_WaitForLastTask();
}


//void USART2_IRQHandler(void)
//{
//	unsigned char 	Uart_Get_Data;	//����2���յ�����
//	UART2_RBUF_ST *p = &uart2_rbuf;
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//	{
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
//		
//		Uart_Get_Data = USART_ReceiveData(USART2);
//		if((p->in - p->out)<UART2_RBUF_SIZE)
//		{
//			p->buf [p->in & (UART2_RBUF_SIZE-1)] = Uart_Get_Data;	
//			p->in++;
//			//Uart2Flag = 1;
//		}
//		
//	}
//}


//void TIM6_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM6, TIM_IT_Update); //�����־λ
//		if(TimingDelay)TimingDelay--;
//	}
//}



/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
