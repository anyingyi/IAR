/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: light.c
* ��    ��: ��ǿ��.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/3

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/3 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "light.h"
#include "data.h"
#include "adc.h"


/*static int float2int(float f)
{
    if( f > 0 )f += 0.5;
    else f -= 0.5;
    return (int)f;
}*/

/*void temp_Init(void)
{
    m_ADC_CTRL.smpcnt = ADC_CNT;
}*/


/*******************************************************************************
* ��������  : light_process
* ��    ��  : ��ȡ��ǿ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void light_process(void)
{
    //adc����
    if( adc_check() )
    {
        //1.��ǿ�Ȳ���ƽ��ֵ
        m_DevData.lightvalue = m_ADC_CTRL.argsmv;
        
        if( m_DevData.lightvalue < (THD_LIGHT+WAVE_LIGHT) )
        {
            m_DevData.lightonoff = 1;
        }
        else if( m_DevData.lightvalue > (THD_LIGHT-WAVE_LIGHT))
        {
            m_DevData.lightonoff = 0;
        }
    }
}

/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
