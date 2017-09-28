/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: remote.c
* 描    述: 遥控器.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/8/10

* 修改历史: 时间       内容           修改者
            2017/8/10 :文件创建       王晶东
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
* 函数名称  : remote_init
* 描    述  : 遥控器初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void remote_init( void )   
{   
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    GPIO_InitTypeDef  		  GPIO_InitStructure;
    TIM_ICInitTypeDef         TIM_ICInitStructure;
    
    //IO口初始化
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //PWM周期配置
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 11999;  //周期12ms
    TIM_TimeBaseStructure.TIM_Prescaler = 71; 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    //通道配置
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //通道选择
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //上升沿触发
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //每次输入均检测
    TIM_ICInitStructure.TIM_ICFilter = 0x00; //不滤波
    TIM_PWMIConfig(TIM4, &TIM_ICInitStructure);
    //主从通道配置
    TIM_SelectInputTrigger(TIM4, TIM_TS_TI1FP1); //选择IC1为始终触发源
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable); //启动定时器的被动触发
    TIM_Cmd(TIM4, ENABLE); //启动TIM4
    TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE); //允许捕获中断
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1); //清除捕获中断标记
}

/*******************************************************************************
* 函数名称  : remote_analyze
* 描    述  : 遥控器码解析
* 输    入  : keycode：按键码
* 输    出  : 按键功能序号
* 返    回  : None
*******************************************************************************/
unsigned char remote_analyze( unsigned int keycode )
{
    unsigned char i, key;
    
    key = REMOTE_NONE;
    //1.查询按键序号
    for( i = 0; i < 6; i++ )
    {
        if( keycode == m_SetData.keycode[i] ) //按键码
        {
            key = i+1;
            break;
        }
    }
    //2.判断是否为微调键
    if( key == REMOTE_NONE )
    {
        //2.1.微调键
        if( (keycode & 0xFFFFFFF0) == (m_SetData.keycode[0] & 0xFFFFFFF0) ) 
            key = REMOTE_ADJUST;
    }
    
    return key;
}

/*******************************************************************************
* 函数名称  : remote_process
* 描    述  : 遥控器按键处理过程
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void remote_process( void )
{
    //0.有遥控按键动作
    if( m_remote.keycode > 0)
    {
        led_on( LED_RX480,  LED_BLINK, 50, 50, 4, 0 );
    }
    //1.有按键动作且在非学习状态,获取按键功能码
    if( !m_remote.studystatus )
    {
        if( m_remote.keycode > 0 )   
        {
            m_remote.keyid = remote_analyze(m_remote.keycode);
            m_remote.keycode = 0;
            m_DevData.actived = ACTTIME;
            m_DevData.devtim_5min = ESCTIME;
            m_DevData.blink = BLINK_TIME;
            //1.先点亮背光
            if( (m_DevData.lcdonoff == 0) && (m_RunData.runmode > 0) )
                m_remote.keyid = REMOTE_NONE;
            //2.开启背光
            m_DevData.lcdonoff = 1;
        }
    }
    //2.遥控学习状态
    remote_study();
}

/*******************************************************************************
* 函数名称  : remote_studyen
* 描    述  : 遥控器学习功能使能
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void remote_studyen( unsigned char en )
{
    //1.学习功能开启
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
* 函数名称  : remote_study
* 描    述  : 遥控器学习功能
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void remote_study( void )
{
    unsigned int code = 0;
    
    //1.遥控器处于学习状态
    if( m_remote.studystatus == 1 )
    {
        //1.1.有按键
        if( m_remote.keycode > 0 )
        {
            //1.1.1按下的为开关机按钮获取按键值
            if( (m_remote.keycode & 0x000F) == 0x0003 )
            {
                //1.1.1.暂存按键值
                if( m_remote.studycounts < 3 )
                {
                    m_studybuff[m_remote.studycounts] = m_remote.keycode;
                    m_remote.studycounts++;
                    //1.1.2同一按键按3次后判断
                    if( m_remote.studycounts == 3 )
                    {
                        //1.1.2.1学习成功
                        if( (m_studybuff[0] == m_studybuff[1]) && 
                            (m_studybuff[0] == m_studybuff[2]))
                        {
                            m_studycode = m_studybuff[0];
                            m_remote.studycounts = 0;
                            m_remote.studystatus = 0;
                            m_remote.studytimes = 0;
                            m_remote.studysucc = 3;
                            //1.2.1.保存数据
                            code = m_studycode&0xFFFFFFF0;
                            m_SetData.keycode[0] = m_studycode; 
                            m_SetData.keycode[1] = code|0x09; 
                            m_SetData.keycode[2] = code|0x07; 
                            m_SetData.keycode[3] = code|0x04; 
                            m_SetData.keycode[4] = code|0x02;
                            m_SetData.keycode[5] = code|0x08; 
                            flash_e( ADDR_SETTING, sizeof( struct SetData ) );
                            flash_w( (unsigned short*)&m_SetData,ADDR_SETTING,sizeof( struct SetData ));
                            //1.2.2.长鸣
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
            //1.1.3.按键值清零
            m_remote.keycode = 0;
        }
    }
}


/**************** (C) COPYRIGHT 2017 杭州卡丽科技股份有限公司 *****************/
