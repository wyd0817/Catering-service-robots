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

//�������������
//�������о��� 
//STM32F103ZET6 

#define MOVE_CAR 1


// ʱ�䳣������
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
	extern u8 BARCODE_0[14];//������0
	extern u8 BARCODE_1[14];//������1
	extern u8 BARCODE_2[14];//������2
	extern u8 BARCODE_3[14];//������2

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

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();					//������ʼ��
	
	
//	Adc_Init();					//���߼�⴫������ʼ��
//  TIM1_PWM_Init(4500-1,0);	 //����Ƶ��PWMƵ��=72000000/4500=16Khz
//	TIM4_PWM_Init(4500-1,0);	 //����Ƶ��PWMƵ��=72000000/4500=16Khz
	
	UltrasonicWave_Init();//������ģ���ʼ��
//	LCD_Init();	
	OLED_IO_Init();
	OLED_Init(0);			//��ʼ��OLED 
	OLED_Init(1);			//��ʼ��OLED 
	OLED_Init(2);			//��ʼ��OLED 
//	steer_init();					//�����ʼ��s
//	NRF24L01_Init();    	//��ʼ��NRF24L01
//	PS2_Wireless_JOYPAD_Init();
	TIM6_Configuration();		//������̳�ʼ��		PWMƵ��=72000000/7200/100=100hz��10ms����һ���жϣ�
//	Barcode_Init(9600);			//ɨ��ǹ���ڳ�ʼ��
	
	
//	while(NRF24L01_Check())	//���NRF24L01�Ƿ���λ.	
//	{
//		OLED_ShowString(0,0,"NRF24L01 Error",0);
//		OLED_Refresh_Gram(0);		
// 		delay_ms(200);
//	}	
//	NRF24L01_RX_Mode();	//����NRF2401Ϊ����ģʽ	
	
	/*******************��ʾ��ӥ�Ƽ�*************************************/
	OLED_ShowChinese_num(0,16,4,0,0);
	OLED_Refresh_Gram(0);
	OLED_ShowString(0,0,"CYSY",1);
	OLED_Refresh_Gram(1);		
	/*******************��ʾ��ӥ�Ƽ�*************************************/

	printf("               ��ԭ��ӥ������                \n");

	tmp_buf[2]=0;//�����ַ���������		
	
	LCD_Clear(WHITE);
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"CYSY");	
	LCD_ShowChinese_num(30,30,7,0);	

	//����Ϊ��ȥ����
	//����
	//���ÿ������ڹ���
	//������

	//	while(!Detection_Location_A())
//											Down_Arm();
//	
//while(1);
	
//	while(!Detection_Location_C())
//											Down_Arm();
	
//while(1);
	
	while(!Detection_Location_B())//����������λ��
											Up_Arm();
	PFout(5)=0;
	PFout(6)=0;
	

//	Flag_Up_arm_finish=0;
//	while(Flag_Up_arm_finish!=1)
//	My_delay_event(Up_Arm,Mark_Up_arm_finish,Detection_Location_A);//̧���ֱ۵��
//	
//	PFout(5)=0;
//	PFout(6)=0;
	
  while(1)
	{	
		
		if(Automatic_Mode!=0)	//�л����ֶ�ģʽ������ֹͣ��
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
		else if(Automatic_Mode==0)//�л����Զ�ģʽ
		{
			if((  front != rear )&&( Automatic_Mode==0 ))//�������Ƿ�Ϊ�գ�����Ƿ��в������У�
			{
				Aim_Table_Number=read_delete();
				while(( Aim_Table_Number!=0 )&&( Automatic_Mode==0 ))//��⵽�������������߽��յ�sony�ֱ��Ľ���ֹͣ�ź�����ѭ��
				{
					if(Flag_Obstruction==0)//������û�м�⵽�ϰ���
					{
								if(Flag_Obstruct_raise==1)
								{	PFout(5)=0;PFout(6)=0;Flag_Obstruct_raise=0;}
								
								Flag_danger=0;
								if(Barcode_Receive_Data(Barcodebuf))//ɨ�����봫�������յ�����
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
										
									 
									OLED_ShowNum(32,0,Now_Table_Number,1,16,0);//��ʾ��ǰ·��������
									OLED_Refresh_Gram(0);
								}
								steer_set_all(560,560,880,880,5);
								if((Now_Table_Number-Aim_Table_Number==50)||(Now_Table_Number-Aim_Table_Number==100))	//��⵽Ŀ�깬λ			
								{
									if(Flag_little_delay==0)
												My_delay(&delay_var_0,Car_Rotation_2,Mark_little_delay,ST_4S);//ת����ʱ
									else
									{
										My_delay_event(Car_Rotation_1,Delete_table_number,Detection_Black);//ת��
										if(Now_Table_Number==0)		Flag_little_delay=0;
									}
								}
								
								else if((Aim_Table_Number==Now_Table_Number)||(Aim_Table_Number-Now_Table_Number==50)) //��⵽Ŀ�����
								{
									if(DISH==0)//�����˲��������в˻��߲���
									{
										Car_Pause();
										if(Flag_Serving_finish==0)
										{
											//�����ƹ��ǳ���
											Flag_Leave_finish=0;
											if(Flag_Up_arm_finish==0)
												My_delay_event(Down_Arm,Mark_Up_arm_finish,Detection_Location_A);//̧���ֱ۵��
											else if(Flag_Up_arm_finish==1)//�Ѿ����̧��������
											{
												if(Aim_Table_Number>50)
												{
													if(Flag_take==0)	Uart_SendCMD(0x03 , 0 , 0x02) ;//����
													else if(Flag_take==1)	Uart_SendCMD(0x03 , 0 , 0x01) ;//����Ϊ��ȥ����
												}
												Flag_Up_arm_finish=0;//�����ֱ۱�־λ��Ϊȱʡֵ
												Flag_Serving_finish=1;
											}
										}
										if(Flag_take==1)//���ױ��Ż���
										{
											if(Flag_Back_finish==0)
											{
												My_delay_event(Up_Arm,Mark_Down_arm_finish,Detection_Location_B);//�����ֱ۵��
											}
											else if(Flag_Back_finish==1)
											My_delay(&delay_var_1,Car_Back,Mark_Back_finish,ST_2S);//����
											else if(Flag_Back_finish==2)//���˳���ִ�����
											{
												My_delay(&delay_var_2,Car_Rotation_2,Mark_little_delay_1,ST_4S);//ת��
												if(Flag_little_delay_1==1) {Flag_little_delay_1=0;Flag_Back_finish=3;}
											}
											else if(Flag_Back_finish==3)
											{
												My_delay_event(Car_Rotation_1,Delete_table_number,Detection_Black);
												if(Now_Table_Number==0)		{Flag_Back_finish=0;Flag_take=0;}
											}
										}
									}											
									else//�����˲�������Ĳ˱�������
									{
										if(Flag_Leave_finish==0)
										{
											//��������
											Flag_Serving_finish=0;
											if(Aim_Table_Number<50)	Uart_SendCMD(0x03 , 0 , 0x04) ;//������
											Flag_Leave_finish=1;
										}
										
										if(Aim_Table_Number<50)//�Ͳ˳��򣨳�ʦ�Ͳˣ�
										{
											if(Flag_Back_finish==0)
											{
												My_delay_event(Up_Arm,Mark_Down_arm_finish,Detection_Location_B);//�����ֱ۵��
											}
											else if(Flag_Back_finish==1)
											My_delay(&delay_var_1,Car_Back,Mark_Back_finish,ST_2S);//����
											else if(Flag_Back_finish==2)//���˳���ִ�����
											{
												My_delay(&delay_var_2,Car_Rotation_2,Mark_little_delay_1,ST_4S);//ת��
												if(Flag_little_delay_1==1) {Flag_little_delay_1=0;Flag_Back_finish=3;}
											}
											else if(Flag_Back_finish==3)
											{
												My_delay_event(Car_Rotation_1,Delete_table_number,Detection_Black);
												if(Now_Table_Number==0)		{Flag_Back_finish=0;Flag_take=0;}
											}
										
										}	
										else if(Aim_Table_Number>50) Flag_take=1;//�ȴ����׷Ż���
									}
									
								}
								else //ѭ��ģʽ   �������������������
								{
									if(HOUS==LINE_BLACK)//ѭ��ģʽһ	  �������
									{
										#ifdef MOVE_CAR  
											Car_Forward();
										#endif
								//		printf("A    ");
									}
									else if(LEFT==LINE_BLACK)//ѭ��ģʽ��	 �������
									{
										#ifdef MOVE_CAR  
											Car_Right();
										#endif
										Flag_turn=1;
										printf("B    ");
									}
									else if(RIGH==LINE_BLACK)//ѭ��ģʽ��	 һ�����
									{
										#ifdef MOVE_CAR  
											Car_Left();
										#endif
										Flag_turn=2;
										printf("C    ");
									}
									else //��������������⵽��ɫ  һ�����
									{
										if(Flag_turn==1) Car_Right();
										else if(Flag_turn==2) Car_Left();
									}
								}
						}
						else if(Flag_Obstruction==1)//��������⵽�ϰ���
						{
							Car_Pause();
							if(Flag_danger==0)	
							{
								Uart_SendCMD(0x03 , 0 , 0x03);
								Flag_Obstruct_raise=1;
							}
							Flag_danger=1;
							My_delay_event(Up_Arm,Mark_Down_arm_finish,Detection_Location_C);//̧���ֱ۵��
						}
					}
				Car_Pause();//�ص�ԭλͣ��
			}
	  }
		//	LED0=!LED0;
	}	 
 }


 

 
 // ����ģ�����
 
/*		
		delay_ms(1000) ;//��ʱ���6S
    
    Uart_SendCMD(0x03 , 0 , 0x01) ;//���ŵ�һ��
    
    delay_ms(1000) ;//��ʱ���6S
    
    Uart_SendCMD(0x03 , 0 , 0x02) ;//���ŵڶ���

    delay_ms(1000) ;//��ʱ���6S

    Uart_SendCMD(0x03 , 0 , 0x04) ;//���ŵ�����

		while(1) ;
 */

