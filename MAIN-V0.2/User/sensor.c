/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����:    sensor.c
* ��    ��:    PM2.5.c�ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/6/1

* �޸���ʷ:    ʱ��        ����                  �޸���
               2017/6/1:  �ļ�����              ������
*******************************************************************************/
//includes
#include "sensor.h"
#include "data.h"

unsigned short m_pm25_period, m_pm25_test;

//----local definitions---------------------------------------------------------
void sensor_init( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	//pm2.5�����߳�ʼ�� Reset(PA0)/Set(PA4)
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//��ʼ������״̬ ����PM2.5
	GPIO_SetBits(GPIOA, GPIO_Pin_1);  
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
	//m_pm25_status = 1;
	m_DevData.pm25status = 1;
	m_pm25_period = 0;
}

//pm2.5ģʽ 0����ʾֹͣ 1����ʾ����
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

//pm2.5���ƺ���
void pm25_process( void )
{
    //1.����״̬���
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
    //2.�ػ�״̬�ر�
    else
    {
        pm25_mode(PM25_MODE_STOP);
    }
}

//��������λ
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


/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************/
