/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: lamp.h
* 描    述: 数码显示.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/8/23

* 修改历史: 时间       内容           修改者
            2017/8/23: 文件创建        王晶东
*******************************************************************************/
#ifndef _LAMP_H_
#define _LAMP_H_

#include "tm1650.h"

//
#define LAMP_OFF     	0x00  //常灭
#define LAMP_ON      	0x01  //常亮
#define LAMP_BLINK   	0x02  //闪灭

//按键背光灯
#define LAMP_MAX        6
#define LAMP_ONOFF      0
#define LAMP_AUTO       1
#define LAMP_TIME       2
#define LAMP_LEVEL1     3
#define LAMP_LEVEL2     4
#define LAMP_LEVEL3     5
#define LAMP_ALL        0xff
//每个灯对应的bit位
#define BIT_LAMP1       0x80
#define BIT_LAMP2       0x40
#define BIT_LAMP3       0x20
#define BIT_LAMP4       0x04
#define BIT_LAMP5       0x08
#define BIT_LAMP6       0x10

/* Exported functions ------------------------------------------------------- */
extern void lamp_init(void);
extern void lamp_on( unsigned char id, unsigned char cmd,
                     unsigned short ontime, unsigned short offtime, 
                     unsigned char times, unsigned char flag );
extern void lamp_off( unsigned char id );
extern void lamp_process(void);
#endif //_LAMP_H_
/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
