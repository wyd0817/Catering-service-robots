#ifndef __UltrasonicWave_H
#define	__UltrasonicWave_H

#include "usart.h"
#include "timer.h"
#include "delay.h"

void UltrasonicWave_Init(void);               //�Գ�����ģ���ʼ��
void UltrasonicWave_StartMeasure(u8 cs);
u32 GET_Distance(u8  US100_x);

#endif /* __UltrasonicWave_H */

