
#include "main.h"

//��ʼ��ADC															   
void  Adc_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

//	//�ȳ�ʼ��ADC1ͨ��5 IO��
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 ͨ��5
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
// 
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//������ת��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		  //����ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   //��������ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC�ⲿ���أ��ر�״̬
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //���뷽ʽ,ADCΪ12λ�У��Ҷ��뷽ʽ
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 //����ͨ������1��
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channel13 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5);
                          //ADCͨ���飬 ��13��ͨ�� ����˳��1��ת��ʱ�� 
 
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);	  //ADC���ʹ��
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);  //����ADC1
	
	 /* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	  //����У׼
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));  //�ȴ�����У׼���
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		//��ʼУ׼
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));	   //�ȴ�У׼���

}	

//���ADCֵ
//ch: @ref ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
//����ֵ:ת�����
unsigned short int Get_Adc(unsigned char ch)
{
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);	//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
		
		
		
	
	







