/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 *****************
* 文件名称:    rtc.h
* 描    述:    外部时钟模块
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/6/2

* 修改历史:    时间       内容                  修改者
               2017/6/2: 文件创建              王晶东
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
/*************(C) COPYRIGHT 2010 杭州卡丽智能科技股份有限公司******************/