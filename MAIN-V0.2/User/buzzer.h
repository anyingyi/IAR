/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************
* �ļ�����:    buzzer.h
* ��    ��:    ������ģ��.hͷ�ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/8/16

* �޸���ʷ:    ʱ��       ����                  �޸���
               2017/8/16: �ļ�����              ������
*******************************************************************************/
#ifndef _BUZZER_H_
#define _BUZZER_H_
//includes
#include "stm32f10x.h"

/* Exported macro ------------------------------------------------------------*/
#define MAX_BUZZER  		1

//����
#define MUSICS_MAX          10
#define MUSIC_ON            0   //������
#define MUSIC_OFF           1   //�ػ���
#define MUSIC_AUTO          2   //�Զ���ť��
#define MUSIC_TIME          3   //��ʱ��ť��
#define MUSIC_LEVEL1        4   //��λ1��ť��
#define MUSIC_LEVEL2        5   //��λ2��ť��
#define MUSIC_LEVEL3        6   //��λ3��ť��
#define MUSIC_ADJUST        7   //��λ3��ť��
#define MUSIC_LONG          8   //������
#define MUSIC_STUDY         9   //ѧϰ��

//����������
typedef struct CBUZZER_TIME_t
{
    unsigned char   onoff;          //BUZZER״̬:0x00-����,0x01-��
    unsigned char   musicid;        //BUZZER�����
	
    unsigned char   fonoff;         //BUZZER״̬:0x00-����,0x01-��
    unsigned char   findex;         //BUZZERƵ������
    unsigned short  ftime;          //BUZZERƵ�ʳ���ʱ��
	
    unsigned char   vonoff;         //BUZZER��ѹ���ر�־
    unsigned char   vindex;         //BUZZER��ѹ����
    unsigned short  vtime;          //BUZZER��ѹ����ʱ��  
}CBUZZER_TIME;

extern CBUZZER_TIME mBUZZER_TIME[MAX_BUZZER];
//----export functions----------------------------------------------------------
extern void buzzer_init(void);
extern void buzzer_freqturn( unsigned char id );
extern void buzzer_freqoff( unsigned char id );
extern void buzzer_process( void );
extern void buzzer_on(unsigned char id, unsigned char musicid );

#endif //_BUZZER_H_

/*************(C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾******************/