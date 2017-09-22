/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************
* �ļ�����: usart.h
* ��    ��: xxxx���ڶ���.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/1

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/1 :�ļ�����       ������
*******************************************************************************/
#ifndef _USART_H_
#define _USART_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define USART_MAX		2

#define USART1_RECV_MAX 512
#define USART2_RECV_MAX 100

//�����շ�����
typedef struct CUSART_CTRL_t
{
    USART_TypeDef*  USARTx;
    unsigned char*  recvbuf;  //���ջ���
    unsigned short  recvidx;  //���ջ�������
    unsigned char   recvbusy; //����:0x00-����,0x01-æµ
    
    unsigned char*  psendbuff;  //�������ݵ�ַ
    unsigned short  sendlen;    //���ͳ���
    unsigned short  sendedlen;  //�ѷ��ͳ���
    unsigned char   backlen;   //0x00-���践��,>0x00-�践��
    unsigned char   sendbusy;   //����:0x00-����,0x01-æµ
}USART_CTRL;

extern USART_CTRL m_USART_CTRL[USART_MAX];

void Usart_Init(void);
void Usart_Send(unsigned char,unsigned char*,unsigned short,unsigned char);

#endif //_USART_H_
/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************/
