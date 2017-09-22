/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: fan.c
* 描    述: 风机.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/8/25

* 修改历史: 时间       内容           修改者
            2017/8/25 :文件创建       王晶东
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "fanctrl.h"
#include "data.h"
#include "flash.h"

#define  ADD_1DAY_TIME      86400 //60*60*24秒
#define  ADD_1MIN_TIME      60     //秒

//电机控制信号IO
typedef struct CFANCTRL_t
{
    GPIO_TypeDef*   GPIOx;      //GPIO
    unsigned short  GPIO_Pin;   //pin
}CFANCTRL;

const CFANCTRL mFANCTRL = 
{
    GPIOA, GPIO_Pin_7
}; 

//电机控制数据结构
CFAN m_fan;
//档位信号
const unsigned char m_fan_duty[4] = { FAN_PWM_OFF,          //电机关机 
                                      FAN_PWM_LEVEL1,       //电机一档
                                      FAN_PWM_LEVEL2,       //电机二档
                                      FAN_PWM_LEVEL3 };     //电机三档
//微调临界值
const unsigned char m_fan_pwm[4] = {  FAN_PWM_OFF,          //电机关机
                                      FAN_PWM_MAX,          //可调最大值
                                      FAN_PWM_LEVEL1,       //电机二档可调最低值
                                      FAN_PWM_LEVEL2 };     //电机三档可调最低值 


unsigned int  m_1DayTimeInc, m_1MinTimeInc;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
* 函数名称  : fan_init
* 描    述  : 电机初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void fan_init(void)   
{   
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;
    GPIO_InitTypeDef  		  GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin   = mFANCTRL.GPIO_Pin;;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(mFANCTRL.GPIOx, &GPIO_InitStructure);
    //PWM配置
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 99;//20KHz
    TIM_TimeBaseStructure.TIM_Prescaler = 35; 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    /* Output Compare Active Mode configuration: Channel2 */    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = 0; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //使能输出状态
    TIM_OC2Init(TIM3, &TIM_OCInitStructure); //TIM3_CH2通道使能 
	TIM_Cmd(TIM3, ENABLE);
	TIM_SetCompare2(TIM3, FAN_PWM_OFF);
    
    //参数初始化
    m_fan.status = 0;
    m_fan.cmd = 0;
    m_fan.flag = 0;
    m_fan.duty = FAN_PWM_OFF;  
}

/*******************************************************************************
* 函数名称  : fan_set
* 描    述  : 电机控制参数设置
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void fan_set( unsigned char cmd )
{
    unsigned char level;
    
    //1.处于关机状态时进入启动状态
    if( m_fan.status == FAN_STATUS_OFF )
    {
        if( cmd > FAN_OFF )
        {
            //1.1.启动参数
            m_fan.status = FAN_STATUS_START; 
            m_fan.duty = FAN_PWM_START;
            m_fan.time = FAN_START_TIME;
            m_fan.flag = 1;
        }
    }
    //2.处于开机状态
    else
    {
        //2.1.关机命令
        if( cmd == FAN_OFF )
        {
            m_fan.duty = FAN_PWM_OFF;
            m_fan.status = FAN_STATUS_OFF;
        }
        //2.2.一二三档状态
        else if( (cmd >= FAN_LEVEL1) && (cmd <= FAN_LEVEL3) )
        {
            m_fan.duty = m_fan_duty[cmd];
            m_fan.status = FAN_STATUS_ON; 
        }
        //2.3.微调状态
        else if( cmd == FAN_ADJUST )
        {
            level = m_RunData.runmode & 0x0003;
            if( m_fan.duty < m_fan_pwm[level] ) 
                m_fan.duty += FAN_PWM_DWN;
            m_fan.status = FAN_STATUS_ON; 
        }
    }
    //3.在非启动状态时更新控制信号
    if( m_fan.status != FAN_STATUS_START )
        m_fan.flag = 1;
    m_fan.cmd = cmd;
}

/*******************************************************************************
* 函数名称  : fan_ctrl
* 描    述  : 电机控制函数
* 输    入  : status：运行模式
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void fan_ctrl( unsigned short status )
{
    unsigned char level;
    
    //1.风机模式有变化
    if( m_DevData.prerunmode != status )
    {
        //1.1.关机状态记录关机档位
        if( status == FAN_OFF )
        {
            if( !m_DevData.timemode )
                m_DevData.oldrunmode = m_RunData.runmode&0x2f;
            else
                m_DevData.oldrunmode = m_RunData.runmode;  
        }
        //1.2.模式更新
        m_RunData.runmode = status;
        //1.1.微调模式
        if( (m_RunData.runmode & FAN_ADJUST) == FAN_ADJUST )
        {
            //1.1.1.微调设置
            fan_set( FAN_ADJUST );
            //1.1.2.标志位清零
            m_RunData.runmode &= ~FAN_ADJUST; 
        }
        //1.2.其他模式
        else
        {
            //1.2.1.档位
            level = status & 0x0F;  
            //1.2.2.正确档位数据
            if( level <= FAN_LEVEL3 )
                fan_set( level );
        }
        //1.3.备份运行模式
        m_DevData.prerunmode = m_RunData.runmode;
    }
}

/*******************************************************************************
* 函数名称  : fan_process
* 描    述  : 电机处理过程函数
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void fan_process( void )
{
    //1.风机处于启动状态
    if( ( m_fan.status == FAN_STATUS_START) && (!m_fan.time) )
    {
        m_fan.status = FAN_STATUS_ON;  
        fan_set( m_fan.cmd );
    }
    //2.控制信号有更新
    if( m_fan.flag )
    {
        TIM_SetCompare2(TIM3, m_fan.duty );
        m_fan.flag = 0;
    }
}

/*******************************************************************************
* 函数名称  : fan_totaltime
* 描    述  : 电机处理过程函数
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void fan_totaltime(void)
{
    unsigned char save;
    float rate;
    
    //风机开启状态
    save = 0;
	//1.有档位数据，计时
	if( (m_RunData.runmode & 0x0F) > FAN_OFF )
    {
        m_1DayTimeInc++;
        m_1MinTimeInc++;
         //1.累计1天保存一次数据
        if( m_1DayTimeInc == ADD_1DAY_TIME )
        {
            m_1MinTimeInc = 0;
            m_1DayTimeInc = 0;
            m_RunData.totaltime += ADD_1DAY_TIME/60;
            m_SetData.usedfilter = m_RunData.totaltime;
            save |= 1;
        }
        else
        {
            //累计1分钟更新一次数据
            if( m_1MinTimeInc == ADD_1MIN_TIME )
            {
                m_1MinTimeInc = 0;
                m_RunData.totaltime += ADD_1MIN_TIME/60;
                m_SetData.usedfilter = m_RunData.totaltime;
                //计算滤芯剩余百分比
                if( m_SetData.filtertime > m_RunData.totaltime )
                {
                    rate = m_SetData.filtertime - m_RunData.totaltime;
                    rate = rate*100/m_SetData.filtertime+0.5;
                }
                else
                {
                    rate = 0;
                }
                m_RunData.filter = (unsigned short)rate;
            }        
        }
    }
    if( save == 1 )
    {
        //参数保存
        flash_e( ADDR_SETTING, sizeof( struct SetData ) );
        flash_w( (unsigned short*)&m_SetData, ADDR_SETTING, sizeof( struct SetData ));
    }
}

/**************** (C) COPYRIGHT 2012 杭州卡丽科技股份有限公司 *****************/
