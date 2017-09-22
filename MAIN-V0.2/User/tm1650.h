/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: tm1650.h
* ��    ��: LED�ͼ�������.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/8/8

* �޸���ʷ: ʱ��       ����           �޸���
            2017/8/8: �ļ�����        ������
*******************************************************************************/
#ifndef TM1650_H
#define TM1650_H

//-- Includes ------------------------------------------------------------------
#include "i2c_hal.h"

#define CMD_MODE        0x48    //ģʽ����
#define CMD_RD_KEY      0x49    //��������������

#define CMD_BRIGHT_8    0x00    //8������
#define CMD_BRIGHT_1    0x10    //1������
#define CMD_BRIGHT_2    0x20    //2������
#define CMD_BRIGHT_3    0x30    //3������
#define CMD_BRIGHT_4    0x40    //4������
#define CMD_BRIGHT_5    0x50    //5������
#define CMD_BRIGHT_6    0x60    //6������
#define CMD_BRIGHT_7    0x70    //7������

#define CMD_8_REG       0x00    //8����ʾ��ʽ
#define CMD_7_REG       0x04    //7����ʾ��ʽ

#define CMD_OFF_DIS     0x00    //����ʾ
#define CMD_ON_DIS      0x01    //����ʾ

//-- Enumerations --------------------------------------------------------------
extern void tm1650_Init( unsigned char id );
extern etError tm1650_WriteConfig(unsigned char id, unsigned char config );
extern etError tm1650_ReadKeycode( unsigned char id, unsigned char *status );
extern etError tm1650_WriteDisplay( unsigned char id, unsigned char addr, unsigned char display );

#endif
