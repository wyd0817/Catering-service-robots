#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "UltrasonicWave.h"
#include "oled.h"
#include "steer.h"
#include "24l01.h"
#include "Barcode.h"
#include "MP3.h"
#include "matrix_key.h" 
#include "string.h"
#include "SnoyHandle.h"
#include "lcd.h"

//餐饮服务机器人
//机器人研究所 
//STM32F103ZET6 

#define MOVE_CAR 1


// 时间常数定义
#define ST_0_5S			166
#define ST_1S				2*ST_0_5S
#define ST_1_5S			3*ST_0_5S
#define ST_2S				4*ST_0_5S
#define ST_2_5S			5*ST_0_5S
#define ST_3S				6*ST_0_5S
#define ST_3_5S			7*ST_0_5S
#define ST_4S				8*ST_0_5S


int main(void)
{
	extern u8 BARCODE_0[14];//条形码0
	extern u8 BARCODE_1[14];//条形码1
	extern u8 BARCODE_2[14];//条形码2
	extern u8 BARCODE_3[14];//条形码2

	extern u32 front, rear;
	extern u8 tmp_buf[3];
	extern u32 distance;
	
	extern u8 Flag_Obstruction;
	extern u8 Automatic_Mode,Car_Direction;
	extern u32 MAIN_PS2_VLAUE;
	
	u8 key=key;
	u8 Barcodebuf[14];
	u8 Aim_Table_Number=0,Now_Turn_Number=Now_Turn_Number;
	u8 Flag_Serving_finish=0,Flag_Leave_finish=0;
	u8 Flag_danger=0;
	u8 Flag_take=0;
	u8 Flag_turn=0;
	
	
	extern u8 Now_Table_Number;
	extern u8 Flag_Back_finish;
	extern u8 Flag_Up_arm_finish;
	extern u8 Flag_little_delay;
	extern u8 Flag_little_delay_1;
	extern u8 Flag_Obstruct_raise;
	float temp=temp;	
	
	extern delay_var delay_var_0,delay_var_1,delay_var_2,delay_var_3;
	Init_delay_var(&delay_var_0);
	Init_delay_var(&delay_var_1);
	Init_delay_var(&delay_var_2);
	Init_delay_var(&delay_var_3);

	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 //串口初始化为9600
 	LED_Init();			     //LED端口初始化
	KEY_Init();					//按键初始化
	
	
//	Adc_Init();					//黑线检测传感器初始化
//  TIM1_PWM_Init(4500-1,0);	 //不分频。PWM频率=72000000/4500=16Khz
//	TIM4_PWM_Init(4500-1,0);	 //不分频。PWM频率=72000000/4500=16Khz
	
	UltrasonicWave_Init();//超声波模块初始化
//	LCD_Init();	
	OLED_IO_Init();
	OLED_Init(0);			//初始化OLED 
	OLED_Init(1);			//初始化OLED 
	OLED_Init(2);			//初始化OLED 
//	steer_init();					//舵机初始化s
//	NRF24L01_Init();    	//初始化NRF24L01
//	PS2_Wireless_JOYPAD_Init();
	TIM6_Configuration();		//矩阵键盘初始化		PWM频率=72000000/7200/100=100hz（10ms进入一次中断）
//	Barcode_Init(9600);			//扫描枪串口初始化
	
	
//	while(NRF24L01_Check())	//检查NRF24L01是否在位.	
//	{
//		OLED_ShowString(0,0,"NRF24L01 Error",0);
//		OLED_Refresh_Gram(0);		
// 		delay_ms(200);
//	}	
//	NRF24L01_RX_Mode();	//配置NRF2401为接收模式	
	
	/*******************显示神鹰科技*************************************/
	OLED_ShowChinese_num(0,16,4,0,0);
	OLED_Refresh_Gram(0);
	OLED_ShowString(0,0,"CYSY",1);
	OLED_Refresh_Gram(1);		
	/*******************显示神鹰科技*************************************/

	printf("               草原神鹰工作组                \n");

	tmp_buf[2]=0;//加入字符串结束符		
	
	LCD_Clear(WHITE);
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,50,200,16,16,"CYSY");	
	LCD_ShowChinese_num(30,30,7,0);	

	//马上为你去做菜
	//请点菜
	//请让开，我在工作
	//请慢用

	//	while(!Detection_Location_A())
//											Down_Arm();
//	
//while(1);
	
//	while(!Detection_Location_C())
//											Down_Arm();
	
//while(1);
	
	while(!Detection_Location_B())//调整到正常位置
											Up_Arm();
	PFout(5)=0;
	PFout(6)=0;
	

//	Flag_Up_arm_finish=0;
//	while(Flag_Up_arm_finish!=1)
//	My_delay_event(Up_Arm,Mark_Up_arm_finish,Detection_Location_A);//抬起手臂电机
//	
//	PFout(5)=0;
//	PFout(6)=0;
	
  while(1)
	{	
		
		if(Automatic_Mode!=0)	//切换到手动模式（紧急停止）
		{
			switch(Car_Direction)
			{
				case 0:Car_Pause();		break;
				case 1:Car_Forward();	break;
				case 2:Car_Back();		break;
				case 3:Car_Left();		break;
				case 4:Car_Right();		break;
			}
		}
		else if(Automatic_Mode==0)//切换到自动模式
		{
			if((  front != rear )&&( Automatic_Mode==0 ))//检测队列是否为空（检测是否有餐桌呼叫）
			{
				Aim_Table_Number=read_delete();
				while(( Aim_Table_Number!=0 )&&( Automatic_Mode==0 ))//检测到后厨的条形码或者接收到sony手柄的紧急停止信号跳出循环
				{
					if(Flag_Obstruction==0)//超声波没有检测到障碍物
					{
								if(Flag_Obstruct_raise==1)
								{	PFout(5)=0;PFout(6)=0;Flag_Obstruct_raise=0;}
								
								Flag_danger=0;
								if(Barcode_Receive_Data(Barcodebuf))//扫条形码传感器接收到数据
								{
									//OLED_ShowString(0,0, Barcodebuf);
									//OLED_Refresh_Gram();
									 //printf("%s",Barcodebuf);
										//LED0=!LED0;
									LCD_ShowString(150,150,200,16,16,Barcodebuf);	
									if(Comparison_array(Barcodebuf,BARCODE_0)==0)	 			 {Now_Table_Number=0;	Aim_Table_Number=0;}
									else if(Comparison_array(Barcodebuf,BARCODE_1)==0)	 Now_Table_Number=1;	
									else if(Comparison_array(Barcodebuf,BARCODE_2)==0)	 Now_Table_Number=2;
									else if(Comparison_array(Barcodebuf,BARCODE_3)==0)	 Now_Table_Number=101;									
										
									 
									OLED_ShowNum(32,0,Now_Table_Number,1,16,0);//显示当前路过的桌号
									OLED_Refresh_Gram(0);
								}
								steer_set_all(560,560,880,880,5);
								if((Now_Table_Number-Aim_Table_Number==50)||(Now_Table_Number-Aim_Table_Number==100))	//检测到目标宫位			
								{
									if(Flag_little_delay==0)
												My_delay(&delay_var_0,Car_Rotation_2,Mark_little_delay,ST_4S);//转弯延时
									else
									{
										My_delay_event(Car_Rotation_1,Delete_table_number,Detection_Black);//转弯
										if(Now_Table_Number==0)		Flag_little_delay=0;
									}
								}
								
								else if((Aim_Table_Number==Now_Table_Number)||(Aim_Table_Number-Now_Table_Number==50)) //检测到目标餐桌
								{
									if(DISH==0)//机器人餐盘上面有菜或者菜谱
									{
										Car_Pause();
										if(Flag_Serving_finish==0)
										{
											//语音掀锅盖程序
											Flag_Leave_finish=0;
											if(Flag_Up_arm_finish==0)
												My_delay_event(Down_Arm,Mark_Up_arm_finish,Detection_Location_A);//抬起手臂电机
											else if(Flag_Up_arm_finish==1)//已经完成抬起电机动作
											{
												if(Aim_Table_Number>50)
												{
													if(Flag_take==0)	Uart_SendCMD(0x03 , 0 , 0x02) ;//请点菜
													else if(Flag_take==1)	Uart_SendCMD(0x03 , 0 , 0x01) ;//马上为你去做菜
												}
												Flag_Up_arm_finish=0;//放下手臂标志位设为缺省值
												Flag_Serving_finish=1;
											}
										}
										if(Flag_take==1)//菜谱被放回来
										{
											if(Flag_Back_finish==0)
											{
												My_delay_event(Up_Arm,Mark_Down_arm_finish,Detection_Location_B);//放下手臂电机
											}
											else if(Flag_Back_finish==1)
											My_delay(&delay_var_1,Car_Back,Mark_Back_finish,ST_2S);//后退
											else if(Flag_Back_finish==2)//后退程序执行完毕
											{
												My_delay(&delay_var_2,Car_Rotation_2,Mark_little_delay_1,ST_4S);//转弯
												if(Flag_little_delay_1==1) {Flag_little_delay_1=0;Flag_Back_finish=3;}
											}
											else if(Flag_Back_finish==3)
											{
												My_delay_event(Car_Rotation_1,Delete_table_number,Detection_Black);
												if(Now_Table_Number==0)		{Flag_Back_finish=0;Flag_take=0;}
											}
										}
									}											
									else//机器人餐盘上面的菜被端走了
									{
										if(Flag_Leave_finish==0)
										{
											//语音程序
											Flag_Serving_finish=0;
											if(Aim_Table_Number<50)	Uart_SendCMD(0x03 , 0 , 0x04) ;//请慢用
											Flag_Leave_finish=1;
										}
										
										if(Aim_Table_Number<50)//送菜程序（厨师送菜）
										{
											if(Flag_Back_finish==0)
											{
												My_delay_event(Up_Arm,Mark_Down_arm_finish,Detection_Location_B);//放下手臂电机
											}
											else if(Flag_Back_finish==1)
											My_delay(&delay_var_1,Car_Back,Mark_Back_finish,ST_2S);//后退
											else if(Flag_Back_finish==2)//后退程序执行完毕
											{
												My_delay(&delay_var_2,Car_Rotation_2,Mark_little_delay_1,ST_4S);//转弯
												if(Flag_little_delay_1==1) {Flag_little_delay_1=0;Flag_Back_finish=3;}
											}
											else if(Flag_Back_finish==3)
											{
												My_delay_event(Car_Rotation_1,Delete_table_number,Detection_Black);
												if(Now_Table_Number==0)		{Flag_Back_finish=0;Flag_take=0;}
											}
										
										}	
										else if(Aim_Table_Number>50) Flag_take=1;//等待菜谱放回来
									}
									
								}
								else //循迹模式   三个传感器共八种情况
								{
									if(HOUS==LINE_BLACK)//循迹模式一	  四种情况
									{
										#ifdef MOVE_CAR  
											Car_Forward();
										#endif
								//		printf("A    ");
									}
									else if(LEFT==LINE_BLACK)//循迹模式二	 两种情况
									{
										#ifdef MOVE_CAR  
											Car_Right();
										#endif
										Flag_turn=1;
										printf("B    ");
									}
									else if(RIGH==LINE_BLACK)//循迹模式三	 一种情况
									{
										#ifdef MOVE_CAR  
											Car_Left();
										#endif
										Flag_turn=2;
										printf("C    ");
									}
									else //三个传感器都检测到白色  一种情况
									{
										if(Flag_turn==1) Car_Right();
										else if(Flag_turn==2) Car_Left();
									}
								}
						}
						else if(Flag_Obstruction==1)//超声波检测到障碍物
						{
							Car_Pause();
							if(Flag_danger==0)	
							{
								Uart_SendCMD(0x03 , 0 , 0x03);
								Flag_Obstruct_raise=1;
							}
							Flag_danger=1;
							My_delay_event(Up_Arm,Mark_Down_arm_finish,Detection_Location_C);//抬起手臂电机
						}
					}
				Car_Pause();//回到原位停车
			}
	  }
		//	LED0=!LED0;
	}	 
 }


 

 
 // 音乐模块测试
 
/*		
		delay_ms(1000) ;//延时大概6S
    
    Uart_SendCMD(0x03 , 0 , 0x01) ;//播放第一首
    
    delay_ms(1000) ;//延时大概6S
    
    Uart_SendCMD(0x03 , 0 , 0x02) ;//播放第二首

    delay_ms(1000) ;//延时大概6S

    Uart_SendCMD(0x03 , 0 , 0x04) ;//播放第四首

		while(1) ;
 */

