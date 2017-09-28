/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: comm.h
* ��    ��: xxxxmodbusͨѶ����.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/1

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/1 :�ļ�����       ������
*******************************************************************************/
#ifndef _COMM_H_
#define _COMM_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usart.h"

#define COMM_MAX 		2
#define ADDR_CAST     0xff      // Ϊ�㲥��ַ
#define ADDR_ADD1     0x01      // Ϊ��ַ1

#define WIFI_UP    		0         //����ͨ��
#define PM25_DOWN     	1         //PM2.5������ͨ��

typedef int  (*usartfunc)(unsigned char*,unsigned char);//����ָ��

typedef struct Mod_Chk_t
{
    USART_CTRL*     puc;
    unsigned char   addr;                   //Ŀ��򱾻���ַ
    unsigned char   status;                 //Ŀ��򱾻���ַ
    unsigned short  index;                  //���ջ����������
	unsigned short  outtime;                //���ճ�ʱ�r�g
    __IO short      timeout;                //���ճ�ʱ
    usartfunc       mreqfunc;               //��������������
    usartfunc       mrspfunc;               //��Ӧ��������
    usartfunc       mdelayfunc;             //��ʱ������������
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
/************* (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************/
