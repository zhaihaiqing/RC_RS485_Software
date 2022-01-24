
#include "main.h"

volatile unsigned char RTC_SEC_FLAG=0;

unsigned char const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5};//月修正数据表，平年的月份日期表
unsigned char const mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};//
calendar_obj calendar;


unsigned char Is_Leap_Year(unsigned short int year)
{
	if( ( (year%4==0) && (year%100!=0) ) || (year%400==0) )return 1;
	else return 0;
}

unsigned char RTC_Get_Week(unsigned short int year,unsigned char month,unsigned char day)
{
	int c=0;
	int y=0;
	int week=0;
	if(month==1 || month==2)
	{
		year--;
		month+=12;
	}
	c=year/100;
	y=year-c*100;
	week=(c/4)-2*c+(y+y/4)+(13*(month+1)/5)+day-1;
	while(week<0)week+=7;
	week%=7;
	return week;
}

unsigned char RTC_Set(unsigned short int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
	unsigned short int t=0;
	unsigned int seccount=0;
	if(syear<1970 || syear>2099)return ERROR;
	for(t=1970;t<syear;t++)//把所有的年份秒相加
	{
		if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒数
		else seccount+=31536000;//平年的秒数
	}
	smon-=1;
	for(t=0;t<smon;t++)//把前面的月份的秒钟相加
	{
		seccount+=(unsigned int)mon_table[t]*86400;//月份秒钟相加
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年 2月份增加1天的秒数
	}
	seccount+=(unsigned int)(sday-1)*86400;//把前面的日期的秒钟数相加
	seccount+=(unsigned int)hour*3600;//小时秒数相加
	seccount+=(unsigned int)min*60;//分钟数
	seccount+=sec;//最后的秒钟
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE );//使能PWR和BKP时钟
	PWR_BackupAccessCmd(ENABLE);//使能RTC和后备寄存器访问
	RTC_SetCounter(seccount);//设置RTC计数器的值
	RTC_WaitForLastTask();//等待完成
	return SUCCESS;
}



unsigned char Internal_RTC_Init(void)
{
	unsigned char temp=0;
	
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//检查是不是第一次配置时钟
	if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050)//从指定的后备寄存器中读出数据
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP , ENABLE);//使能PWR和BKP外设时钟
		PWR_BackupAccessCmd(ENABLE);//使能后备寄存器访问
		BKP_DeInit();               //复位备份区域
		RCC_LSEConfig(RCC_LSE_ON);  //设置外部低速时钟
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET)//检查指定的RCC标志位设置，等待时钟就绪
		{
			Delay(10);
			temp++;
			if(temp>=250)return ERROR;//初始化失败
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//设置RTC时钟，选择LSE
		RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
		RTC_WaitForLastTask();//等待完成
		RTC_WaitForSynchro();//等待RTC寄存器同步
		RTC_ITConfig(RTC_IT_SEC,ENABLE);//使能RTC秒钟中断
		RTC_WaitForLastTask();//等待RTC完成
		RTC_EnterConfigMode();//进入配置模式
		RTC_SetPrescaler(32767);//设置RTC时钟预分频值
		RTC_WaitForLastTask();//等待完成
		RTC_Set(2017,07,17,12,0,0);//设置时间
		RTC_ExitConfigMode();//退出配置模式
		BKP_WriteBackupRegister(BKP_DR1,0x5050);//向指定的后备寄存器中写入数据
	}
	else
	{
		RTC_WaitForSynchro();//等待寄存器完成同步
		RTC_ITConfig(RTC_IT_SEC,ENABLE);//使能秒钟中断
		RTC_WaitForLastTask();	//等待完成
	}
	RTC_Get();//更新时间
	return SUCCESS;
}

unsigned char RTC_Get(void)
{
	static unsigned short int daycnt=0;
	unsigned int timecount=0;
	unsigned int temp=0;
	unsigned short int temp1=0;
	timecount =RTC->CNTH;//得到计数器中的值
	timecount<<=16;
	timecount+=RTC->CNTL;
	
	temp=timecount/86400;//得到天数
	if(daycnt!=temp)//超过了1天
	{
		daycnt=temp;
		temp1=1970;
		while(temp>365)
		{
			if(Is_Leap_Year(temp1))//闰年
			{
				if(temp>=365)temp-=366;//闰年的秒钟数
				else{temp1++;break;}
			}
			else temp-=365;//平年
			temp1++;
		}
		calendar.w_year=temp1;//得到月份
		temp1=0;
		while(temp>=28)//超过了1个月
		{
			if(Is_Leap_Year(calendar.w_year) && temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break;
			}
			else
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;
		}
		calendar.w_month=temp1+1;//得到月份
		calendar.w_date=temp+1;//得到日期
	}
	temp=timecount%86400;
	calendar.hour=temp/3600;
	calendar.min=(temp%3600)/60;
	calendar.sec=(temp%3600)%60;
	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);
	return SUCCESS;
}
		
		
		
	
	







