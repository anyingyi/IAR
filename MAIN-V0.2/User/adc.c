/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 *****************
* 文件名称:    adc.c
* 描    述:    ad采样.c文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/6/7

* 修改历史:    时间        内容                  修改者
               2017/6/7:  文件创建              王晶东
*******************************************************************************/
#include "adc.h"
#include "data.h"

//volatile unsigned short m_ADC_RegConvValTab[ADC_CHNUM];
//volatile unsigned short m_ADC_InjConvSmv[ADC_CNT];
//volatile unsigned short m_ADC_InjConvSmpCnt = 0;
//volatile unsigned short m_ADC_InjConvArgSmv;

ADC_CTRL m_ADC_CTRL;

/*******************************************************************************
* 函数名称  : Adc_Init
* 描    述  : ADC初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void Adc_Init(void)
{
    ADC_InitTypeDef           ADC_InitStructure;
    
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_CHNUM;
    ADC_Init(ADC1, &ADC_InitStructure);
    /* ADC1 regular channel14 configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    /* Enable ADC1 reset calibration register */   
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));
    /* Start ADC1 calibration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));
}

/*******************************************************************************
* 函数名称  : adc_process
* 描    述  : ADC处理函数
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
unsigned char adc_check( void )
{
    unsigned char rt, status;
    
    rt = 0;
    //1.空闲状态启动转换
    if( m_ADC_CTRL.busy == 0 ) 
    {   
        //1.1.启动转换
        ADC_SoftwareStartConvCmd( ADC1, ENABLE );  
        m_ADC_CTRL.busy = 1;
    }
    //2.查询是否转换结束
    else
    {
        //2.1.查询状态
        status = ADC_GetFlagStatus( ADC1, ADC_FLAG_EOC );
        if( status == SET )
        {
            m_ADC_CTRL.samplesum += ADC_GetConversionValue( ADC1 );
            m_ADC_CTRL.smpcnt++;
            m_ADC_CTRL.busy = 0;
        }
    }
    //3.达到采样次数
    if( m_ADC_CTRL.smpcnt == ADC_SAMPLE )
    {
        m_ADC_CTRL.argsmv = m_ADC_CTRL.samplesum/ADC_SAMPLE;
        m_ADC_CTRL.samplesum = 0;
        m_ADC_CTRL.busy = 0;
        m_ADC_CTRL.smpcnt = 0;
        rt = 1;
    }
    
    return rt;
}



/*void Get_InjectAdcValue(void)
{
    //注入组转换结束标志
    if( ADC_GetFlagStatus( ADC1, ADC_FLAG_JEOC ) == 1 )
    {
        //清楚标志位
        ADC_ClearFlag( ADC1, ADC_FLAG_JEOC );
        if( m_ADC_InjConvSmpCnt < ADC_CNT )
        {
            //读取注入通道数据
            m_ADC_InjConvSmv[m_ADC_InjConvSmpCnt] = 
                    ADC_GetInjectedConversionValue( ADC1, ADC_InjectedChannel_1 );
            m_ADC_InjConvSmpCnt++;
        }
    }
}

unsigned char Adc_Check(void)
{
    if( m_ADC_InjConvSmpCnt < ADC_CNT )
        return 0;
    //去极值取平均
    unsigned int sum;
    unsigned short j,min,max,smv;

    sum = 0;min = 0xffff;max = 0;
    for( j = 0; j < ADC_CNT; j++ )
    {
        smv =  m_ADC_InjConvSmv[j];
        if( min > smv )min = smv;
        if( max < smv )max = smv;
        sum += smv;
    }
    m_ADC_InjConvArgSmv = ( sum - min - max )/(ADC_CNT - 2);
    
    m_ADC_InjConvSmpCnt = 0;
    return 1;
}*/

/*unsigned char Adc_Check( unsigned char id )
{
    if( m_ADC_CTRL.smpcnt < ADC_CNT )
        return 0;
    //去极值取平均
    unsigned int sum;
    unsigned short j,min,max,smv;

    sum = 0;min = 0xffff;max = 0;
    for( j = 0; j < ADC_CNT; j++ )
    {
        smv =  m_ADC_CTRL.smv[id][j];
        if( min > smv )min = smv;
        if( max < smv )max = smv;
        sum += smv;
    }
    m_ADC_CTRL.argsmv[id] = ( sum - min - max )/(ADC_CNT - 2);
    
    return 1;
}*/


/***************(C) COPYRIGHT 20127 杭州卡丽智能科技股份有限公司***************/

