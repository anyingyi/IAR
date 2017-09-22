/*******************************************************************************
* File   :  fan.h
* Author :  allen   
* Date   :  2017.8.25
* Version:  V01
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FANCTRL_H
#define __FANCTRL_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

//风机运行模式命令
#define FAN_OFF             0       //关机
#define FAN_LEVEL1          1       //一档
#define FAN_LEVEL2          2       //二档
#define FAN_LEVEL3          3       //三档
#define FAN_TIME            4       //定时
#define FAN_AUTO            5       //智能
#define FAN_ADJUST          0x80    //微调
#define FAN_ERR             0xff    //错误

//风机占空比参数
#define FAN_PWM_OFF         100
#define FAN_PWM_MAX         90
#define FAN_PWM_MIN         10
#define FAN_PWM_LEVEL1      80
#define FAN_PWM_LEVEL2      60
#define FAN_PWM_LEVEL3      40
#define FAN_PWM_START       70
#define FAN_PWM_DWN         2

//风机状态
#define FAN_STATUS_OFF      0
#define FAN_STATUS_ON       1
#define FAN_STATUS_START    2

//启动延时时间
#define FAN_START_TIME      5       //5s


//LED控制
typedef struct CFAN_t
{
    unsigned char   status;       //FAN状态:0x00-关,0x01-开,0x02-启动中
    unsigned char   flag;         //更新标志
    unsigned char   cmd;          //FAN命令:0-关闭，1-一档，2-二档，3-三档，4-定制，5-智能，6-微调
    unsigned char   duty;         //FAN控制占空比
    unsigned short  time;         //FAN启动延时
}CFAN;

extern CFAN m_fan;

/* Exported functions ------------------------------------------------------- */
extern void fan_init(void);
extern void fan_ctrl( unsigned short status );
extern void fan_totaltime(void);
extern void fan_process(void);

#endif /* __FANCTRL_H */

/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************/
