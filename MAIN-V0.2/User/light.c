/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: light.c
* 描    述: 光强度.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/3

* 修改历史: 时间       内容           修改者
            2017/6/3 :文件创建       王晶东
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
* 函数名称  : light_process
* 描    述  : 获取光强度
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void light_process(void)
{
    //adc采样
    if( adc_check() )
    {
        //1.光强度采样平均值
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

/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
