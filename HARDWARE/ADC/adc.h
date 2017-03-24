#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//ADC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define RIGH  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_12)//左侧传感器
#define LEFT  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_13)//右侧传感器
#define HOUS  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)//中间传感器

#define DISH  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_14)//上菜传感器

#define LINE_BLACK  1//黑色
#define LINE_WHITE  0//白色


void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
u8 Detection_Location_A(void); 
u8 Detection_Location_B(void); 
u8 Detection_Location_C(void); 
 
#endif 
