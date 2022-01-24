
#include "main.h"

//初始化ADC															   
void  Adc_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

//	//先初始化ADC1通道5 IO口
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 通道5
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
// 
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//独立的转换模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		  //开启扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   //开启连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC外部开关，关闭状态
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //对齐方式,ADC为12位中，右对齐方式
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 //开启通道数，1个
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channel13 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5);
                          //ADC通道组， 第13个通道 采样顺序1，转换时间 
 
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);	  //ADC命令，使能
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);  //开启ADC1
	
	 /* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	  //重新校准
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));  //等待重新校准完成
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		//开始校准
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));	   //等待校准完成

}	

//获得ADC值
//ch: @ref ADC_channels 
//通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
unsigned short int Get_Adc(unsigned char ch)
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);	//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
		
		
		
	
	







