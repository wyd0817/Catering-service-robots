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


//TIM1 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1  | RCC_APB2Periph_AFIO, ENABLE);	//ʹ�ܶ�ʱ��1ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOF, ENABLE);  //ʹ��GPIO�����ģ��ʱ��

	TIM1_GPIO_Config();
	

   //��ʼ��TIM1
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM1  PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	//TIM1_CH1N/CH2N/CH3Nֻ�������TIM1_CH1/CH2/CH3�෴�Ĳ���
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����ָ���Ĳ�����ʼ������TIM1 OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���
	
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //����ָ���Ĳ�����ʼ������TIM1 OC4
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR4�ϵ�Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
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
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��4ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, DISABLE); //Timer4������ӳ��  TIM4_CH3->PB8    
 
   //���ø�����Ϊ�����������,���TIM4 CH3��PWM���岨��	GPIOB.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
	
 //���ø�����Ϊ�����������,���TIM3 CH4��PWM���岨��	GPIOB.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM4 Channel3��4 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC3
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR3�ϵ�Ԥװ�ؼĴ���
	
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //����ָ���Ĳ�����ʼ������TIM4 OC4
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR4�ϵ�Ԥװ�ؼĴ���
	
 
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
	
	TIM_SetCompare3(TIM4,0);
 	TIM_SetCompare4(TIM4,0);	
}
//ռ�ձȲ��ô���4500*0.98=4410.   
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
void Car_Rotation(void)//˫���Ӵ�ת
{
	TIM_SetCompare4(TIM4,0000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,1000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,1000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}
void Car_Rotation_1(void)//�����Ӵ�ת����
{
	TIM_SetCompare4(TIM4,1000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,0000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,0000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}
void Car_Rotation_2(void)//�����Ӵ�ת����
{
	TIM_SetCompare4(TIM4,3000);//PB9 (A IN1)
	TIM_SetCompare4(TIM1,0000);//PA11(A IN2)
	TIM_SetCompare3(TIM4,0000);//PB8 (B IN1)
	TIM_SetCompare1(TIM1,0000);//PA8 (B IN2) 
}


