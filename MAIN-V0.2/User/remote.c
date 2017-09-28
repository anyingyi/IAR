/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: remote.c
* ��    ��: ң����.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/8/10

* �޸���ʷ: ʱ��       ����           �޸���
            2017/8/10 :�ļ�����       ������
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "remote.h"
#include "data.h"
#include "flash.h"
#include "buzzer.h"
#include "ledctrl.h"
#include "lamp.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

CREMOTE m_remote;
unsigned int m_studybuff[3];
unsigned int m_studycode;
/* Private variables ---------------------------------------------------------*/
/*******************************************************************************
* ��������  : remote_init
* ��    ��  : ң������ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void remote_init( void )   
{   
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    GPIO_InitTypeDef  		  GPIO_InitStructure;
    TIM_ICInitTypeDef         TIM_ICInitStructure;
    
    //IO�ڳ�ʼ��
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //PWM��������
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 11999;  //����12ms
    TIM_TimeBaseStructure.TIM_Prescaler = 71; 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    //ͨ������
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //ͨ��ѡ��
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //�����ش���
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //ÿ����������
    TIM_ICInitStructure.TIM_ICFilter = 0x00; //���˲�
    TIM_PWMIConfig(TIM4, &TIM_ICInitStructure);
    //����ͨ������
    TIM_SelectInputTrigger(TIM4, TIM_TS_TI1FP1); //ѡ��IC1Ϊʼ�մ���Դ
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable); //������ʱ���ı�������
    TIM_Cmd(TIM4, ENABLE); //����TIM4
    TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE); //�������ж�
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1); //��������жϱ��
}

/*******************************************************************************
* ��������  : remote_analyze
* ��    ��  : ң���������
* ��    ��  : keycode��������
* ��    ��  : �����������
* ��    ��  : None
*******************************************************************************/
unsigned char remote_analyze( unsigned int keycode )
{
    unsigned char i, key;
    
    key = REMOTE_NONE;
    //1.��ѯ�������
    for( i = 0; i < 6; i++ )
    {
        if( keycode == m_SetData.keycode[i] ) //������
        {
            key = i+1;
            break;
        }
    }
    //2.�ж��Ƿ�Ϊ΢����
    if( key == REMOTE_NONE )
    {
        //2.1.΢����
        if( (keycode & 0xFFFFFFF0) == (m_SetData.keycode[0] & 0xFFFFFFF0) ) 
            key = REMOTE_ADJUST;
    }
    
    return key;
}

/*******************************************************************************
* ��������  : remote_process
* ��    ��  : ң���������������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void remote_process( void )
{
    //0.��ң�ذ�������
    if( m_remote.keycode > 0)
    {
        led_on( LED_RX480,  LED_BLINK, 50, 50, 4, 0 );
    }
    //1.�а����������ڷ�ѧϰ״̬,��ȡ����������
    if( !m_remote.studystatus )
    {
        if( m_remote.keycode > 0 )   
        {
            m_remote.keyid = remote_analyze(m_remote.keycode);
            m_remote.keycode = 0;
            m_DevData.actived = ACTTIME;
            m_DevData.devtim_5min = ESCTIME;
            m_DevData.blink = BLINK_TIME;
            //1.�ȵ�������
            if( (m_DevData.lcdonoff == 0) && (m_RunData.runmode > 0) )
                m_remote.keyid = REMOTE_NONE;
            //2.��������
            m_DevData.lcdonoff = 1;
        }
    }
    //2.ң��ѧϰ״̬
    remote_study();
}

/*******************************************************************************
* ��������  : remote_studyen
* ��    ��  : ң����ѧϰ����ʹ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void remote_studyen( unsigned char en )
{
    //1.ѧϰ���ܿ���
    if( en == 1 )
    {
        m_remote.studystatus = 1;
        m_remote.studytimes = STUDY_TIME;
    }
    else
    {
        m_remote.studystatus = 0;
        m_remote.studytimes = 0;  
        m_remote.keyid = 0;
    }
    m_remote.studycounts = 0;
    m_remote.studysucc = 0;
}

/*******************************************************************************
* ��������  : remote_study
* ��    ��  : ң����ѧϰ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void remote_study( void )
{
    unsigned int code = 0;
    
    //1.ң��������ѧϰ״̬
    if( m_remote.studystatus == 1 )
    {
        //1.1.�а���
        if( m_remote.keycode > 0 )
        {
            //1.1.1���µ�Ϊ���ػ���ť��ȡ����ֵ
            if( (m_remote.keycode & 0x000F) == 0x0003 )
            {
                //1.1.1.�ݴ水��ֵ
                if( m_remote.studycounts < 3 )
                {
                    m_studybuff[m_remote.studycounts] = m_remote.keycode;
                    m_remote.studycounts++;
                    //1.1.2ͬһ������3�κ��ж�
                    if( m_remote.studycounts == 3 )
                    {
                        //1.1.2.1ѧϰ�ɹ�
                        if( (m_studybuff[0] == m_studybuff[1]) && 
                            (m_studybuff[0] == m_studybuff[2]))
                        {
                            m_studycode = m_studybuff[0];
                            m_remote.studycounts = 0;
                            m_remote.studystatus = 0;
                            m_remote.studytimes = 0;
                            m_remote.studysucc = 3;
                            //1.2.1.��������
                            code = m_studycode&0xFFFFFFF0;
                            m_SetData.keycode[0] = m_studycode; 
                            m_SetData.keycode[1] = code|0x09; 
                            m_SetData.keycode[2] = code|0x07; 
                            m_SetData.keycode[3] = code|0x04; 
                            m_SetData.keycode[4] = code|0x02;
                            m_SetData.keycode[5] = code|0x08; 
                            flash_e( ADDR_SETTING, sizeof( struct SetData ) );
                            flash_w( (unsigned short*)&m_SetData,ADDR_SETTING,sizeof( struct SetData ));
                            //1.2.2.����
                            buzzer_on( 0, MUSIC_STUDY );
                            lamp_off( LAMP_ONOFF );
                        }
                        else
                        {
                            m_remote.studycounts = 0;
                        }
                    }
                }
            }
            else
            {
                m_remote.studycounts = 0;
            }
            //1.1.3.����ֵ����
            m_remote.keycode = 0;
        }
    }
}


/**************** (C) COPYRIGHT 2017 ���ݿ����Ƽ��ɷ����޹�˾ *****************/
