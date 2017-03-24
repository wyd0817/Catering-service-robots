
/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                �������
--�ļ���    : steer.h
--�汾      : V1.0
--����      : ������
--��������  : 2015/7/28
--�޸�����  : 2015/7/28
--ע��      : 1) ������Ƶ�PWM�� 0.5ms~2.5ms, 20ms����, 50Hz
                 ��� steering engine
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
--�޸���ʷ��
*************************************************************************************************/
#include "steer.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "adc.h"

// �����ʱ������

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

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
}		
		
u32 SYS_CNT=0;//����ʱ��
//1ms
void TIM3_IRQHandler(void)
{
	if(TIM3->SR & TIM_FLAG_Update)//�����ж�
	{
		SYS_CNT++;//����ʱ��
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
//	AFIO->MAPR|=3<<8;      //ȫ����ӳ��

	TIM2->ARR = arr;
	TIM2->PSC = psc;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = (MAX_PWM-MIN_PWM)*0.5+MIN_PWM;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//�������:TIM����Ƚϼ��Ը�

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
					steer_set_pwm(SE[i].angle, i);//�������
				}
			}
			else
			{
				SE[i].sta=ENGINE_STOP;
			}
		}
	}
}

//���ö������PWM��
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


//-------------------------�û����ú���---------------------
/*
����һ������Ƕ��ٶ�
angle : MIN_PWM  MAX_PWM
sel   : 0~3  4�����ѡ��
speed : x ms  �ƶ��ٶȣ����ִ��ƶ��������1ms
*/
void steer_set_sel(s16 angle, char sel, s16 speed)
{
	if (SE[sel].sta==ENGINE_STOP)
	{
		if (angle <= MIN_PWM)
			angle = MIN_PWM;
		if (angle >= MAX_PWM)
			angle = MAX_PWM;
		if ((SE[sel].angle-angle) >= 0)//�ж϶�����з���
		{
			SE[sel].dir = 1;
		}
		else
		{
			SE[sel].dir = 0;
		}
		SE[sel].setangle = angle;
		SE[sel].speed = speed;//�ٶ�
		SE[sel].sta=ENGINE_RUN;
	}
}

/*
�������ж���ĽǶȺ��ٶ�
ang0: ���0�ĽǶ�, ȡֵ��Χ180~900
...
speed : ���ж�����ƶ��ٶ�
*/
void steer_set_all(s16 ang0, s16 ang1, s16 ang2, s16 ang3, s16 speed)
{
	steer_set_sel(ang0, 0, speed);
	steer_set_sel(ang1, 1, speed);
	steer_set_sel(ang2, 2, speed);
	steer_set_sel(ang3, 3, speed);
}

/*
������ƽ�
A0 A1 A2 A3
*/
void steer_init(void)
{
	u8 i;
	TIM3_Int_Init(99, 719);//1ms ��ʱ�ж�
	TIM2_PWM_Init(7199, 199);//200��Ƶ��PWMƵ��=��72000000/200��/7200=50Khz     20ms 180~900
	for(i=0; i<4; i++)//��ʼ��PWMΪһ�룬Ҳ���Ƕ��ת��90��λ��
		steer_set_pwm((MAX_PWM-MIN_PWM)*0.5+MIN_PWM, i);
}

void Init_delay_var(delay_var *delay_var)
{
	delay_var->SoftTimerCounter = 0;
	delay_var->SoftTimerOver = 0;
	delay_var->Allow_SoftTimer = 0;
	delay_var->Allow_Timer = 0;
}
		
/*********************************��ʱ����**********************************************/
void My_delay(delay_var *delay_var,void (*delay_before)(void),void (*delay_after)(void),u32 delay_time)
{
	if(delay_var->Allow_Timer==0){delay_var->Allow_SoftTimer=1;delay_var->Allow_Timer=1;delay_var->SoftTimerCounter = delay_time;}
	else{
				if(delay_var->SoftTimerOver==0)
				{
					/*****************��Ҫ��ʱ�Ĵ���***********************/
					(*delay_before)();
					/*****************��Ҫ��ʱ�Ĵ���***********************/
				}
				else if(delay_var->SoftTimerOver==1)
				{
					/*****************��ʱ����Ҫִ�еĴ���******************/
					(*delay_after)();
					/*****************��ʱ����Ҫִ�еĴ���******************/
					delay_var->Allow_Timer=0;
					
					delay_var->SoftTimerOver=0;
					
				}
			}
	}
	/*********************************��ʱ����**********************************************/

/********************************�ȴ��¼�����*********************************************/
void My_delay_event(void (*delay_before)(void),void (*delay_after)(void),u8 (*delay_event)(void))
{
	if((*delay_event)()==0)
	{
		/*****************��Ҫ��ʱ�Ĵ���***********************/
				(*delay_before)();
		/*****************��Ҫ��ʱ�Ĵ���***********************/
	}
	else if((*delay_event)()==1)
	{
		/*****************��ʱ����Ҫִ�еĴ���******************/
		(*delay_after)();
		/*****************��ʱ����Ҫִ�еĴ���******************/
		
	}
}
	/********************************�ȴ��¼�����*********************************************/
	
void Delete_table_number(void)
{
	Now_Table_Number=0;
}

void Mark_Back_finish(void)
{
	Flag_Back_finish=2;
}
//̧����
void Up_Arm(void)
{
	PFout(5)=1;
	PFout(6)=0;
}
//���µ��
void Down_Arm(void)
{
	PFout(5)=0;
	PFout(6)=1;
}
//ֹ̧ͣ����
void Mark_Up_arm_finish(void)
{
	PFout(5)=0;
	PFout(6)=0;
	Flag_Up_arm_finish=1;
}

//ֹͣ���µ��
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



