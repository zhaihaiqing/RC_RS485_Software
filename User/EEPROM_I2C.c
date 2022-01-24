
#include "main.h"
//#include "EEPROM_I2C.h"

//适用于AT24C系列 ,根据器件容量大小，修改I2C_PAGESIZE即可
#define EEPROM_ADDR			      0xA0			//EEPROM器件地址，格式：1 0 1 0  A2 A1 A0 R/W
#define I2C1_SLAVE_ADDRESS7   0x30     //自身地址
#define ClockSpeed            50000   //速率50K
#define I2C_PAGESIZE		      64    //AT24C64D每页64byte

#define I2C_FLAG_TimeOut  		0x5000     //超时常量 0x5000
#define I2C_LONG_TimeOut  		(10 * I2C_FLAG_TimeOut)

/*****************************************************************************
//EEPROM数据存储结构，，，单字节对齐
//
	*****************************************************************************/


/*******************************************************************************
* Function Name  : I2C1_Configuration
* 
*******************************************************************************/
void I2C1_Configuration(void)
{
  //定义GPIO结构体，定义I2C结构体
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
  /* Enable I2C1 和GPIO clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1| RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);//I2C1端口重映射为PB8、PB9
	
  /* Reset I2C1 peripheral */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,  ENABLE);
  /* Release reset signal of I2C1 IP */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
 
  /* Configure I2C1 pins: SCL and SDA --GPIO8 and GPIO9 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//外设复用模式
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  /* I2C1 configuration ------------------------------------------------------*/
  I2C_DeInit(I2C1);//I2C初始化
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//配置为I2C模式
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//读参数只有在I2C工作在快速模式下（时钟工作频率高于100KHz）下才有意义
  I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;//设置第一个设备的自身地址
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//使能应答，
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//AT24C64地址为7位，所以设置7位
  I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;//设置时钟速度，
  I2C_AcknowledgeConfig(I2C1, ENABLE);  //允许1字节1应答模式
 
  I2C_Init(I2C1, &I2C_InitStructure);
  I2C_Cmd(I2C1, ENABLE);	
}
/****************************************************
**函数名:I2C_Standby_24C
**功能:24C是否准备好再写入的判断
**注意事项:本函数可以理解为:判忙
****************************************************/
void I2C_Standby_24C(void)      
{
  unsigned short int SR1_Tmp=0;
  unsigned int timeout=0;
  timeout = I2C_LONG_TimeOut;//设置超时变量
  do
  {
    /*起始位*/
    I2C_GenerateSTART(I2C1, ENABLE);
    /*读SR1*/
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /*器件地址(写)*/
    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
		if(timeout == (I2C_LONG_TimeOut - 1000))I2C1_Configuration();
  }while((!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002)) && (timeout--));
  /**/
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
  /*发送停止位*/    
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/***************************************************
**函数名:I2C1_ReadS_24C
**功能:读取EEPROM多个字节，最大8192
**
***************************************************/
void I2C1_ReadS_24C(unsigned short addr ,unsigned char * pBuffer,unsigned short Length)
{
  unsigned int timeout;
  if(Length==0)return;  //长度为0直接返回
  timeout = I2C_FLAG_TimeOut;//设置超时变量
	I2C_Standby_24C();
	
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && (timeout--));
  /*允许1字节1应答模式*/
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  /* 发送起始位 */
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));/*EV5,主模式*/
  /*发送器件地址(写)*/
  I2C_Send7bitAddress(I2C1,  EEPROM_ADDR, I2C_Direction_Transmitter);
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));
  /*发送地址,,地址分两部分，高地址是字节地址，低地址是页地址*/
  I2C_SendData(I2C1, (addr & 0xff00) >> 8); //发送高8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*数据已发送*/
  I2C_SendData(I2C1, addr & 0x00ff); //发送低8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*数据已发送*/
  /*起始位*/
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
  /*器件读*/
  I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (timeout--));
  while (Length)
  {
    if(Length==1)
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);	//最后一位后要关闭应答的
			I2C_GenerateSTOP(I2C1, ENABLE);			//发送停止位
		}

		timeout = I2C_FLAG_TimeOut;//设置超时变量
		while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) && (timeout--)); /* EV7 */
		*pBuffer = I2C_ReceiveData(I2C1);
		pBuffer++;
		/* Decrement the read bytes counter */
		Length--;
  }
  //再次允许应答模式
  I2C_AcknowledgeConfig(I2C1, ENABLE);
}
/*************************************************
**函数名:I2C_PageWrite_24C
**功能:写入一页(以内)  最大32byte
*************************************************/
unsigned char I2C_PageWrite_24C(unsigned short addr,unsigned char* pBuffer, unsigned char Length)
{
  unsigned int timeout;
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && (timeout--));
	if(!timeout)return ERROR;

  /*起始位*/
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
	if(!timeout)return ERROR;	

  /*器件地址(写)*/
  I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));
	if(!timeout)return ERROR;

  /*写地址值*/
  I2C_SendData(I2C1, (addr & 0xff00) >> 8); //发送高8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*数据已发送*/
	if(!timeout)return ERROR;
  I2C_SendData(I2C1, addr & 0x00ff); //发送低8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*数据已发送*/
	if(!timeout)return ERROR;

  while(Length--)  
  {
    I2C_SendData(I2C1, *pBuffer); 
    pBuffer++; 
    timeout = I2C_FLAG_TimeOut;//设置超时变量
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));
		if(!timeout)return ERROR;
  }
  /*停止位*/
  I2C_GenerateSTOP(I2C1, ENABLE);
  return SUCCESS;
}
/*************************************************
**函数名:I2C_WriteS_24C
**功能:页写入24C
*************************************************/
unsigned char I2C1_WriteS_24C(unsigned short addr,unsigned char* pBuffer,  unsigned short Length)
{
	unsigned char temp;
	//1.先把页不对齐的部分写入
	temp=addr % I2C_PAGESIZE;
	I2C_Standby_24C(); //判断是否忙
	if(temp)
	{
    temp=I2C_PAGESIZE-temp;
    if(Length >= temp)
    {
      I2C_PageWrite_24C(addr,pBuffer,  temp);
      Length-=temp;
      addr+=temp;
      pBuffer+=temp;
    }
    else
    {
      if(I2C_PageWrite_24C(addr,pBuffer,  Length) == ERROR )return ERROR;
      Length = 0;
    }
		I2C_Standby_24C(); //判断是否忙
	}
	//2.从页对齐的地方开始写
	while(Length)
	{
		if(Length>=I2C_PAGESIZE)
		{
			if(I2C_PageWrite_24C(addr,pBuffer,  I2C_PAGESIZE) == ERROR )return ERROR; //
			Length-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
			pBuffer+=I2C_PAGESIZE;
			I2C_Standby_24C();  //判断是否忙
		}
		else
		{
			if(I2C_PageWrite_24C(addr,pBuffer,  Length) == ERROR )return ERROR;
			Length=0;
			I2C_Standby_24C(); //判断是否忙
		}
	}
	return SUCCESS;
}

/*******************************************************************************
* Function Name  : EEWrite
* Description    : EEWrite写函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEInit()
{
	I2C1_Configuration();
}

/*******************************************************************************
* Function Name  : EEWrite
* Description    : EEWrite写函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEWrite(unsigned short addr ,unsigned char * pBuffer,unsigned short Length)
{
	I2C1_WriteS_24C(addr,pBuffer,Length);
}

/*******************************************************************************
* Function Name  : EERead
* Description    : EERead读函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EERead(unsigned short addr ,unsigned char * pBuffer,unsigned short Length)
{
	I2C1_ReadS_24C(addr,pBuffer,Length);
}

/*******************************************************************************
* Function Name  : EEErase
* Description    : EEPROM擦除函数，写0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEErase(unsigned short addr, unsigned short Length)
{
	unsigned char Data_temp=0x00;
	I2C1_WriteS_24C(addr,&Data_temp,Length);
}

/*******************************************************************************
* Function Name  : EEPROM_Check
* Description    : EEPROM校验程序，向EEPROM中写入8个数据，读出，检查是否一致
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char EECheck(void)
{
	unsigned char counti=0,countj=0;
	unsigned char I2C_WriteData[8]={0x00,0x01,0x02,0x03,0x04,0x05,0x07,0x08};
	unsigned char I2C_ReadData[8];
	
	I2C1_WriteS_24C(0x0000,I2C_WriteData,8);
	Delay(5);
	I2C1_ReadS_24C(0x0000,I2C_ReadData,8);
	
	for(counti=0;counti<8;counti++)
	{
		if(I2C_ReadData[counti]==I2C_WriteData[counti])
		{
			countj++;
		}
	}
	if(countj==8)
		return SUCCESS;
	else
		return ERROR;
}




