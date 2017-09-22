/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************
* 文件名称: comm_co2.c
* 描    述: xxxx.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/2

* 修改历史: 时间       内容           修改者
            2017/6/2 :文件创建       王晶东
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "comm_pm25.h"
#include "sensor.h"
#include "comm.h"
#include "data.h"

unsigned char m_pm25_timeout = 0;

/******************************************************************************
//  对上响应数据信息
*******************************************************************************/
int Com_PM25_Rsp( unsigned char* buf,unsigned char id)
{
	unsigned char i;
	unsigned short *p;
	
  	if( (buf[0] != 0x42) || (buf[1] != 0x4d) )
		return 0;
	//1.数据存储
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
//2.对上网络通信--超时函数
**------------------------------------------------------------*/
int Com_PM25_Dly( unsigned char* buf,unsigned char id )
{
  	//传感器运行状态下才是真超时
  	if( m_DevData.pm25status == 1 )
	{
		if( m_pm25_timeout < 12)
		{
			m_pm25_timeout++;
			if( m_pm25_timeout == 12 )
			{
				//1.传感器故障标志位
                m_RunData.status |= 0x01;
                m_RunData.warnsensor |= 0x01;  
				//2.复位PM2.5传感器
				sensor_reset(SENSOR_PM25);
				m_pm25_timeout = 0;
			}
		}
	}
    return 0;
}

