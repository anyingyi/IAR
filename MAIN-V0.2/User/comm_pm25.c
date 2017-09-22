/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************
* �ļ�����: comm_co2.c
* ��    ��: xxxx.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/2

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/2 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "comm_pm25.h"
#include "sensor.h"
#include "comm.h"
#include "data.h"

unsigned char m_pm25_timeout = 0;

/******************************************************************************
//  ������Ӧ������Ϣ
*******************************************************************************/
int Com_PM25_Rsp( unsigned char* buf,unsigned char id)
{
	unsigned char i;
	unsigned short *p;
	
  	if( (buf[0] != 0x42) || (buf[1] != 0x4d) )
		return 0;
	//1.���ݴ洢
	p = &m_RunData.pm01;
	for( i = 0; i < 3; i++ )
	{
		*p = (buf[10+2*i]<<8)|buf[11+2*i];
		p++;
	}

	m_pm25_timeout = 0;
    m_RunData.status &= ~0x01;
	m_RunData.warnsensor &= ~0x01;  
	return 1;   
}

/*-------------------------------------------------------------
//2.��������ͨ��--��ʱ����
**------------------------------------------------------------*/
int Com_PM25_Dly( unsigned char* buf,unsigned char id )
{
  	//����������״̬�²����泬ʱ
  	if( m_DevData.pm25status == 1 )
	{
		if( m_pm25_timeout < 12)
		{
			m_pm25_timeout++;
			if( m_pm25_timeout == 12 )
			{
				//1.���������ϱ�־λ
                m_RunData.status |= 0x01;
                m_RunData.warnsensor |= 0x01;  
				//2.��λPM2.5������
				sensor_reset(SENSOR_PM25);
				m_pm25_timeout = 0;
			}
		}
	}
    return 0;
}

