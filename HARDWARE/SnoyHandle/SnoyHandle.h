#ifndef __SNOYHANDLE_H
#define __SNOYHANDLE_H	 

#include "sys.h"
#include "delay.h"
#include "usart.h"

#define PS2_JOYPAD_DATA  PFin(0)
#define PS2_JOYPAD_CMND PFout(1)
#define PS2_JOYPAD_ATT PFout(3)
#define PS2_JOYPAD_CLOCK PFout(4)
#define PS2_JOYPAD_ACK PFin(2)

#define PS2_JOYPAD_CMND_START  0X01  //起始命令 
#define PS2_JOYPAD_CMND_DEMAND 0X42 //数据请求
#define PS2_JOYPAD_CMND_NOP   0X00   //idle

/***********************数字手柄返回值0x41*****************************************************/
#define PS2_LEFT     ((~MAIN_PS2_VLAUE)&0x8000)//左
#define PS2_DOWN     ((~MAIN_PS2_VLAUE)&0x4000)//下
#define PS2_RGHT     ((~MAIN_PS2_VLAUE)&0x2000)//右
#define PS2_UP       ((~MAIN_PS2_VLAUE)&0x1000)//上
#define PS2_STRT     ((~MAIN_PS2_VLAUE)&0x0800)//开始
#define PS2_SLCT     ((~MAIN_PS2_VLAUE)&0x0100)//选择
#define PS2_SQUARE   ((~MAIN_PS2_VLAUE)&0x0080)//正方形
#define PS2_X_MARK   ((~MAIN_PS2_VLAUE)&0x0040)//叉
#define PS2_CIRCLE   ((~MAIN_PS2_VLAUE)&0x0020)//圆形
#define PS2_TRIANGLE ((~MAIN_PS2_VLAUE)&0x0010)//三角形 
#define PS2_RGHT_R1  ((~MAIN_PS2_VLAUE)&0x0008)//右1
#define PS2_LEFT_L1  ((~MAIN_PS2_VLAUE)&0x0004)//左1
#define PS2_RGHT_R2  ((~MAIN_PS2_VLAUE)&0x0002)//右2
#define PS2_LEFT_L2  ((~MAIN_PS2_VLAUE)&0x0001)//左2

/***********************模拟手柄返回值0x73*****************************************************/
/***********************模拟手柄特有的两个摇杆值***********************************************/
#define PS2_ROCKER_RL (PS2_TABLE[3]<60)
#define PS2_ROCKER_RR (PS2_TABLE[3]>200)
#define PS2_ROCKER_RU (PS2_TABLE[4]<60)
#define PS2_ROCKER_RD (PS2_TABLE[4]>200)

#define PS2_ROCKER_LL (PS2_TABLE[5]<60)
#define PS2_ROCKER_LR (PS2_TABLE[5]>200)
#define PS2_ROCKER_LU (PS2_TABLE[6]<60)
#define PS2_ROCKER_LD (PS2_TABLE[6]>200)


void PS2_Wireless_JOYPAD_Init(void);
u16 PS2_Wireless_JOYPAD_DATA(void);

#endif
