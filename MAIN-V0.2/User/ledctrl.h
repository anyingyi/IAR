/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: ledctrl.h
* 描    述: xxxx.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/1

* 修改历史: 时间       内容           修改者
            2017/6/1 :文件创建       王晶东
*******************************************************************************/
#ifndef _LEDCTRL_H_
#define _LEDCTRL_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define MAX_LED			2

//串口闪灭类型
#define LED_OFF     	0x00  //常灭
#define LED_ON      	0x01  //常亮
#define LED_BLINK   	0x02  //闪灭

#define LED_RUN			0
#define LED_RX480	    1


#define	LED_ON_LOW		0
#define	LED_ON_HIGH		1

extern void led_init( void );
extern void led_process(void);
extern void led_on(unsigned char id,unsigned char cmd,
            unsigned short ontime,unsigned short offtime,unsigned char times,unsigned char flag);
extern void led_off(unsigned char id);

#endif //_LEDCTRL_H_
