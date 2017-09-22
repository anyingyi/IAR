/************* (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ *****************
* �ļ�����:    warn.c
* ��    ��:    ���ϸ澯����ģ��.C�ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/6/6

* �޸���ʷ:    ʱ��        ����                  �޸���
               2017/6/6:  �ļ�����              ������
*******************************************************************************/
//includes
#include "warn.h"

unsigned char m_pm25_warnlevel = 0;
unsigned char m_co2_warnlevel = 0;
unsigned char m_tvoc_warnlevel = 0;
unsigned char m_warn_time = 0;

/* Exported functions ------------------------------------------------------- */
/*******************************************************************************
* ��������  : wave_threshold(void)
* ��    ��  : ��ֵ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
unsigned char wave_threshold( unsigned short real, unsigned short* threshold, unsigned char level, unsigned char wave, unsigned char oldstatus)
{
	unsigned char i, rt = oldstatus;
	
	if( real >= (threshold[0] - wave) )
	{
	  	if( real >= (threshold[level] + wave) )
			rt = level+1;
		else
		{
			for( i = 0; i < level; i++)
			{
				if((real >= (threshold[i] + wave)) && (real < (threshold[i+1] - wave)))
				{
					rt = i+1;
					break;
				}
			}
		}
	}
	else
	{
		rt = 0;
	}
	
	return rt;
}

/*******************************************************************************
* ��������  : analy_warn(void)
* ��    ��  : �澯����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void analy_warn( void )
{
	unsigned short real;  
	unsigned short* threshold;
    unsigned char oldstatus;
 
    //1.pm2.5�澯�ж� 2-�����Ⱦ��3-�ж���Ⱦ��4-�ض���Ⱦ��5-������Ⱦ
	real = m_RunData.pm25;
	threshold = m_SetData.pm25;
	oldstatus = m_RunData.wpm25;
	m_RunData.wpm25 = wave_threshold( real, threshold, 4, WAVE_PM25, oldstatus );  
	//2.�¶ȸ澯�ж� 1-���¸澯��2-���¸澯  
	real = m_RunData.temp;
	threshold = (unsigned short*)m_SetData.temp;
	if( real < (threshold[0]-WAVE_TEMP) )
		m_RunData.wtemp = 1;
	else if( real >= (threshold[1] + WAVE_TEMP) )
		m_RunData.wtemp = 2;
	else if( ( real >= (threshold[0] + WAVE_TEMP)) && ( real < (threshold[1] - WAVE_TEMP)) )
		m_RunData.wtemp = 0;
	//3.ʪ�ȸ澯�ж� 1-ʪ�ȹ��͸澯��2-ʪ�ȹ��߸澯
	real = m_RunData.humi;
	threshold = m_SetData.humi;
	if( real < (threshold[0] - WAVE_HUMI) )
		m_RunData.whumi = 1;
	else if( real >= (threshold[1] + WAVE_HUMI) )
		m_RunData.whumi = 2;
	else if( ( real >= (threshold[0] + WAVE_HUMI)) && ( real < (threshold[1] - WAVE_HUMI)) )
		m_RunData.whumi = 0;  
	//4.CO2�澯�ж� 2-�жȸ澯��3-���ظ澯
	real = m_RunData.co2;
	threshold = m_SetData.co2;
	oldstatus = m_RunData.wco2; 
	m_RunData.wco2 = wave_threshold( real, threshold, 2, WAVE_CO2, oldstatus );    
	//5.TVOC�澯�ж� 1-�ٽ�澯��2-����澯
	real = m_RunData.tvoc;
	threshold = m_SetData.tvoc;
	oldstatus = m_RunData.wtvoc; 
	m_RunData.wtvoc = wave_threshold( real, threshold, 1, WAVE_TVOC, oldstatus );    
    //6.�����澯 0-������1-�澯
    m_RunData.wfilter = 0;
    if( m_RunData.filter < RATE_FILTER )
        m_RunData.wfilter = 1;  
}

/*******************************************************************************
* ��������  : warn_process
* ��    ��  : 
* ��    ��  : 
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void warn_process( void )
{
	m_warn_time++;
	if( m_warn_time == WARN_TIME )
	{
		//�澯�ж�
		analy_warn();
		m_warn_time = 0;
	}
}

/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/