/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: touch.h
* ��    ��: ����.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/3

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/3: �ļ�����        ������
*******************************************************************************/
#ifndef _TOUCH_H_
#define _TOUCH_H_

#define T_None          0X0000
#define T_K1            0x0001
#define T_K2            0x0002
#define T_K3            0x0004
#define T_K4            0x0020
#define T_K5            0x0010
#define T_K6            0x0008
#define T_L_X           0x0100
#define T_L_K1          (T_L_X|T_K1)
#define T_L_K2          (T_L_X|T_K2)
#define T_L_K4K5K6      (T_L_X|T_K4|T_K5|T_K6)
#define T_AUTO_ESC      0Xffff


extern unsigned short m_key;
extern void touch_init( void );
extern void touch_process( void );

#endif //_TOUCH_H_
/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
