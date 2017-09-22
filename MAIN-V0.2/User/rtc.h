/************* (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ *****************
* �ļ�����:    rtc.h
* ��    ��:    �ⲿʱ��ģ��
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/6/2

* �޸���ʷ:    ʱ��       ����                  �޸���
               2017/6/2: �ļ�����              ������
*******************************************************************************/
#ifndef _RTC_H_
#define _RTC_H_
//includes
#include "stm32f10x.h"
#include "time.h"

/* Exported macro ------------------------------------------------------------*/
//----export functions----------------------------------------------------------
extern void rtc_init(void);
extern void rtc_time_set(unsigned short year,unsigned char month,unsigned char day,
                         unsigned char  hour,unsigned char minute,unsigned char second);
extern void rtc_time_set1( struct tm mtime );
extern void rtc_time_get( void );
extern void rtc_time_string(unsigned char* timstr,unsigned char timetype,unsigned int);
#endif //_FRAM_H_
/*************(C) COPYRIGHT 2010 ���ݿ������ܿƼ��ɷ����޹�˾******************/