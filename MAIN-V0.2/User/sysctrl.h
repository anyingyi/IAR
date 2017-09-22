/****************** (C) COPYRIGHT 2010 开发路上工作小组 ***********************
* 文件名称:    fanctrl.h
* 描    述:    待机ｈ文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2015/11/18

* 修改历史:    时间       内容                  修改者
               2015/11/18: 文件创建              王晶东
*******************************************************************************/
#ifndef _SYSCTRL_H_
#define _SYSCTRL_H_
//includes
#include "data.h"


#define	MODE_LEVEL0			0x00	//档位关
#define	MODE_LEVEL1			0x01	//档位1
#define	MODE_LEVEL2			0x02	//档位2
#define	MODE_LEVEL3			0x03	//档位关3

#define MODE_M				0x00    //手动
#define MODE_TIME			0x10	//定时
#define	MODE_INTEL			0x20	//智能关
#define	MODE_REMOTE			0x40	//远程锁定
#define	MODE_ADJUST			0x80	//微调

#define	TIME_DELAY			5		//定时延时

#define	WAVE_C_PM25			3		//智能PM2.5控制波动值

#define NORMAL              0       //正常模式
#define STUDY               1       //学习模式

#define TIME_STEP           1800    //定时步伐30分钟一步
#define TIME_HOUR           3600    //定时步伐30分钟一步
#define TIME_MAX            28800   //最长定时8小时
#define TIME_ACTIVED        5       //定时延时显示时间5s

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
/*********(C) COPYRIGHT 2010 开发路上工作小组*****End of this file*************/