#include "sys.h"		    
#include "MP3.h"	 
#include "delay.h"

static u8 Send_buf[10] = {0} ;

//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void MP3_Init(u32 bound)
{  
 
}

/********************************************************************************************
 - ���������� ���ڷ���һ���ֽ�
 - ����ģ�飺 �ⲿ
 - ����˵����
 - ����˵����
 - ע��	      
********************************************************************************************/
void Uart_PutByte(u8 ch)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,ch);
}

/*****************************************************************************************************
 - ���������� ���ڷ���һ֡����
 - ����ģ�飺 �ڲ� 
 - ����˵���� 
 - ����˵���� 
 - ע����     
*****************************************************************************************************/
void SendCmd(u8 len)
{
    u8 i = 0 ;

    Uart_PutByte(0x7E); //��ʼ
    for(i=0; i<len; i++)//����
    {
		Uart_PutByte(Send_buf[i]) ;
    }
    Uart_PutByte(0xEF) ;//����
}

/********************************************************************************************
 - �������������У��
 - ����ģ�飺
 - ����˵����
 - ����˵����
 - ע��      ��У���˼·����
             ���͵�ָ�ȥ����ʼ�ͽ��������м��6���ֽڽ����ۼӣ����ȡ����
             ���ն˾ͽ����յ���һ֡���ݣ�ȥ����ʼ�ͽ��������м�������ۼӣ��ټ��Ͻ��յ���У��
             �ֽڡ��պ�Ϊ0.�����ʹ������յ���������ȫ��ȷ��
********************************************************************************************/
void DoSum( u8 *Str, u8 len)
{
    u16 xorsum = 0;
    u8 i;

    for(i=0; i<len; i++)
    {
        xorsum  = xorsum + Str[i];
    }
	xorsum     = 0 -xorsum;
	*(Str+i)   = (u8)(xorsum >>8);
	*(Str+i+1) = (u8)(xorsum & 0x00ff);
}


/********************************************************************************************
 - ���������� �������ⷢ������[�������ƺͲ�ѯ]
 - ����ģ�飺 �ⲿ
 - ����˵���� CMD:��ʾ����ָ������ָ�������������ѯ�����ָ��
              feedback:�Ƿ���ҪӦ��[0:����ҪӦ��1:��ҪӦ��]
              data:���͵Ĳ���
 - ����˵����
 - ע��       
********************************************************************************************/
void Uart_SendCMD(u8 CMD ,u8 feedback , u16 dat)
{
    Send_buf[0] = 0xff;    //�����ֽ� 
    Send_buf[1] = 0x06;    //����
    Send_buf[2] = CMD;     //����ָ��
    Send_buf[3] = feedback;//�Ƿ���Ҫ����
    Send_buf[4] = (u8)(dat >> 8);//datah
    Send_buf[5] = (u8)(dat);     //datal
    DoSum(&Send_buf[0],6);        //У��
    SendCmd(8);       //���ʹ�֡����
}


