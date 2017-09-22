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

//�������ģʽ����
#define FAN_OFF             0       //�ػ�
#define FAN_LEVEL1          1       //һ��
#define FAN_LEVEL2          2       //����
#define FAN_LEVEL3          3       //����
#define FAN_TIME            4       //��ʱ
#define FAN_AUTO            5       //����
#define FAN_ADJUST          0x80    //΢��
#define FAN_ERR             0xff    //����

//���ռ�ձȲ���
#define FAN_PWM_OFF         100
#define FAN_PWM_MAX         90
#define FAN_PWM_MIN         10
#define FAN_PWM_LEVEL1      80
#define FAN_PWM_LEVEL2      60
#define FAN_PWM_LEVEL3      40
#define FAN_PWM_START       70
#define FAN_PWM_DWN         2

//���״̬
#define FAN_STATUS_OFF      0
#define FAN_STATUS_ON       1
#define FAN_STATUS_START    2

//������ʱʱ��
#define FAN_START_TIME      5       //5s


//LED����
typedef struct CFAN_t
{
    unsigned char   status;       //FAN״̬:0x00-��,0x01-��,0x02-������
    unsigned char   flag;         //���±�־
    unsigned char   cmd;          //FAN����:0-�رգ�1-һ����2-������3-������4-���ƣ�5-���ܣ�6-΢��
    unsigned char   duty;         //FAN����ռ�ձ�
    unsigned short  time;         //FAN������ʱ
}CFAN;

extern CFAN m_fan;

/* Exported functions ------------------------------------------------------- */
extern void fan_init(void);
extern void fan_ctrl( unsigned short status );
extern void fan_totaltime(void);
extern void fan_process(void);

#endif /* __FANCTRL_H */

/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************/
