#ifndef __BARCODE_H
#define __BARCODE_H			 
#include "sys.h"	 								  



extern u8 Barcode_RX_BUF[14]; 		//���ջ���,���64���ֽ�
extern u8 Barcode_RX_CNT;   			//���յ������ݳ���
//����봮���жϽ��գ��벻Ҫע�����º궨��
#define EN_USART3_RX 	1			//0,������;1,����.

void Barcode_Init(u32 bound);

u8 Barcode_Receive_Data(u8 *buf);
u8 Comparison_array(u8 *p1,u8 *p2);


#endif	   



































