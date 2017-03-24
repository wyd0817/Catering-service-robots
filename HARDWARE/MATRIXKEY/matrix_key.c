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
extern u32 tempup2;	//捕获总高电平的时间
extern u8 Flag_Obstruction;
extern u8 TIM8CH2_CAPTURE_STA;
//*****基本定时器6初始化*********
void TIM6_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Prescaler = 100 - 1;//预分频系数为36000-1，这样计数器时钟为72MHz/36000 = 2kHz
	TIM_TimeBaseStructure.TIM_Period = 7200 - 1;//设置计数溢出大小，每计2000个数就产生一个更新事件
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割 TIM_CKD_DIV1=0x0000,不分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;//设置计数器模式为向上计数模式
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);//将配置应用到TIM6中
	TIM_UpdateRequestConfig( TIM6, TIM_UpdateSource_Regular);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//使能中断
	
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
	TIM_Cmd(TIM6, ENABLE);//使能计数器
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能PORTD时钟
}


//stm32矩阵键盘翻转扫描法（PB口的低8位）
//将此矩阵键盘扫描程序放在定时器中断服务程序里
//每10ms进入一次中断服务程序
u8 scan_MatrixKey(void)
{
    #define PORT GPIOD->IDR
    u8 column;//列
    u8 row;//行
    u8 tmp;//临时变量
		u8 MatrixKey_value = 20;//初始值不能为0~15 
		static u8 key_count = 0;//按键被中断扫描的次数
    
    ///////////IO口的配置/////////////
    //低8位为推挽输出
    GPIOD->CRL &= 0X00000000;
    GPIOD->CRL |= 0X33333333;
    //初值:低4位为低,次低4位为高
    GPIOD->ODR &= 0XFF00;
    GPIOD->ODR |= 0X00F0;
    //次低4位为上拉输入
    GPIOD->CRL &= 0X0000FFFF;
    GPIOD->CRL |= 0X88880000;
  

    tmp = PORT;//必须要
    if (tmp != 0XF0)//如果有键按下
    {   
        //防止长按时,持续自增导致变量溢出
        if (key_count <= 2)
        {
            key_count++;
        }
    }
    //若产生抖动按键被抬起则计数清0 
    else
    {
        key_count = 0;
    }
    //若按键连续2次扫描均处于按下状态
    //则认为按键确实被按下了
    if (key_count == 2)
    {          
        column = tmp & 0X00F0;//获取列号 
        
        ///////////IO口的配置/////////////
        //低8位为推挽输出
        GPIOD->CRL &= 0X00000000;
        GPIOD->CRL |= 0X33333333;
        //翻转:低4位为高,次低4位为低
        GPIOD->ODR &= 0XFF00;
        GPIOD->ODR |= 0X000F;//低4位为高,次低4位为低
        //低4位为上拉输入
        GPIOD->CRL &= 0XFFFF0000;
        GPIOD->CRL |= 0X00008888; 
                            
        row = PORT & 0X000F;//获取行号                                         
        switch (column | row)//column|row为按键被按下对应端口的编码
        {   
            //按键对应的码表（可以根据需求调整欲返回的键值） 
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
			//printf("键值：%c\r\n",MatrixKey_value);
			USART_SendData(USART1, MatrixKey_value);//向串口1发送数据
			//printf("\r\n");
    } 
    //若没有按键被按下(已松手)则扫描次数清0
    //方便下次按下扫描计数
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

	//检测是否发生溢出更新事件
	if(TIM_GetITStatus(TIM6, TIM_IT_Update)== SET)
	{
		u8 Temp_MatrixKey;

		Temp_MatrixKey=scan_MatrixKey();
		if(('0'<=Temp_MatrixKey)&&(Temp_MatrixKey<='9'))							{Digital_Storage=Temp_MatrixKey-'0';printf("%d      ",Digital_Storage);}
		if(Temp_MatrixKey=='A')										{insert(Digital_Storage);printf("%c      ",Digital_Storage);}
		if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
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
		
		 			 
		 
//			MAIN_PS2_VLAUE=PS2_Wireless_JOYPAD_DATA();//检测snoy手柄
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


