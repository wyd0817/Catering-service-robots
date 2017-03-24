#include "SnoyHandle.h"
#include "led.h"

u8 PS2_JOYPAD_ID=0;

//PS2�����ֱ��ĳ�ʼ��
//��ʼ���ɹ�����0��ʧ�ܷ���1
void PS2_Wireless_JOYPAD_Init(void)
{
	RCC->APB2ENR|=1<<7; //ʹ������PORTFʱ��
	GPIOF->CRL&=0XFFF00000;
	GPIOF->CRL|=0X00033838;   
	GPIOF->ODR|=1<<4;
	GPIOF->ODR|=1<<3;
	GPIOF->ODR|=1<<2;
	GPIOF->ODR|=1<<1;
	GPIOF->ODR|=1<<0;
	PS2_JOYPAD_CLOCK=1;
	PS2_JOYPAD_CMND=1;
	PS2_JOYPAD_ATT=1;
	delay_ms(10);
}

u8 PS2_Wireless_JOYPAD_WriteRead(u8 cmd)
{
	u8 i=0;
	u8 value=0;
	u8 CMD=cmd;
	for(i=0;i<8;i++)
	{
		PS2_JOYPAD_CLOCK=1;
		if(CMD&0x01)PS2_JOYPAD_CMND=1;
		else PS2_JOYPAD_CMND=0;
		CMD>>=1;
		delay_us(10);
		PS2_JOYPAD_CLOCK=0;
		delay_us(15);
		value>>=1;
		if(PS2_JOYPAD_DATA)value|=0x80; //0000 0000
	}
	PS2_JOYPAD_CLOCK=1;
	return value;
}

//���Ӧ����
//��Ӧ���źŷ���0����Ӧ���źŷ���1
u8 PS2_Wireless_JOYPAD_ACK(void)
{
	u8 i=0;
	PS2_JOYPAD_CLOCK=1;
	delay_us(5);
	PS2_JOYPAD_CLOCK=0;
	while(i<100)
	{
		if(0==PS2_JOYPAD_ACK)break;
		i++;
	}
	PS2_JOYPAD_CLOCK=1;
	return i<100?0:1;
}

//��ȡ���ݺ���
//�ɹ�����0��ʧ�ܷ���1
u16 PS2_Wireless_JOYPAD_DATA(void)
{
	u8 table[3]={0};
	//LEFT DOWN RGHT UP STRT X X SLCT       ������ �� Բ�������� R1 L1 R2 L2 
	u16 PS2_VALUE=0XFFFF;

	//PS2_JOYPAD_ID=0;
	
	PS2_JOYPAD_ATT=0;//Ƭѡʹ��
	delay_us(10);

	PS2_Wireless_JOYPAD_WriteRead(PS2_JOYPAD_CMND_START);
	
	if(1==PS2_Wireless_JOYPAD_ACK())return 1;
	//�ֱ�ID�ţ��ҵ��������ֱ�����0X41
	PS2_JOYPAD_ID=PS2_Wireless_JOYPAD_WriteRead(PS2_JOYPAD_CMND_DEMAND);
	
	if(1==PS2_Wireless_JOYPAD_ACK())return 1;
	table[0]=PS2_Wireless_JOYPAD_WriteRead(PS2_JOYPAD_CMND_NOP);//0x5a
	if(1==PS2_Wireless_JOYPAD_ACK())return 1;
	table[1]=PS2_Wireless_JOYPAD_WriteRead(PS2_JOYPAD_CMND_NOP);//data1
	if(1==PS2_Wireless_JOYPAD_ACK())return 1;
	table[2]=PS2_Wireless_JOYPAD_WriteRead(PS2_JOYPAD_CMND_NOP);//data2

	PS2_VALUE=(table[1]<<8)|table[2];

	delay_us(10);
	PS2_JOYPAD_ATT=1;

	return PS2_VALUE;
}
