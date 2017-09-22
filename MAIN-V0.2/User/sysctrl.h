/****************** (C) COPYRIGHT 2010 ����·�Ϲ���С�� ***********************
* �ļ�����:    fanctrl.h
* ��    ��:    �������ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2015/11/18

* �޸���ʷ:    ʱ��       ����                  �޸���
               2015/11/18: �ļ�����              ������
*******************************************************************************/
#ifndef _SYSCTRL_H_
#define _SYSCTRL_H_
//includes
#include "data.h"


#define	MODE_LEVEL0			0x00	//��λ��
#define	MODE_LEVEL1			0x01	//��λ1
#define	MODE_LEVEL2			0x02	//��λ2
#define	MODE_LEVEL3			0x03	//��λ��3

#define MODE_M				0x00    //�ֶ�
#define MODE_TIME			0x10	//��ʱ
#define	MODE_INTEL			0x20	//���ܹ�
#define	MODE_REMOTE			0x40	//Զ������
#define	MODE_ADJUST			0x80	//΢��

#define	TIME_DELAY			5		//��ʱ��ʱ

#define	WAVE_C_PM25			3		//����PM2.5���Ʋ���ֵ

#define NORMAL              0       //����ģʽ
#define STUDY               1       //ѧϰģʽ

#define TIME_STEP           1800    //��ʱ����30����һ��
#define TIME_HOUR           3600    //��ʱ����30����һ��
#define TIME_MAX            28800   //���ʱ8Сʱ
#define TIME_ACTIVED        5       //��ʱ��ʱ��ʾʱ��5s

/* Exported macro ------------------------------------------------------------*/
extern void display_init( void );
extern void display_process( void );


extern void mode_process( void );
extern void mode_set( unsigned short status );
extern void time_process( void );
extern void blink_process( void );
extern void func_process( void );
extern void pvd_init( void );
extern void check_process( void );
extern void mode_set( unsigned short status );
extern void display_enable( unsigned char status, unsigned char lamponoff );
#endif //_SYSCTRL_H_
/*********(C) COPYRIGHT 2010 ����·�Ϲ���С��*****End of this file*************/