#include "timer.h"
#include "led.h"
#include "usart.h"
#include "UltrasonicWave.h"
#include "MP3.h"
//////////////////////////////////////////////////////////////////////////////////	 
	  
extern u8 Flag_us100_0,Flag_us100_1,Flag_us100_2;
extern u32 Us100_Count_0,Us100_Count_1,Us100_Count_2;
u32 distance=0;

void TIM1_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA Configuration: Channel 1,4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


//TIM1 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1  | RCC_APB2Periph_AFIO, ENABLE);	//使能定时器1时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOF, ENABLE);  //使能GPIO外设和模块时钟

	TIM1_GPIO_Config();
	

   //初始化TIM1
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM1  PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	//TIM1_CH1N/CH2N/CH3N只能输出与TIM1_CH1/CH2/CH3相反的波形
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIM1 OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR1上的预装载寄存器
	
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIM1 OC4
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR4上的预装载寄存器
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1
	/* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);//TIM1->BDTR|=0X8000;
	
	TIM_SetCompare1(TIM1,0);
 	TIM_SetCompare4(TIM1,0);	
	
}

void TIM4_PWM_Init(u16 arr,u16 psc)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器4时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, DISABLE); //Timer4部分重映射  TIM4_CH3->PB8    
 
   //设置该引脚为复用输出功能,输出TIM4 CH3的PWM脉冲波形	GPIOB.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	
 //设置该引脚为复用输出功能,输出TIM3 CH4的PWM脉冲波形	GPIOB.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM4 Channel3，4 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC3
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR3上的预装载寄存器
	
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIM4 OC4
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM1在CCR4上的预装载寄存器
	
 
	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
	
	TIM_SetCompare3(TIM4,0);
 	TIM_SetCompare4(TIM4,0);	
}
//占空比不得大于4500*0.98=4410.   
//PB9(A IN1),PA11(A IN2),
//PB8(B IN1),PA8(B IN2)
void Car_Forward(void)
{
 	TIM_SetCompare4(TIM4,0000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,1000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,0000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,1000);//PA8 (B IN2) 
}
void Car_Back(void)
{	
	TIM_SetCompare4(TIM4,1000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,0000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,1000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}
void Car_Pause(void)
{
	TIM_SetCompare4(TIM4,0000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,0000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,0000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}
void Car_Left(void)
{
	TIM_SetCompare4(TIM4,0000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,0000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,0000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,1000);//PA8 (B IN2) 	

}

void Car_Right(void)
{
	TIM_SetCompare4(TIM4,0000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,1000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,0000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}
void Car_Rotation(void)//双轮子打转
{
	TIM_SetCompare4(TIM4,0000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,1000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,1000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}
void Car_Rotation_1(void)//单轮子打转低速
{
	TIM_SetCompare4(TIM4,1000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,0000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,0000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}
void Car_Rotation_2(void)//单轮子打转高速
{
	TIM_SetCompare4(TIM4,3000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,0000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,0000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}


