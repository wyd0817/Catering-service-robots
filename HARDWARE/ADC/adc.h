#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define RIGH  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_12)//��ഫ����
#define LEFT  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_13)//�Ҳഫ����
#define HOUS  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)//�м䴫����

#define DISH  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_14)//�ϲ˴�����

#define LINE_BLACK  1//��ɫ
#define LINE_WHITE  0//��ɫ


void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
u8 Detection_Location_A(void); 
u8 Detection_Location_B(void); 
u8 Detection_Location_C(void); 
 
#endif 
