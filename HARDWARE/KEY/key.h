#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//读取按键1

#define K2	2
#define K1	1


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
#endif
/**************************************推荐用法******************************/
//	u8 t;	//定义全局变量 	
//	while(1)
//		{
//		t=KEY_Scan(0);		//得到键值
//	   	if(t)
//		{						   
//			switch(t)
//			{				 
//				case K1:	//控制蜂鸣器
//					LED1=!LED1;
//					break;
//				case K2:	//控制LED0翻转
//					LED0=!LED0;
//					break;
//			}
//		}
//	}
/**************************************推荐用法******************************/


