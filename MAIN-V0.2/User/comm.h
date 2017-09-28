/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: comm.h
* 描    述: xxxxmodbus通讯定义.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/1

* 修改历史: 时间       内容           修改者
            2017/6/1 :文件创建       王晶东
*******************************************************************************/
#ifndef _COMM_H_
#define _COMM_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usart.h"

#define COMM_MAX 		2
#define ADDR_CAST     0xff      // 为广播地址
#define ADDR_ADD1     0x01      // 为地址1

#define WIFI_UP    		0         //对上通信
#define PM25_DOWN     	1         //PM2.5传感器通信

typedef int  (*usartfunc)(unsigned char*,unsigned char);//函数指针

typedef struct Mod_Chk_t
{
    USART_CTRL*     puc;
    unsigned char   addr;                   //目标或本机地址
    unsigned char   status;                 //目标或本机地址
    unsigned short  index;                  //接收缓存解析索引
	unsigned short  outtime;                //接收超时時間
    __IO short      timeout;                //接收超时
    usartfunc       mreqfunc;               //请求函数函数解析
    usartfunc       mrspfunc;               //响应函数解析
    usartfunc       mdelayfunc;             //超时函数函数解析
}Mod_Chk;

extern Mod_Chk mMod_Chk[COMM_MAX];

extern void Comm_Init(void);
int modbus_check(unsigned char id,unsigned short min,unsigned short max);
extern unsigned short calc_sum(unsigned char *inData, unsigned short inLen);
extern void Com_Send(unsigned char id,unsigned char* psendbuff,
					 unsigned short sendlen,unsigned short backlen,
					 unsigned short timeout);
extern int ComPM25_check(unsigned char id, unsigned short packetlen);
extern int ComNet_check(unsigned char id, unsigned short min);

#endif //_COMM_H_
/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************/
