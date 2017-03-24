
/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                舵机控制
--文件名    : steer.h
--版本      : V1.0
--作者      : 王永东
--创建日期  : 2015/7/28
--修改日期  : 2015/7/28
--注明      : 1) 舵机控制的PWM是 0.5ms~2.5ms, 20ms周期, 50Hz
                 舵机 steering engine
             _______ yn_______
            |        |        |
          xp|________|________|xn
            |        |        |
            |________|________|
                     yp
                        | MIN_PWM(yn)
                        |
                        |
   (xp)900 x<-----------|----------- 180(xn)
                        |
                        |
                       \|/y MAX_PWM(yp)
**************************************************************************************************
--修改历史：
*************************************************************************************************/
#include "steer.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "adc.h"

// 软件定时器定义

u8 Now_Table_Number=0;
u8 Flag_Back_finish=0;
u8 Flag_Up_arm_finish=0;
u8 Flag_little_delay=0;
u8 Flag_little_delay_1=0;
u8 Flag_Obstruct_raise=0;
delay_var delay_var_0,delay_var_1,delay_var_2,delay_var_3;

ENGINE SE[4]={
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
};

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}		
		
u32 SYS_CNT=0;//调度时钟
//1ms
void TIM3_IRQHandler(void)
{
	if(TIM3->SR & TIM_FLAG_Update)//更新中断
	{
		SYS_CNT++;//调度时钟
		steer_exe_int();
	}
	TIM3->SR &= ~TIM_FLAG_Update;
	
	if(delay_var_0.Allow_SoftTimer==1)
	{
		if(delay_var_0.SoftTimerCounter)
		{
			delay_var_0.SoftTimerCounter--;
			if(!delay_var_0.SoftTimerCounter) {delay_var_0.SoftTimerOver = 1;delay_var_0.Allow_SoftTimer=0;}
		}
	}	
	
	if(delay_var_1.Allow_SoftTimer==1)
	{
		if(delay_var_1.SoftTimerCounter)
		{
			delay_var_1.SoftTimerCounter--;
			if(!delay_var_1.SoftTimerCounter) {delay_var_1.SoftTimerOver = 1;delay_var_1.Allow_SoftTimer=0;}
		}
	}	
	
	if(delay_var_2.Allow_SoftTimer==1)
	{
		if(delay_var_2.SoftTimerCounter)
		{
			delay_var_2.SoftTimerCounter--;
			if(!delay_var_2.SoftTimerCounter) {delay_var_2.SoftTimerOver = 1;delay_var_2.Allow_SoftTimer=0;}
		}
	}	
	
	if(delay_var_3.Allow_SoftTimer==1)
	{
		if(delay_var_3.SoftTimerCounter)
		{
			delay_var_3.SoftTimerCounter--;
			if(!delay_var_3.SoftTimerCounter) {delay_var_3.SoftTimerOver = 1;delay_var_3.Allow_SoftTimer=0;}
		}
	}	
	
}

void TIM2_PWM_Init(u16 arr, u16 psc)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC->APB1ENR |= RCC_APB1Periph_TIM2;
  RCC->APB2ENR |= RCC_APB2Periph_GPIOA;
	GPIOA->CRL &= 0xFFFF0000;
	GPIOA->CRL |= 0x0000BBBB;
    GPIOA->ODR |= 0X000F;
//	AFIO->MAPR|=3<<8;      //全部重映射

	TIM2->ARR = arr;
	TIM2->PSC = psc;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = (MAX_PWM-MIN_PWM)*0.5+MIN_PWM;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性:TIM输出比较极性高

	//channel 1
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM2, TIM_Channel_1, TIM_CCx_Enable);

	//channel 2
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM2, TIM_Channel_2, TIM_CCx_Enable);

	//channel 3
	//TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	//TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	//TIM_CCxCmd(TIM2, TIM_Channel_3, TIM_CCx_Enable);

	//channel 4
	//TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	//TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	//TIM_CCxCmd(TIM2, TIM_Channel_4, TIM_CCx_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}


//1ms
void steer_exe_int(void)
{
	static u16 cnt[4]={0,0,0,0};
	u8 i;

	for (i=0; i<4; i++)
	{
		if (SE[i].sta==ENGINE_RUN)
		{
			if (SE[i].angle != SE[i].setangle)
			{
				cnt[i]++;
				if (cnt[i] >= SE[i].speed)
				{
					cnt[i]=0;

					switch(SE[i].dir)
					{
					case 0: SE[i].angle++;break;
					case 1: SE[i].angle--;break;
					}
					steer_set_pwm(SE[i].angle, i);//输出脉宽波
				}
			}
			else
			{
				SE[i].sta=ENGINE_STOP;
			}
		}
	}
}

//设置舵机控制PWM波
void steer_set_pwm(s16 pwm, char sel)
{
	if(sel==0)
		TIM2->CCR1=pwm;
	else if(sel==1)
		TIM2->CCR2=pwm;
	else if(sel==2)
		TIM2->CCR3=pwm;
	else if(sel==3)
		TIM2->CCR4=pwm;
}


//-------------------------用户调用函数---------------------
/*
设置一个舵机角度速度
angle : MIN_PWM  MAX_PWM
sel   : 0~3  4个舵机选择
speed : x ms  移动速度，数字大，移动慢，最快1ms
*/
void steer_set_sel(s16 angle, char sel, s16 speed)
{
	if (SE[sel].sta==ENGINE_STOP)
	{
		if (angle <= MIN_PWM)
			angle = MIN_PWM;
		if (angle >= MAX_PWM)
			angle = MAX_PWM;
		if ((SE[sel].angle-angle) >= 0)//判断舵机运行方向
		{
			SE[sel].dir = 1;
		}
		else
		{
			SE[sel].dir = 0;
		}
		SE[sel].setangle = angle;
		SE[sel].speed = speed;//速度
		SE[sel].sta=ENGINE_RUN;
	}
}

/*
设置所有舵机的角度和速度
ang0: 舵机0的角度, 取值范围180~900
...
speed : 所有舵机的移动速度
*/
void steer_set_all(s16 ang0, s16 ang1, s16 ang2, s16 ang3, s16 speed)
{
	steer_set_sel(ang0, 0, speed);
	steer_set_sel(ang1, 1, speed);
	steer_set_sel(ang2, 2, speed);
	steer_set_sel(ang3, 3, speed);
}

/*
舵机控制脚
A0 A1 A2 A3
*/
void steer_init(void)
{
	u8 i;
	TIM3_Int_Init(99, 719);//1ms 定时中断
	TIM2_PWM_Init(7199, 199);//200分频。PWM频率=（72000000/200）/7200=50Khz     20ms 180~900
	for(i=0; i<4; i++)//初始化PWM为一半，也就是舵机转到90度位置
		steer_set_pwm((MAX_PWM-MIN_PWM)*0.5+MIN_PWM, i);
}

void Init_delay_var(delay_var *delay_var)
{
	delay_var->SoftTimerCounter = 0;
	delay_var->SoftTimerOver = 0;
	delay_var->Allow_SoftTimer = 0;
	delay_var->Allow_Timer = 0;
}
		
/*********************************延时代码**********************************************/
void My_delay(delay_var *delay_var,void (*delay_before)(void),void (*delay_after)(void),u32 delay_time)
{
	if(delay_var->Allow_Timer==0){delay_var->Allow_SoftTimer=1;delay_var->Allow_Timer=1;delay_var->SoftTimerCounter = delay_time;}
	else{
				if(delay_var->SoftTimerOver==0)
				{
					/*****************需要延时的代码***********************/
					(*delay_before)();
					/*****************需要延时的代码***********************/
				}
				else if(delay_var->SoftTimerOver==1)
				{
					/*****************延时后需要执行的代码******************/
					(*delay_after)();
					/*****************延时后需要执行的代码******************/
					delay_var->Allow_Timer=0;
					
					delay_var->SoftTimerOver=0;
					
				}
			}
	}
	/*********************************延时代码**********************************************/

/********************************等待事件代码*********************************************/
void My_delay_event(void (*delay_before)(void),void (*delay_after)(void),u8 (*delay_event)(void))
{
	if((*delay_event)()==0)
	{
		/*****************需要延时的代码***********************/
				(*delay_before)();
		/*****************需要延时的代码***********************/
	}
	else if((*delay_event)()==1)
	{
		/*****************延时后需要执行的代码******************/
		(*delay_after)();
		/*****************延时后需要执行的代码******************/
		
	}
}
	/********************************等待事件代码*********************************************/
	
void Delete_table_number(void)
{
	Now_Table_Number=0;
}

void Mark_Back_finish(void)
{
	Flag_Back_finish=2;
}
//抬起电机
void Up_Arm(void)
{
	PFout(5)=1;
	PFout(6)=0;
}
//放下电机
void Down_Arm(void)
{
	PFout(5)=0;
	PFout(6)=1;
}
//停止抬起电机
void Mark_Up_arm_finish(void)
{
	PFout(5)=0;
	PFout(6)=0;
	Flag_Up_arm_finish=1;
}

//停止放下电机
void Mark_Down_arm_finish(void)
{
	PFout(5)=0;
	PFout(6)=0;
	Flag_Back_finish=1;
}
void Mark_Obstruct_raise_finish(void)
{
	PFout(5)=0;
	PFout(6)=0;
	Flag_Obstruct_raise=0;
}


void Mark_little_delay(void)
{
	Flag_little_delay=1;
}
void Mark_little_delay_1(void)
{
	Flag_little_delay_1=1;
}
u8 Detection_Black(void)
{
	if(HOUS==LINE_BLACK) return 1;
	else 								 return 0;
}



