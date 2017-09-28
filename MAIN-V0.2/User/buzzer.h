/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************
* 文件名称:    buzzer.h
* 描    述:    蜂鸣器模块.h头文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/8/16

* 修改历史:    时间       内容                  修改者
               2017/8/16: 文件创建              王晶东
*******************************************************************************/
#ifndef _BUZZER_H_
#define _BUZZER_H_
//includes
#include "stm32f10x.h"

/* Exported macro ------------------------------------------------------------*/
#define MAX_BUZZER  		1

//音调
#define MUSICS_MAX          10
#define MUSIC_ON            0   //开机音
#define MUSIC_OFF           1   //关机音
#define MUSIC_AUTO          2   //自动按钮音
#define MUSIC_TIME          3   //定时按钮音
#define MUSIC_LEVEL1        4   //档位1按钮音
#define MUSIC_LEVEL2        5   //档位2按钮音
#define MUSIC_LEVEL3        6   //档位3按钮音
#define MUSIC_ADJUST        7   //档位3按钮音
#define MUSIC_LONG          8   //长嘀音
#define MUSIC_STUDY         9   //学习音

//蜂鸣器控制
typedef struct CBUZZER_TIME_t
{
    unsigned char   onoff;          //BUZZER状态:0x00-不响,0x01-响
    unsigned char   musicid;        //BUZZER谱序号
	
    unsigned char   fonoff;         //BUZZER状态:0x00-不响,0x01-响
    unsigned char   findex;         //BUZZER频率索引
    unsigned short  ftime;          //BUZZER频率持续时间
	
    unsigned char   vonoff;         //BUZZER电压开关标志
    unsigned char   vindex;         //BUZZER电压索引
    unsigned short  vtime;          //BUZZER电压控制时间  
}CBUZZER_TIME;

extern CBUZZER_TIME mBUZZER_TIME[MAX_BUZZER];
//----export functions----------------------------------------------------------
extern void buzzer_init(void);
extern void buzzer_freqturn( unsigned char id );
extern void buzzer_freqoff( unsigned char id );
extern void buzzer_process( void );
extern void buzzer_on(unsigned char id, unsigned char musicid );

#endif //_BUZZER_H_

/*************(C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司******************/