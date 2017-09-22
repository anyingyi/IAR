/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************
* 文件名称: usart.h
* 描    述: xxxx串口定义.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/1

* 修改历史: 时间       内容           修改者
            2017/6/1 :文件创建       王晶东
*******************************************************************************/
#ifndef _USART_H_
#define _USART_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define USART_MAX		2

#define USART1_RECV_MAX 512
#define USART2_RECV_MAX 100

//串口收发控制
typedef struct CUSART_CTRL_t
{
    USART_TypeDef*  USARTx;
    unsigned char*  recvbuf;  //接收缓存
    unsigned short  recvidx;  //接收缓存索引
    unsigned char   recvbusy; //接收:0x00-空闲,0x01-忙碌
    
    unsigned char*  psendbuff;  //发送数据地址
    unsigned short  sendlen;    //发送长度
    unsigned short  sendedlen;  //已发送长度
    unsigned char   backlen;   //0x00-无需返回,>0x00-需返回
    unsigned char   sendbusy;   //发送:0x00-空闲,0x01-忙碌
}USART_CTRL;

extern USART_CTRL m_USART_CTRL[USART_MAX];

void Usart_Init(void);
void Usart_Send(unsigned char,unsigned char*,unsigned short,unsigned char);

#endif //_USART_H_
/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************/
