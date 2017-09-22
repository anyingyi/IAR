/****************** (C) COPYRIGHT 2010 ����·�Ϲ���С�� ***********************
* �ļ�����:    warn.h
* ��    ��:    �澯�ж�ģ����ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2015/11/12

* �޸���ʷ:    ʱ��       ����                  �޸���
               2015/11/12: �ļ�����              ������
*******************************************************************************/
#ifndef _WARN_H_
#define _WARN_H_
//includes
#include "data.h"

#define WARN_TIME					3  					//�澯�ж�����3s
/* @defgroup Warn_bits_define
*/
/*#define Warn_NONE					0x00000000			//�޹���
#define Warn_PM25SENSOR				0x00000001			//PM2.5����������
#define Warn_THSENSOR				0x00000002			//��ʪ�ȴ���������
#define Warn_CO2SENSOR				0x00000004			//CO2����������
#define Warn_TVOCSENSOR				0x00000008	    	//TVOC����������
#define Warn_PM25					0x00000010			//PM2.5�澯
#define Warn_TEMP					0x00000020			//�¶ȸ澯
#define Warn_HUMI					0x00000040			//ʪ�ȸ澯
#define Warn_CO2					0x00000080			//CO2�澯
#define Warn_TVOC					0x00000100			//TVOC�澯
#define Warn_FILTER					0x00000200			//���������澯*/

/* @defgroup ����ֵ
*/
#define	WAVE_PM25	                3					//3
#define	WAVE_CO2	                40					//40
#define	WAVE_TVOC	                10					//10
#define	WAVE_TEMP	                30   				//0.3
#define	WAVE_HUMI	                30   				//0.3


#define RATE_FILTER                 15                  //��оʣ��15%ʱ�澯

/*typedef enum
{
  	CMD_Warn_NONE					= 0x00,
  	CMD_Warn_PM25SENSOR				= 0x01, 			//PM2.5����������
	CMD_Warn_THSENSOR				= 0x02,				//��ʪ�ȴ���������
  	CMD_Warn_CO2SENSOR				= 0x03,				//CO2����������
	CMD_Warn_TVOCSENSOR				= 0x04,         	//��ȩ����������
	CMD_Warn_PM25					= 0x05,				//PM2.5�澯
	CMD_Warn_TEMP					= 0x06,				//�¶ȸ澯
	CMD_Warn_HUMI					= 0x07,				//ʪ�ȸ澯
	CMD_Warn_CO2					= 0x08,				//CO2�澯
	CMD_Warn_TVOC					= 0x09,				//TVOC�澯
	CMD_Warn_FILTER					= 0x0A,				//��ظ澯

	CMD_Warn_MAX					= 0x0B,				//�澯����
}CMD_WARN_ID;*/


/* Exported macro ------------------------------------------------------------*/
extern void analy_warn( void );
extern void warn_process( void );
extern unsigned char analy_warnitem( unsigned int warnbits );


#endif //_WARN_H_
/*********(C) COPYRIGHT 2010 ����·�Ϲ���С��*****End of this file*************/