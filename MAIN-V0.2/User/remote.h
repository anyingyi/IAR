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


#define STUDY_TIME          60     //学习倒计时1分钟
//remote 数据
typedef struct CREMOTE_t
{
    unsigned char   status;         //状态：1:接收到同步信号，2:接收到完整一帧数据
    unsigned char   busy;           //接收状态；0空闲 >0忙
    unsigned char   receivedbits;   //已接收bit数
    unsigned int    receive;        //按键码缓存
    unsigned int    keycode;        //按键码
    unsigned char   keyid;          //按键功能号
    unsigned char   studystatus;    //遥控学习功能标志 0:正常状态，1-学习状态，2-学习结束保存状态
    unsigned char   studysucc;        //遥控学习id号
    unsigned char   studycounts;    //学习计数器
    unsigned char   studytimes;     //学习倒计时
}CREMOTE;

extern CREMOTE m_remote;


/* Exported functions ------------------------------------------------------- */
extern void remote_init( void );
extern void remote_process( void );
extern void remote_studyen( unsigned char en );
extern void remote_study( void );

#endif /* __REMOTE_H */

/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************/
