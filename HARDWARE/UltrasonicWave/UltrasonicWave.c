/******************** (C) 1209 Lab **************************
 * �ļ���  : UltrasonicWave.c
 * ����    �����������ģ�飬UltrasonicWave_Configuration��������
             ��ʼ������ģ�飬UltrasonicWave_StartMeasure��������
			 ������࣬������õ�����ͨ������1��ӡ����         
 * ʵ��ƽ̨��Mini STM32������  STM32F103ZET6
 * Ӳ��1���ӣ�------------------
 *          | PC6  - TRIG      |
 *          | PC7  - ECHO      |
 *           ------------------
 * Ӳ��2���ӣ�------------------
 *          | PC8  - TRIG      |
 *          | PC9  - ECHO      |
 *           ------------------
 * ��汾  ��ST3.5.0
 *
 * ����    �������� 
*********************************************************************************/

#include "UltrasonicWave.h"
#include "usart.h"
#include "timer.h"
#include "delay.h"
#include "led.h"

u8 Flag_Obstruction=0;

u8 TIM8CH2_CAPTURE_STA = 0;	//ͨ��2���벶���־������λ�������־����6λ�������־		
u16 TIM8CH2_CAPTURE_UPVAL;
u16 TIM8CH2_CAPTURE_DOWNVAL;
u32 tempup2 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 TIM8_T2;

u8 TIM8CH4_CAPTURE_STA = 0;	//ͨ��3���벶���־������λ�������־����6λ�������־		
u16 TIM8CH4_CAPTURE_UPVAL;
u16 TIM8CH4_CAPTURE_DOWNVAL;
u32 tempup4 = 0;	//�����ܸߵ�ƽ��ʱ��
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
 * ��������UltrasonicWave_Configuration
 * ����  ��������ģ��ĳ�ʼ��
 * ����  ����
 * ���  ����	
 */
 
 TIM_ICInitTypeDef  TIM8_ICInitStructure;
 
void UltrasonicWave_Init(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure;	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//�ر�jtag
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
    
  GPIO_InitStructure.GPIO_Pin = TRIG_PIN_0;					 //PC6��TRIG
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //��Ϊ�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(TRIG_PORT_0, &GPIO_InitStructure);	                 //��ʼ������GPIO 

  GPIO_InitStructure.GPIO_Pin = ECHO_PIN_0;				     //PC7��ECH0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //��Ϊ����
  GPIO_Init(ECHO_PORT_0,&GPIO_InitStructure);						 //��ʼ��GPIOC
	
	
	 GPIO_InitStructure.GPIO_Pin = TRIG_PIN_1;					 //PC8��TRIG
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //��Ϊ�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(TRIG_PORT_1, &GPIO_InitStructure);	                 //��ʼ������GPIO 

  GPIO_InitStructure.GPIO_Pin = ECHO_PIN_1;				     //PC9��ECH0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //��Ϊ����
  GPIO_Init(ECHO_PORT_1,&GPIO_InitStructure);						 //��ʼ��GPIOC
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//ʹ��TIM8ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //ʹ��GPIOCʱ��
	
	
	//��ʼ����ʱ��8 TIM8	 
	TIM_TimeBaseStructure.TIM_Period = 0xffff; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//Ԥ��Ƶ��     ��1Mhz��Ƶ�ʼ��� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM8���벶�����
	TIM8_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM8_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM8_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM8_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM8_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM8, &TIM8_ICInitStructure);
	
	TIM8_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM8_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM8_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM8_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM8_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM8, &TIM8_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;  //TIM8�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM8,TIM_IT_CC2|TIM_IT_CC4,ENABLE);//��������ж� ,����CC1IE�����ж�	
	//TIM_ITConfig(TIM8,TIM_IT_CC2,ENABLE);//��������ж� ,����CC1IE�����ж�	
	
  TIM_Cmd(TIM8,ENABLE ); 	//ʹ�ܶ�ʱ��8
	
}

/*
 * ��������UltrasonicWave_StartMeasure
 * ����  ����ʼ��࣬����һ��>10us�����壬Ȼ��������صĸߵ�ƽʱ��
 * ����  ����
 * ���  ����	
 */
void UltrasonicWave_StartMeasure(u8 cs)
{
  if(cs==1)
	{
		GPIO_SetBits(TRIG_PORT_0,TRIG_PIN_0);  //��>10US�ĸߵ�ƽ�TRIG_PORT,TRIG_PIN��������define����?
		delay_us(20);		                      //��ʱ20US
		GPIO_ResetBits(TRIG_PORT_0,TRIG_PIN_0);
	}
	else if(cs==2)
	{
		GPIO_SetBits(TRIG_PORT_1,TRIG_PIN_1);  //��>10US�ĸߵ�ƽ�TRIG_PORT,TRIG_PIN��������define����?
		delay_us(20);		                      //��ʱ20US
		GPIO_ResetBits(TRIG_PORT_1,TRIG_PIN_1);
	}
}

		
//��ʱ��8�жϷ������,��CH2��Ϊ������ת����ͨ�� ��PC7�˿�
void TIM8_CC_IRQHandler(void)
{
		if ((TIM8CH2_CAPTURE_STA & 0X80) == 0) 		//��δ�ɹ�����	
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET) 		//����1���������¼�
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC2); 		//����жϱ�־λ
			if (TIM8CH2_CAPTURE_STA & 0X40)		//����һ���½���
			{
				TIM8CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM8);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM8CH2_CAPTURE_DOWNVAL < TIM8CH2_CAPTURE_UPVAL)
				{/* �����������ʼֵ����ĩβֵ��˵������������� */
					TIM8_T2 = 65535;
				}
				else
				TIM8_T2 = 0;  
				tempup2 = TIM8CH2_CAPTURE_DOWNVAL - TIM8CH2_CAPTURE_UPVAL
						+ TIM8_T2;		//�õ��ܵĸߵ�ƽ��ʱ��
				tempup2 =tempup2 *17/1000;//�������&&UltrasonicWave_Distance<85
			
				printf(" %d   \t",TIM8CH2_CAPTURE_UPVAL);
				printf(" %d   \t",TIM8CH2_CAPTURE_DOWNVAL);
				printf("Channel 2 : %d cm\r\n",tempup2);
	
				OLED_ShowNum(0,0,tempup2,3,16,0);
				OLED_Refresh_Gram(0);
				
				TIM8CH2_CAPTURE_STA = 0;		//�����־λ���㣬��һ������Ҫ��
				TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			}
			else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM8CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM8);		//��ȡ����������
				TIM8CH2_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}
	
		if ((TIM8CH4_CAPTURE_STA & 0X80) == 0) 		//��δ�ɹ�����	
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC4) != RESET) 		//����1���������¼�
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC4); 		//����жϱ�־λ
			if (TIM8CH4_CAPTURE_STA & 0X40)		//����һ���½���
			{
				TIM8CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM8);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM8CH4_CAPTURE_DOWNVAL < TIM8CH4_CAPTURE_UPVAL)
				{/* �����������ʼֵ����ĩβֵ��˵������������� */
					TIM8_T4 = 65535;
				}
				else
				TIM8_T4 = 0;  
				tempup4 = TIM8CH4_CAPTURE_DOWNVAL - TIM8CH4_CAPTURE_UPVAL
						+ TIM8_T4;		//�õ��ܵĸߵ�ƽ��ʱ��
				tempup4 =tempup4 *17/1000;//�������&&UltrasonicWave_Distance<85
			
				printf(" %d   \t",TIM8CH4_CAPTURE_UPVAL);
				printf(" %d   \t",TIM8CH4_CAPTURE_DOWNVAL);
				printf("Channel 4 : %d cm\r\n",tempup4);
				
				OLED_ShowNum(60,0,tempup4,3,16,0);
				OLED_Refresh_Gram(0);
				
				
				TIM8CH4_CAPTURE_STA = 0;		//�����־λ���㣬��һ������Ҫ��
				TIM_OC4PolarityConfig(TIM8, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			}
			else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM8CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM8);		//��ȡ����������
				TIM8CH4_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC4PolarityConfig(TIM8, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}
	
	
	

}	
/******************* (C) 1209 Lab *****END OF FILE************/
