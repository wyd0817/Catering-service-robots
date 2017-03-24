#include "sys.h"	
#include "led.h"
#include "Barcode.h"	 
#include "delay.h"


#ifdef EN_USART3_RX   	//���ʹ���˽���

const u8 BARCODE_0[14]={'9','7','8','7','0','4','0','1','9','2','1','0','0',0x0D};//������0
const u8 BARCODE_1[14]={'9','7','8','7','1','1','1','0','5','3','2','9','3',0x0D};//������1
const u8 BARCODE_2[14]={'9','7','8','7','1','0','9','0','8','7','4','5','3',0x0D};//������2
const u8 BARCODE_3[14]={'9','7','8','7','1','1','1','1','5','1','3','5','7',0x0D};//������3


//���ջ����� 	
u8 Barcode_RX_BUF[14];  	//���ջ���,���13���ֽ�.
//���յ������ݳ���
u8 Barcode_RX_CNT=0;   		  
  
void USART3_IRQHandler(void)
{
	u8 res;	    
 
 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //���յ�����
	{	 
	 			 
		res =USART_ReceiveData(USART3); 	//��ȡ���յ�������
		if(Barcode_RX_CNT<14)
		{
			Barcode_RX_BUF[Barcode_RX_CNT]=res;		//��¼���յ���ֵ
			Barcode_RX_CNT++;						//������������1 
		} 
	}  											 
} 
#endif										 
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void Barcode_Init(u32 bound)
{  
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PB10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

		RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,ENABLE);//��λ����3
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,DISABLE);//ֹͣ��λ
 
	
 #ifdef EN_USART3_RX		  	//���ʹ���˽���
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ���ݳ���
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;///��żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ

    USART_Init(USART3, &USART_InitStructure); ; //��ʼ������
  
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //ʹ�ܴ���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
 
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 

 #endif

}
//Barcode��ѯ���յ�������
//buf:���ջ����׵�ַ
u8 Barcode_Receive_Data(u8 *buf)
{
	u8 i=0;
	//delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(Barcode_RX_CNT == 14)//���յ�������,�ҽ��������
	{
		for(i=0;i<14;i++)
		{
			buf[i]=Barcode_RX_BUF[i];	
		}		
		Barcode_RX_CNT=0;		//����
		return 1;
	}
	return 0;
}
//����0�������������
//����1���������鲻��
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
