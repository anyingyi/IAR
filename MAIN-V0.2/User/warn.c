/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 *****************
* 文件名称:    warn.c
* 描    述:    故障告警管理模块.C文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/6/6

* 修改历史:    时间        内容                  修改者
               2017/6/6:  文件创建              王晶东
*******************************************************************************/
//includes
#include "warn.h"

unsigned char m_pm25_warnlevel = 0;
unsigned char m_co2_warnlevel = 0;
unsigned char m_tvoc_warnlevel = 0;
unsigned char m_warn_time = 0;

/* Exported functions ------------------------------------------------------- */
/*******************************************************************************
* 函数名称  : wave_threshold(void)
* 描    述  : 阀值波动
* 输    入  : None
* 输    出  : None
* 返    回  : None
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
* 函数名称  : analy_warn(void)
* 描    述  : 告警解析
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void analy_warn( void )
{
	unsigned short real;  
	unsigned short* threshold;
    unsigned char oldstatus;
 
    //1.pm2.5告警判断 2-轻度污染、3-中度污染、4-重度污染、5-严重污染
	real = m_RunData.pm25;
	threshold = m_SetData.pm25;
	oldstatus = m_RunData.wpm25;
	m_RunData.wpm25 = wave_threshold( real, threshold, 4, WAVE_PM25, oldstatus );  
	//2.温度告警判断 1-低温告警、2-高温告警  
	real = m_RunData.temp;
	threshold = (unsigned short*)m_SetData.temp;
	if( real < (threshold[0]-WAVE_TEMP) )
		m_RunData.wtemp = 1;
	else if( real >= (threshold[1] + WAVE_TEMP) )
		m_RunData.wtemp = 2;
	else if( ( real >= (threshold[0] + WAVE_TEMP)) && ( real < (threshold[1] - WAVE_TEMP)) )
		m_RunData.wtemp = 0;
	//3.湿度告警判断 1-湿度过低告警、2-湿度过高告警
	real = m_RunData.humi;
	threshold = m_SetData.humi;
	if( real < (threshold[0] - WAVE_HUMI) )
		m_RunData.whumi = 1;
	else if( real >= (threshold[1] + WAVE_HUMI) )
		m_RunData.whumi = 2;
	else if( ( real >= (threshold[0] + WAVE_HUMI)) && ( real < (threshold[1] - WAVE_HUMI)) )
		m_RunData.whumi = 0;  
	//4.CO2告警判断 2-中度告警、3-严重告警
	real = m_RunData.co2;
	threshold = m_SetData.co2;
	oldstatus = m_RunData.wco2; 
	m_RunData.wco2 = wave_threshold( real, threshold, 2, WAVE_CO2, oldstatus );    
	//5.TVOC告警判断 1-临界告警、2-超标告警
	real = m_RunData.tvoc;
	threshold = m_SetData.tvoc;
	oldstatus = m_RunData.wtvoc; 
	m_RunData.wtvoc = wave_threshold( real, threshold, 1, WAVE_TVOC, oldstatus );    
    //6.滤网告警 0-正常、1-告警
    m_RunData.wfilter = 0;
    if( m_RunData.filter < RATE_FILTER )
        m_RunData.wfilter = 1;  
}

/*******************************************************************************
* 函数名称  : warn_process
* 描    述  : 
* 输    入  : 
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void warn_process( void )
{
	m_warn_time++;
	if( m_warn_time == WARN_TIME )
	{
		//告警判断
		analy_warn();
		m_warn_time = 0;
	}
}

/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/