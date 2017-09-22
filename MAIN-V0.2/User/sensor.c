/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称:    sensor.c
* 描    述:    PM2.5.c文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/6/1

* 修改历史:    时间        内容                  修改者
               2017/6/1:  文件创建              王晶东
*******************************************************************************/
//includes
#include "sensor.h"
#include "data.h"

unsigned short m_pm25_period, m_pm25_test;

//----local definitions---------------------------------------------------------
void sensor_init( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	//pm2.5控制线初始化 Reset(PA0)/Set(PA4)
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//初始化启动状态 开启PM2.5
	GPIO_SetBits(GPIOA, GPIO_Pin_1);  
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
	//m_pm25_status = 1;
	m_DevData.pm25status = 1;
	m_pm25_period = 0;
}

//pm2.5模式 0：表示停止 1：表示运行
void pm25_mode( unsigned char mode )
{
  	if( (m_DevData.pm25status == 0) && (mode == PM25_MODE_RUN) )
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		m_DevData.pm25status = 1;
	}
	else if( (m_DevData.pm25status == 1) && (mode == PM25_MODE_STOP) )
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		m_DevData.pm25status = 0;
	}
}

//pm2.5控制函数
void pm25_process( void )
{
    //1.开机状态检测
    if( m_RunData.runmode  > 0 )
    {
        if( m_DevData.actived > 0 )
        {
            pm25_mode(PM25_MODE_RUN);
            m_pm25_period = 0;
        }
        else
        {
            if( m_pm25_period > 0 )
                m_pm25_period--;
            if( m_pm25_test > 0 )
                m_pm25_test--;
            if( m_pm25_period == 0 )
            {
                pm25_mode(PM25_MODE_RUN);
                m_pm25_period = PM25_PERIOD;
                m_pm25_test = PM25_TEST;
            }
            if( m_pm25_test == 0 )
            {
                pm25_mode(PM25_MODE_STOP);
            }
        }
    }
    //2.关机状态关闭
    else
    {
        pm25_mode(PM25_MODE_STOP);
    }
}

//传感器复位
void sensor_reset( unsigned char id )
{
  	if( id == SENSOR_PM25 )
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		m_DevData.time_reset = TIME_RESET;
	}
	else
	{
	  	//GPIO_SetBits(GPIOC, GPIO_Pin_5);
		//m_time_reset[1] = TIME_RESET;
	}
}


/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************/
