#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//��ȡ����1

#define K2	2
#define K1	1


void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��					    
#endif
/**************************************�Ƽ��÷�******************************/
//	u8 t;	//����ȫ�ֱ��� 	
//	while(1)
//		{
//		t=KEY_Scan(0);		//�õ���ֵ
//	   	if(t)
//		{						   
//			switch(t)
//			{				 
//				case K1:	//���Ʒ�����
//					LED1=!LED1;
//					break;
//				case K2:	//����LED0��ת
//					LED0=!LED0;
//					break;
//			}
//		}
//	}
/**************************************�Ƽ��÷�******************************/


