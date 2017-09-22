/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: ledctrl.h
* ��    ��: xxxx.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/1

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/1 :�ļ�����       ������
*******************************************************************************/
#ifndef _LEDCTRL_H_
#define _LEDCTRL_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define MAX_LED			2

//������������
#define LED_OFF     	0x00  //����
#define LED_ON      	0x01  //����
#define LED_BLINK   	0x02  //����

#define LED_RUN			0
#define LED_RX480	    1


#define	LED_ON_LOW		0
#define	LED_ON_HIGH		1

extern void led_init( void );
extern void led_process(void);
extern void led_on(unsigned char id,unsigned char cmd,
            unsigned short ontime,unsigned short offtime,unsigned char times,unsigned char flag);
extern void led_off(unsigned char id);

#endif //_LEDCTRL_H_
