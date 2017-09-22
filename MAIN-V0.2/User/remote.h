/*******************************************************************************
* File   :  remote.h
* Author :  allen   
* Date   :  2017.8.10
* Version:  V01
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __REMOTE_H
#define __REMOTE_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define	REMOTE_NONE         0x00
#define	REMOTE_ONOFF        0x01			
#define	REMOTE_AUTO         0x02	
#define	REMOTE_TIME         0x03
#define	REMOTE_LEVEL1       0x04
#define	REMOTE_LEVEL2       0x05
#define	REMOTE_LEVEL3       0x06
#define	REMOTE_ADJUST       0x07


#define STUDY_TIME          60     //ѧϰ����ʱ1����
//remote ����
typedef struct CREMOTE_t
{
    unsigned char   status;         //״̬��1:���յ�ͬ���źţ�2:���յ�����һ֡����
    unsigned char   busy;           //����״̬��0���� >0æ
    unsigned char   receivedbits;   //�ѽ���bit��
    unsigned int    receive;        //�����뻺��
    unsigned int    keycode;        //������
    unsigned char   keyid;          //�������ܺ�
    unsigned char   studystatus;    //ң��ѧϰ���ܱ�־ 0:����״̬��1-ѧϰ״̬��2-ѧϰ��������״̬
    unsigned char   studysucc;        //ң��ѧϰid��
    unsigned char   studycounts;    //ѧϰ������
    unsigned char   studytimes;     //ѧϰ����ʱ
}CREMOTE;

extern CREMOTE m_remote;


/* Exported functions ------------------------------------------------------- */
extern void remote_init( void );
extern void remote_process( void );
extern void remote_studyen( unsigned char en );
extern void remote_study( void );

#endif /* __REMOTE_H */

/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************/
