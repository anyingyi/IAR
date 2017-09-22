/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: nixie.h
* ��    ��: ������ʾ.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/8/8

* �޸���ʷ: ʱ��       ����           �޸���
            2017/8/8: �ļ�����        ������
*******************************************************************************/
#ifndef _NIXIE_H_
#define _NIXIE_H_

#include "tm1650.h"

//�Դ��ַ
#define ADDR_DIG1   0x68
#define ADDR_DIG2   0x6A
#define ADDR_DIG3   0x6C
#define ADDR_DIG4   0x6E

extern void nixie_init();
extern void nixie_display();
extern void nixie_set( unsigned char* pbuf, unsigned char onoff, unsigned short bright, unsigned short blink );
#endif //_NIXIE_H_
/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
