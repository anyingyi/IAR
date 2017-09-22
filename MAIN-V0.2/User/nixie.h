/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: nixie.h
* 描    述: 数码显示.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/8/8

* 修改历史: 时间       内容           修改者
            2017/8/8: 文件创建        王晶东
*******************************************************************************/
#ifndef _NIXIE_H_
#define _NIXIE_H_

#include "tm1650.h"

//显存地址
#define ADDR_DIG1   0x68
#define ADDR_DIG2   0x6A
#define ADDR_DIG3   0x6C
#define ADDR_DIG4   0x6E

extern void nixie_init();
extern void nixie_display();
extern void nixie_set( unsigned char* pbuf, unsigned char onoff, unsigned short bright, unsigned short blink );
#endif //_NIXIE_H_
/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
