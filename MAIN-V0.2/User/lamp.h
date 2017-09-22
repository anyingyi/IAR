/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: lamp.h
* ��    ��: ������ʾ.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/8/23

* �޸���ʷ: ʱ��       ����           �޸���
            2017/8/23: �ļ�����        ������
*******************************************************************************/
#ifndef _LAMP_H_
#define _LAMP_H_

#include "tm1650.h"

//
#define LAMP_OFF     	0x00  //����
#define LAMP_ON      	0x01  //����
#define LAMP_BLINK   	0x02  //����

//���������
#define LAMP_MAX        6
#define LAMP_ONOFF      0
#define LAMP_AUTO       1
#define LAMP_TIME       2
#define LAMP_LEVEL1     3
#define LAMP_LEVEL2     4
#define LAMP_LEVEL3     5
#define LAMP_ALL        0xff
//ÿ���ƶ�Ӧ��bitλ
#define BIT_LAMP1       0x80
#define BIT_LAMP2       0x40
#define BIT_LAMP3       0x20
#define BIT_LAMP4       0x04
#define BIT_LAMP5       0x08
#define BIT_LAMP6       0x10

/* Exported functions ------------------------------------------------------- */
extern void lamp_init(void);
extern void lamp_on( unsigned char id, unsigned char cmd,
                     unsigned short ontime, unsigned short offtime, 
                     unsigned char times, unsigned char flag );
extern void lamp_off( unsigned char id );
extern void lamp_process(void);
#endif //_LAMP_H_
/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
