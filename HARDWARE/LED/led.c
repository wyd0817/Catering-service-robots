#include "led.h"

//��ʼ��PE0��PE2Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2;				 //LED0-->PE.0 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOE.0
 GPIO_SetBits(GPIOE,GPIO_Pin_0|GPIO_Pin_2);						 //PE.0 �����
	
	//�ֱ۵��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PE�˿�ʱ��
//	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;				 //LED0-->PE.0 �˿�����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
// GPIO_Init(GPIOF, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOE.0
// GPIO_SetBits(GPIOF,GPIO_Pin_5|GPIO_Pin_6);						 //PE.0 �����

}
