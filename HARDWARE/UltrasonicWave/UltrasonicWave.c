/******************** (C) 1209 Lab **************************
 * 文件名  : UltrasonicWave.c
 * 描述    ：超声波测距模块，UltrasonicWave_Configuration（）函数
             初始化超声模块，UltrasonicWave_StartMeasure（）函数
			 启动测距，并将测得的数据通过串口1打印出来         
 * 实验平台：Mini STM32开发板  STM32F103ZET6
 * 硬件1连接：------------------
 *          | PC6  - TRIG      |
 *          | PC7  - ECHO      |
 *           ------------------
 * 硬件2连接：------------------
 *          | PC8  - TRIG      |
 *          | PC9  - ECHO      |
 *           ------------------
 * 库版本  ：ST3.5.0
 *
 * 作者    ：王永东 
*********************************************************************************/

#include "UltrasonicWave.h"
#include "usart.h"
#include "timer.h"
#include "delay.h"
#include "led.h"

u8 Flag_Obstruction=0;

u8 TIM8CH2_CAPTURE_STA = 0;	//通道2输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM8CH2_CAPTURE_UPVAL;
u16 TIM8CH2_CAPTURE_DOWNVAL;
u32 tempup2 = 0;	//捕获总高电平的时间
u32 TIM8_T2;

u8 TIM8CH4_CAPTURE_STA = 0;	//通道3输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM8CH4_CAPTURE_UPVAL;
u16 TIM8CH4_CAPTURE_DOWNVAL;
u32 tempup4 = 0;	//捕获总高电平的时间
u32 TIM8_T4;


#define	TRIG_PORT_0      GPIOC		//TRIG       
#define	ECHO_PORT_0      GPIOC		//ECHO 
#define	TRIG_PIN_0       GPIO_Pin_6 //TRIG       
#define	ECHO_PIN_0       GPIO_Pin_7	//ECHO   

#define	TRIG_PORT_1      GPIOC		//TRIG       
#define	ECHO_PORT_1      GPIOC		//ECHO 
#define	TRIG_PIN_1       GPIO_Pin_8 //TRIG       
#define	ECHO_PIN_1       GPIO_Pin_9	//ECHO  
 
/*
 * 函数名：UltrasonicWave_Configuration
 * 描述  ：超声波模块的初始化
 * 输入  ：无
 * 输出  ：无	
 */
 
 TIM_ICInitTypeDef  TIM8_ICInitStructure;
 
void UltrasonicWave_Init(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure;	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//关闭jtag
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
    
  GPIO_InitStructure.GPIO_Pin = TRIG_PIN_0;					 //PC6接TRIG
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //设为推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(TRIG_PORT_0, &GPIO_InitStructure);	                 //初始化外设GPIO 

  GPIO_InitStructure.GPIO_Pin = ECHO_PIN_0;				     //PC7接ECH0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //设为输入
  GPIO_Init(ECHO_PORT_0,&GPIO_InitStructure);						 //初始化GPIOC
	
	
	 GPIO_InitStructure.GPIO_Pin = TRIG_PIN_1;					 //PC8接TRIG
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //设为推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(TRIG_PORT_1, &GPIO_InitStructure);	                 //初始化外设GPIO 

  GPIO_InitStructure.GPIO_Pin = ECHO_PIN_1;				     //PC9接ECH0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //设为输入
  GPIO_Init(ECHO_PORT_1,&GPIO_InitStructure);						 //初始化GPIOC
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//使能TIM8时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //使能GPIOC时钟
	
	
	//初始化定时器8 TIM8	 
	TIM_TimeBaseStructure.TIM_Period = 0xffff; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//预分频器     以1Mhz的频率计数 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM8输入捕获参数
	TIM8_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM8_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM8_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM8_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM8_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM8, &TIM8_ICInitStructure);
	
	TIM8_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM8_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM8_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM8_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM8_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM8, &TIM8_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;  //TIM8中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM8,TIM_IT_CC2|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	//TIM_ITConfig(TIM8,TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	
  TIM_Cmd(TIM8,ENABLE ); 	//使能定时器8
	
}

/*
 * 函数名：UltrasonicWave_StartMeasure
 * 描述  ：开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWave_StartMeasure(u8 cs)
{
  if(cs==1)
	{
		GPIO_SetBits(TRIG_PORT_0,TRIG_PIN_0);  //送>10US的高电平RIG_PORT,TRIG_PIN这两个在define中有?
		delay_us(20);		                      //延时20US
		GPIO_ResetBits(TRIG_PORT_0,TRIG_PIN_0);
	}
	else if(cs==2)
	{
		GPIO_SetBits(TRIG_PORT_1,TRIG_PIN_1);  //送>10US的高电平RIG_PORT,TRIG_PIN这两个在define中有?
		delay_us(20);		                      //延时20US
		GPIO_ResetBits(TRIG_PORT_1,TRIG_PIN_1);
	}
}

		
//定时器8中断服务程序,用CH2作为超声波转换的通道 即PC7端口
void TIM8_CC_IRQHandler(void)
{
		if ((TIM8CH2_CAPTURE_STA & 0X80) == 0) 		//还未成功捕获	
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET) 		//捕获1发生捕获事件
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC2); 		//清除中断标志位
			if (TIM8CH2_CAPTURE_STA & 0X40)		//捕获到一个下降沿
			{
				TIM8CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM8);//记录下此时的定时器计数值
				if (TIM8CH2_CAPTURE_DOWNVAL < TIM8CH2_CAPTURE_UPVAL)
				{/* 如果计数器初始值大于末尾值，说明计数器有溢出 */
					TIM8_T2 = 65535;
				}
				else
				TIM8_T2 = 0;  
				tempup2 = TIM8CH2_CAPTURE_DOWNVAL - TIM8CH2_CAPTURE_UPVAL
						+ TIM8_T2;		//得到总的高电平的时间
				tempup2 =tempup2 *17/1000;//计算距离&&UltrasonicWave_Distance<85
			
				printf(" %d   \t",TIM8CH2_CAPTURE_UPVAL);
				printf(" %d   \t",TIM8CH2_CAPTURE_DOWNVAL);
				printf("Channel 2 : %d cm\r\n",tempup2);
	
				OLED_ShowNum(0,0,tempup2,3,16,0);
				OLED_Refresh_Gram(0);
				
				TIM8CH2_CAPTURE_STA = 0;		//捕获标志位清零，这一步很重要！
				TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			}
			else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM8CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM8);		//获取上升沿数据
				TIM8CH2_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}
	
		if ((TIM8CH4_CAPTURE_STA & 0X80) == 0) 		//还未成功捕获	
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC4) != RESET) 		//捕获1发生捕获事件
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC4); 		//清除中断标志位
			if (TIM8CH4_CAPTURE_STA & 0X40)		//捕获到一个下降沿
			{
				TIM8CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM8);//记录下此时的定时器计数值
				if (TIM8CH4_CAPTURE_DOWNVAL < TIM8CH4_CAPTURE_UPVAL)
				{/* 如果计数器初始值大于末尾值，说明计数器有溢出 */
					TIM8_T4 = 65535;
				}
				else
				TIM8_T4 = 0;  
				tempup4 = TIM8CH4_CAPTURE_DOWNVAL - TIM8CH4_CAPTURE_UPVAL
						+ TIM8_T4;		//得到总的高电平的时间
				tempup4 =tempup4 *17/1000;//计算距离&&UltrasonicWave_Distance<85
			
				printf(" %d   \t",TIM8CH4_CAPTURE_UPVAL);
				printf(" %d   \t",TIM8CH4_CAPTURE_DOWNVAL);
				printf("Channel 4 : %d cm\r\n",tempup4);
				
				OLED_ShowNum(60,0,tempup4,3,16,0);
				OLED_Refresh_Gram(0);
				
				
				TIM8CH4_CAPTURE_STA = 0;		//捕获标志位清零，这一步很重要！
				TIM_OC4PolarityConfig(TIM8, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			}
			else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM8CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM8);		//获取上升沿数据
				TIM8CH4_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC4PolarityConfig(TIM8, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}
	
	
	

}	
/******************* (C) 1209 Lab *****END OF FILE************/
