/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
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

/**
  ******************************************************************************
  * @Description
  *
  * �������ǽ�������MKI062V3��֤�壨�����壩�ϵģ�ּ����֤���ٶȴ��������ų���
  *��IMU��Ԫ������ѹ�Ƶȴ������Ĳ��ԣ������������ʵ�֡�
  *
  *V2�汾ּ�ڲ��Դ��ڴ����ַ������ݵ�����������֤����ATָ����ܡ�
*/

/* Includes ------------------------------------------------------------------*/


#include "main.h"


volatile unsigned int SysTick_Count = 0;
volatile unsigned int TimingDelay = 0;

/*******************************************************************************
* Function Name  : Delay function
* Description    : ��ʱ����������ʱ����sleepģʽ
* Input          : ��ʱʱ����ms��
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(volatile unsigned int nTime)
{ 
	//TIM6_Reconfiguration(nTime);
	TimingDelay = nTime;
	while(TimingDelay != 0)__WFI;//�ȴ������н���sleepģʽ,�·�ָ����ִ�й��������׽���
	
	//Ӳ���쳣ģʽ
	//PWR_EnterSleepMode(PWR_Regulator_LowPower,PWR_SLEEPEntry_WFI);
	//TIM6_Disable();
	
}
/*******************************************************************************
* Function Name  : SysTick init function
* Description    : ���ö�ʱ���ȼ�SysTick���ȼ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_SysTick(void)
{
	SysTick_Config(SystemCoreClock / 1000); //���ö�ʱ���ȣ�1ms
	NVIC_SetPriority(SysTick_IRQn, 0x0);    //SysTick�ж����ȼ�����
}

/*******************************************************************************
* Function Name  : Main function
* Description    : None
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{ 
	unsigned char USART1_RX[256];
	unsigned char USART2_RX[256];
	unsigned char USART3_RX[256];
	unsigned char test=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�ж����ȼ�����
	UART3_Configuration(115200);printf("Init_UART3 OK!\r\n");
	printf("Sys_clock:%dHz\r\n",SystemCoreClock);
	Init_SysTick();	printf("Init_SysTick OK!\r\n");
	GPIO_Configuration();printf("GPIO_Configuration OK!\r\n");
	EEInit();
	if(EECheck())printf("AT24C64D_EEPROM Init OK!\r\n");
	else printf("AT24C64D_EEPROM Init Failure!\r\n");
	if(Internal_RTC_Init())printf("RTC Init OK!\r\n");
	else printf("RTC Init Failure!\r\n");
	Adc_Init();
	printf("Hardware ready!\r\n");

	LED0_ON();
	LED1_ON();
	//RTC_Set(2017,7,18,20,10,15);
	
	while(1)
	{
		if(Uart3Flag)
		{
			printf("Len:%d\r\n",USART3_RX_Len);
			USART3_ClearBuf_Flag();
			printf("USART3 RX Complete!\r\n");
			printf("Get_Adc:%fV\r\n",(float)Get_Adc(ADC_Channel_0)*3.3/4096);
		}
		if(RTC_SEC_FLAG)
		{
			RTC_SEC_FLAG=0;
			RTC_Get();
			printf("%d��%d��%d�գ�%d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
		}
	}
	if(0)SystemResetSoft();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
