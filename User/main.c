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
  * 本工程是建立在在MKI062V3验证板（开发板）上的，旨在验证加速度传感器、磁场计
  *（IMU单元）、气压计等传感器的测试，相关驱动代码实现。
  *
  *V2版本旨在测试串口处理字符型数据的能力，并验证类似AT指令集功能。
*/

/* Includes ------------------------------------------------------------------*/


#include "main.h"


volatile unsigned int SysTick_Count = 0;
volatile unsigned int TimingDelay = 0;

/*******************************************************************************
* Function Name  : Delay function
* Description    : 延时函数，空闲时进入sleep模式
* Input          : 延时时长（ms）
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(volatile unsigned int nTime)
{ 
	//TIM6_Reconfiguration(nTime);
	TimingDelay = nTime;
	while(TimingDelay != 0)__WFI;//等待过程中进入sleep模式,下方指令在执行过程中容易进入
	
	//硬件异常模式
	//PWR_EnterSleepMode(PWR_Regulator_LowPower,PWR_SLEEPEntry_WFI);
	//TIM6_Disable();
	
}
/*******************************************************************************
* Function Name  : SysTick init function
* Description    : 设置定时长度及SysTick优先级
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_SysTick(void)
{
	SysTick_Config(SystemCoreClock / 1000); //设置定时长度，1ms
	NVIC_SetPriority(SysTick_IRQn, 0x0);    //SysTick中断优先级设置
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//中断优先级分组
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
			printf("%d年%d月%d日，%d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
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
