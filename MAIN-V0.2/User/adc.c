/************* (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ *****************
* �ļ�����:    adc.c
* ��    ��:    ad����.c�ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/6/7

* �޸���ʷ:    ʱ��        ����                  �޸���
               2017/6/7:  �ļ�����              ������
*******************************************************************************/
#include "adc.h"
#include "data.h"

//volatile unsigned short m_ADC_RegConvValTab[ADC_CHNUM];
//volatile unsigned short m_ADC_InjConvSmv[ADC_CNT];
//volatile unsigned short m_ADC_InjConvSmpCnt = 0;
//volatile unsigned short m_ADC_InjConvArgSmv;

ADC_CTRL m_ADC_CTRL;

/*******************************************************************************
* ��������  : Adc_Init
* ��    ��  : ADC��ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
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
* ��������  : adc_process
* ��    ��  : ADC������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
unsigned char adc_check( void )
{
    unsigned char rt, status;
    
    rt = 0;
    //1.����״̬����ת��
    if( m_ADC_CTRL.busy == 0 ) 
    {   
        //1.1.����ת��
        ADC_SoftwareStartConvCmd( ADC1, ENABLE );  
        m_ADC_CTRL.busy = 1;
    }
    //2.��ѯ�Ƿ�ת������
    else
    {
        //2.1.��ѯ״̬
        status = ADC_GetFlagStatus( ADC1, ADC_FLAG_EOC );
        if( status == SET )
        {
            m_ADC_CTRL.samplesum += ADC_GetConversionValue( ADC1 );
            m_ADC_CTRL.smpcnt++;
            m_ADC_CTRL.busy = 0;
        }
    }
    //3.�ﵽ��������
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
    //ע����ת��������־
    if( ADC_GetFlagStatus( ADC1, ADC_FLAG_JEOC ) == 1 )
    {
        //�����־λ
        ADC_ClearFlag( ADC1, ADC_FLAG_JEOC );
        if( m_ADC_InjConvSmpCnt < ADC_CNT )
        {
            //��ȡע��ͨ������
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
    //ȥ��ֵȡƽ��
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
    //ȥ��ֵȡƽ��
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


/***************(C) COPYRIGHT 20127 ���ݿ������ܿƼ��ɷ����޹�˾***************/

