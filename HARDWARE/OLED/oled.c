#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
u8 OLED_GRAM_0[128][8],OLED_GRAM_1[128][8],OLED_GRAM_2[128][8];	 

//�����Դ浽LCD		 
void OLED_Refresh_Gram(u8 cs)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD,cs);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD,cs);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD,cs);      //������ʾλ�á��иߵ�ַ   
		if(cs == 0)for(n=0;n<128;n++)			OLED_WR_Byte(OLED_GRAM_0[n][i],OLED_DATA,cs); 
		else if(cs == 1)for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM_1[n][i],OLED_DATA,cs); 
		else if(cs == 2)for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM_2[n][i],OLED_DATA,cs); 
	}   
}

//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd,u8 cs)
{	
	u8 i;			  
	if(cmd)
	  OLED_RS_Set();
	else 
	  OLED_RS_Clr();
	
	if(cs==0)				OLED_CS0_Clr();
	else if(cs==1)	OLED_CS1_Clr();
	else if(cs==2)	OLED_CS2_Clr();
	
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				

	if(cs==0)				OLED_CS0_Set();
	else if(cs==1)	OLED_CS1_Set();
	else if(cs==2)	OLED_CS2_Set();	
	OLED_RS_Set();   	  
} 
	  	  
//����OLED��ʾ    
void OLED_Display_On(u8 cs)
{
	OLED_WR_Byte(0X8D,OLED_CMD,cs);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD,cs);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD,cs);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(u8 cs)
{
	OLED_WR_Byte(0X8D,OLED_CMD,cs);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD,cs);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD,cs);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(u8 cs)  
{  
	u8 i,n;  
	
		if(cs == 0)			for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM_0[n][i]=0X00;  
		else if(cs == 1)for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM_1[n][i]=0X00;   
		else if(cs == 2)for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM_2[n][i]=0X00;  
	  
	OLED_Refresh_Gram(cs);//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t,u8 cs)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
			
	if(cs == 0)			{if(t)OLED_GRAM_0[x][pos]|=temp;else OLED_GRAM_0[x][pos]&=~temp;}	
	else if(cs == 1){if(t)OLED_GRAM_1[x][pos]|=temp;else OLED_GRAM_1[x][pos]&=~temp;}	   
	else if(cs == 2){if(t)OLED_GRAM_2[x][pos]|=temp;else OLED_GRAM_2[x][pos]&=~temp;}	
    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot,u8 cs)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot,cs);
	}													    
	OLED_Refresh_Gram(cs);//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode,u8 cs)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=oled_asc2_1206[chr][t];  //����1206����
		else temp=oled_asc2_1608[chr][t];		 //����1608���� 	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode,cs);
			else OLED_DrawPoint(x,y,!mode,cs);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 cs)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1,cs);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1,cs); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//��16����
void OLED_ShowString(u8 x,u8 y,u8 *p,u8 cs)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear(cs);}
        OLED_ShowChar(x,y,*p,16,1,cs);	 
        x+=8;
        p++;
    }  
}	
//��ָ��λ����ʾһ������,48*32
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ  
//size:����Ϊ192 
void OLED_ShowChinese(u8 x,u8 y,u8 chr,u8 mode,u8 cs)
{           
	u8 temp,t,t1;
	u8 y0=y;    
	for(t=0;t<192;t++)
	{   
		temp=oled_chinese_4832[chr][t];                           
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode,cs);
			else OLED_DrawPoint(x,y,!mode,cs);
			temp<<=1;
			y++;
			if((y-y0)==48)
			{
				y=y0;
				x++;
				break;
			}
		}    
	}          
} 
//��ʾһ������
//x,y:�������  
//num:���ֵĸ���
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//��4832����
void OLED_ShowChinese_num(u8 x,u8 y,u8 num,u8 mode,u8 cs)
{		  
#define MAX_CHINESE_POSX 120
#define MAX_CHINESE_POSY 48 
	u8 temp_num = 0;         
	while(temp_num != num)
	{       
		if(x>MAX_CHINESE_POSX){y=x=0;OLED_Clear(cs);}
		if(y>MAX_CHINESE_POSY){y=x=0;OLED_Clear(cs);}
		OLED_ShowChinese(x,y,temp_num,mode,cs);	 
		x+=32;
		temp_num++;
	}  
}
void OLED_IO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,C�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;	 //PC10,PC11�������  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);	  //PC10,PC11
 	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13);	//PC10,PC11 �����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PC12 OUT�������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,GPIO_Pin_12);						 //PC12 OUT  �����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4;				 //PB5,PB4 OUT�������	  RST
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_4);						 //PB5,PB4 OUT  �����
	
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 
}
//��ʼ��SSD1306					    
void OLED_Init(u8 cs)
{ 					  
	OLED_WR_Byte(0xAE,OLED_CMD,cs); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD,cs); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(80,OLED_CMD,cs);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD,cs); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD,cs); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD,cs); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD,cs); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD,cs); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD,cs); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD,cs); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD,cs); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD,cs); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD,cs); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD,cs); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD,cs); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD,cs); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD,cs); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD,cs); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD,cs); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD,cs); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD,cs); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD,cs); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD,cs); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD,cs); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD,cs); //������ʾ	 
	OLED_Clear(cs);
}  
