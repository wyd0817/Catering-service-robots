#ifndef _STEER_H_
#define _STEER_H_
#include "sys.h"

//舵机的脉宽比较最大最小值
#define MIN_PWM 180
#define MAX_PWM 900
#define ANGLE_RESOLUTION ((MAX_PWM-MIN_PWM)/180.0)


typedef struct delay_var
{
	u32 SoftTimerCounter;//计数变量
	u8 SoftTimerOver;//是否计数完毕
	u8 Allow_SoftTimer;//允许定时器计数
	u8 Allow_Timer;//计数的时间
} delay_var;


typedef struct 
{
	char sta;//舵机状态
	char dir;
	s16 angle;//当前的角度
	s16 setangle;//设置的角度
	s16 speed;//舵机运动速度
}ENGINE;
extern ENGINE SE[];
//舵机状态
#define ENGINE_RUN     1//舵机正在移动
#define ENGINE_STOP    0

void steer_exe_int(void);
void steer_set_pwm(s16 pwm, char sel);

void steer_init(void);
void steer_set_sel(s16 angx, char sel, s16 speed);
void steer_set_all(s16 ang0, s16 ang1, s16 ang2, s16 ang3, s16 speed);

void Init_delay_var(delay_var *delay_var);
void My_delay(delay_var *delay_var,void (*delay_before)(void),void (*delay_after)(void),u32 delay_time);
void My_delay_event(void (*delay_before)(void),void (*delay_after)(void),u8 (*delay_event)(void));
void Delete_table_number(void);
void Mark_Back_finish(void);
void Mark_little_delay(void);
void Mark_little_delay_1(void);
void Up_Arm(void);
void Down_Arm(void);
void Mark_Up_arm_finish(void);
void Mark_Down_arm_finish(void);
u8 Detection_Black(void);
void Car_Rotation(void);
void Car_Rotation_1(void);
void Car_Rotation_2(void);

extern u32 SYS_CNT;
#endif
