#include "matrix_key.h" 
#include "usart.h"
#include "led.h"
#include "oled.h"
#include "24l01.h"
#include "SnoyHandle.h"
#include "UltrasonicWave.h"


u8 Digital_Storage=0;
u32 MAIN_PS2_VLAUE=0;	
u8 Automatic_Mode=0,Car_Direction=0;
extern u8 PS2_JOYPAD_ID;
extern u16 TIM8CH2_CAPTURE_UPVAL;
extern u16 TIM8CH2_CAPTURE_DOWNVAL;
extern u32 Distance_T2;
extern u32 tempup2;	//�����ܸߵ�ƽ��ʱ��
extern u8 Flag_Obstruction;
extern u8 TIM8CH2_CAPTURE_STA;
//*****������ʱ��6��ʼ��*********
void TIM6_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Prescaler = 100 - 1;//Ԥ��Ƶϵ��Ϊ36000-1������������ʱ��Ϊ72MHz/36000 = 2kHz
	TIM_TimeBaseStructure.TIM_Period = 7200 - 1;//���ü��������С��ÿ��2000�����Ͳ���һ�������¼�
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//����ʱ�ӷָ� TIM_CKD_DIV1=0x0000,���ָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;//���ü�����ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);//������Ӧ�õ�TIM6��
	TIM_UpdateRequestConfig( TIM6, TIM_UpdateSource_Regular);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//ʹ���ж�
	
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
	TIM_Cmd(TIM6, ENABLE);//ʹ�ܼ�����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//ʹ��PORTDʱ��
}


//stm32������̷�תɨ�跨��PB�ڵĵ�8λ��
//���˾������ɨ�������ڶ�ʱ���жϷ��������
//ÿ10ms����һ���жϷ������
u8 scan_MatrixKey(void)
{
    #define PORT GPIOD->IDR
    u8 column;//��
    u8 row;//��
    u8 tmp;//��ʱ����
		u8 MatrixKey_value = 20;//��ʼֵ����Ϊ0~15 
		static u8 key_count = 0;//�������ж�ɨ��Ĵ���
    
    ///////////IO�ڵ�����/////////////
    //��8λΪ�������
    GPIOD->CRL &= 0X00000000;
    GPIOD->CRL |= 0X33333333;
    //��ֵ:��4λΪ��,�ε�4λΪ��
    GPIOD->ODR &= 0XFF00;
    GPIOD->ODR |= 0X00F0;
    //�ε�4λΪ��������
    GPIOD->CRL &= 0X0000FFFF;
    GPIOD->CRL |= 0X88880000;
  

    tmp = PORT;//����Ҫ
    if (tmp != 0XF0)//����м�����
    {   
        //��ֹ����ʱ,�����������±������
        if (key_count <= 2)
        {
            key_count++;
        }
    }
    //����������������̧���������0 
    else
    {
        key_count = 0;
    }
    //����������2��ɨ������ڰ���״̬
    //����Ϊ����ȷʵ��������
    if (key_count == 2)
    {          
        column = tmp & 0X00F0;//��ȡ�к� 
        
        ///////////IO�ڵ�����/////////////
        //��8λΪ�������
        GPIOD->CRL &= 0X00000000;
        GPIOD->CRL |= 0X33333333;
        //��ת:��4λΪ��,�ε�4λΪ��
        GPIOD->ODR &= 0XFF00;
        GPIOD->ODR |= 0X000F;//��4λΪ��,�ε�4λΪ��
        //��4λΪ��������
        GPIOD->CRL &= 0XFFFF0000;
        GPIOD->CRL |= 0X00008888; 
                            
        row = PORT & 0X000F;//��ȡ�к�                                         
        switch (column | row)//column|rowΪ���������¶�Ӧ�˿ڵı���
        {   
            //������Ӧ��������Ը���������������صļ�ֵ�� 
            case 0XEE: MatrixKey_value = '1'; break;//
            case 0XDE: MatrixKey_value = '2'; break;
            case 0XBE: MatrixKey_value = '3'; break;                
            case 0X7E: MatrixKey_value = 'A'; break;                                              
            case 0XED: MatrixKey_value = '4'; break;
            case 0XDD: MatrixKey_value = '5'; break;
            case 0XBD: MatrixKey_value = '6'; break;
            case 0X7D: MatrixKey_value = 'B'; break;                                 
            case 0XEB: MatrixKey_value = '7'; break;
            case 0XDB: MatrixKey_value = '8'; break;
            case 0XBB: MatrixKey_value = '9'; break;
            case 0X7B: MatrixKey_value = 'C'; break;                                    
            case 0XE7: MatrixKey_value = '*'; break;
            case 0XD7: MatrixKey_value = '0'; break;
            case 0XB7: MatrixKey_value = '#'; break;
            case 0X77: MatrixKey_value = 'D'; break;
            default:   break;     
        }
	
			OLED_ShowString(64,0,&MatrixKey_value,0);
			OLED_Refresh_Gram(0);
			//printf("��ֵ��%c\r\n",MatrixKey_value);
			USART_SendData(USART1, MatrixKey_value);//�򴮿�1��������
			//printf("\r\n");
    } 
    //��û�а���������(������)��ɨ�������0
    //�����´ΰ���ɨ�����
    if ((PORT & 0X00FF) == 0x00F0)
    {
        key_count = 0;  
    }

    return MatrixKey_value;
}

u8 tmp_buf[3];
u8 UltrasonicWave_time=0;
void TIM6_IRQHandler(void) 
{ 

	//����Ƿ�����������¼�
	if(TIM_GetITStatus(TIM6, TIM_IT_Update)== SET)
	{
		u8 Temp_MatrixKey;

		Temp_MatrixKey=scan_MatrixKey();
		if(('0'<=Temp_MatrixKey)&&(Temp_MatrixKey<='9'))							{Digital_Storage=Temp_MatrixKey-'0';printf("%d      ",Digital_Storage);}
		if(Temp_MatrixKey=='A')										{insert(Digital_Storage);printf("%c      ",Digital_Storage);}
		if(NRF24L01_RxPacket(tmp_buf)==0)//һ�����յ���Ϣ,����ʾ����.
		{
			if((tmp_buf[0]=='a')&&(tmp_buf[1]=='b'))	{insert(51);}
			if((tmp_buf[0]=='a')&&(tmp_buf[1]=='c'))	{insert(52);}
			if((tmp_buf[0]=='a')&&(tmp_buf[1]=='d'))	{insert(53);}
			if((tmp_buf[0]=='a')&&(tmp_buf[1]=='e'))	{insert(54);}
			if((tmp_buf[0]=='a')&&(tmp_buf[1]=='f'))	{insert(55);}
			if((tmp_buf[0]=='a')&&(tmp_buf[1]=='g'))	{insert(56);}
	
			OLED_ShowString(0,0,tmp_buf,0);
			OLED_Refresh_Gram(0);	
		 }
		 UltrasonicWave_time++;
		 if(UltrasonicWave_time==20)
		 {
				UltrasonicWave_StartMeasure(1);
		 }
		 else if(UltrasonicWave_time==40)
		 {
				UltrasonicWave_StartMeasure(2);
				UltrasonicWave_time=0;
		 }
		
		 			 
		 
//			MAIN_PS2_VLAUE=PS2_Wireless_JOYPAD_DATA();//���snoy�ֱ�
			if(0x41==PS2_JOYPAD_ID)
			{	
				if(PS2_SLCT)
				{
						Automatic_Mode=1;
				}
				if(PS2_STRT)
				{
						Automatic_Mode=0;
				}
				
				if(PS2_UP)
				{
						Car_Direction=1;
				}
				if(PS2_DOWN)
				{
						Car_Direction=2;
				}
				if(PS2_LEFT)
				{
						Car_Direction=3;
				}
				if(PS2_RGHT)
				{
						Car_Direction=4;
				}
				if(PS2_LEFT_L1)
				{
						Car_Direction=0;
				}
			}
	
}
	TIM_ClearITPendingBit(TIM6 , TIM_FLAG_Update);
}


