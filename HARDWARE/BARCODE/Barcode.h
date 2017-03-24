#ifndef __BARCODE_H
#define __BARCODE_H			 
#include "sys.h"	 								  



extern u8 Barcode_RX_BUF[14]; 		//接收缓冲,最大64个字节
extern u8 Barcode_RX_CNT;   			//接收到的数据长度
//如果想串口中断接收，请不要注释以下宏定义
#define EN_USART3_RX 	1			//0,不接收;1,接收.

void Barcode_Init(u32 bound);

u8 Barcode_Receive_Data(u8 *buf);
u8 Comparison_array(u8 *p1,u8 *p2);


#endif	   



































