#include "sys.h"	
#include "led.h"
#include "Barcode.h"	 
#include "delay.h"


#ifdef EN_USART3_RX   	//如果使能了接收

const u8 BARCODE_0[14]={'9','7','8','7','0','4','0','1','9','2','1','0','0',0x0D};//条形码0
const u8 BARCODE_1[14]={'9','7','8','7','1','1','1','0','5','3','2','9','3',0x0D};//条形码1
const u8 BARCODE_2[14]={'9','7','8','7','1','0','9','0','8','7','4','5','3',0x0D};//条形码2
const u8 BARCODE_3[14]={'9','7','8','7','1','1','1','1','5','1','3','5','7',0x0D};//条形码3


//接收缓存区 	
u8 Barcode_RX_BUF[14];  	//接收缓冲,最大13个字节.
//接收到的数据长度
u8 Barcode_RX_CNT=0;   		  
  
void USART3_IRQHandler(void)
{
	u8 res;	    
 
 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
	{	 
	 			 
		res =USART_ReceiveData(USART3); 	//读取接收到的数据
		if(Barcode_RX_CNT<14)
		{
			Barcode_RX_BUF[Barcode_RX_CNT]=res;		//记录接收到的值
			Barcode_RX_CNT++;						//接收数据增加1 
		} 
	}  											 
} 
#endif										 
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void Barcode_Init(u32 bound)
{  
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PB10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

		RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,ENABLE);//复位串口3
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,DISABLE);//停止复位
 
	
 #ifdef EN_USART3_RX		  	//如果使能了接收
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

    USART_Init(USART3, &USART_InitStructure); ; //初始化串口
  
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //使能串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART3, ENABLE);                    //使能串口 

 #endif

}
//Barcode查询接收到的数据
//buf:接收缓存首地址
u8 Barcode_Receive_Data(u8 *buf)
{
	u8 i=0;
	//delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(Barcode_RX_CNT == 14)//接收到了数据,且接收完成了
	{
		for(i=0;i<14;i++)
		{
			buf[i]=Barcode_RX_BUF[i];	
		}		
		Barcode_RX_CNT=0;		//清零
		return 1;
	}
	return 0;
}
//返回0：两个数组相等
//返回1：两个数组不等
u8 Comparison_array(u8 *p1,u8 *p2)
{
	u8 temp=0,wyd;
 	for(wyd=0;wyd<9;wyd++)
	{
			if(p1[wyd]==p2[wyd])	temp=0;
			else							{temp=1;break;}
	}
	return temp;
}
