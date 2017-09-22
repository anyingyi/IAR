/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: tm1650.h
* 描    述: LED和键盘驱动.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/8/8

* 修改历史: 时间       内容           修改者
            2017/8/8: 文件创建        王晶东
*******************************************************************************/
#ifndef TM1650_H
#define TM1650_H

//-- Includes ------------------------------------------------------------------
#include "i2c_hal.h"

#define CMD_MODE        0x48    //模式命令
#define CMD_RD_KEY      0x49    //读按键数据命令

#define CMD_BRIGHT_8    0x00    //8级亮度
#define CMD_BRIGHT_1    0x10    //1级亮度
#define CMD_BRIGHT_2    0x20    //2级亮度
#define CMD_BRIGHT_3    0x30    //3级亮度
#define CMD_BRIGHT_4    0x40    //4级亮度
#define CMD_BRIGHT_5    0x50    //5级亮度
#define CMD_BRIGHT_6    0x60    //6级亮度
#define CMD_BRIGHT_7    0x70    //7级亮度

#define CMD_8_REG       0x00    //8段显示方式
#define CMD_7_REG       0x04    //7段显示方式

#define CMD_OFF_DIS     0x00    //关显示
#define CMD_ON_DIS      0x01    //开显示

//-- Enumerations --------------------------------------------------------------
extern void tm1650_Init( unsigned char id );
extern etError tm1650_WriteConfig(unsigned char id, unsigned char config );
extern etError tm1650_ReadKeycode( unsigned char id, unsigned char *status );
extern etError tm1650_WriteDisplay( unsigned char id, unsigned char addr, unsigned char display );

#endif
