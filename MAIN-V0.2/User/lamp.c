/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: lamp.c
* 描    述: 数码显示.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/8/23

* 修改历史: 时间       内容           修改者
            2017/8/23 :文件创建       王晶东
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "lamp.h"
#include "data.h"

#define ADDR_LAMP   0x68

//LED控制
typedef struct CLAMPCTRL_t
{
    unsigned char   lastcmd;      //上一LED控制命令
    unsigned char   onoff;        //LAMP状态:0x00-灭,0x01-亮
    unsigned char   cmd;          //LAMP控制命令
    unsigned short  ontime;       //LAMP亮时间
    unsigned short  offtime;      //LAMP灭时间
    unsigned short  backontime;   //LAMP亮时间备份
    unsigned short  backofftime;  //LAMP灭时间备份
    unsigned char   times;        //闪灭次数:0x00常亮
}CLAMPCTRL;

typedef struct CLAMP_t
{
    unsigned char   buff;               //数据
    unsigned char   prebuff;            //上一次数据
    CLAMPCTRL       lamp[LAMP_MAX];     //数码显示
}CLAMP;

CLAMP m_lamp;

const unsigned char m_lampbit[LAMP_MAX] = { BIT_LAMP1,      //lamp1bit位
                                            BIT_LAMP2,      //lamp2bit位 
                                            BIT_LAMP3,      //lamp3bit位 
                                            BIT_LAMP4,      //lamp4bit位 
                                            BIT_LAMP5,      //lamp5bit位
                                            BIT_LAMP6 };    //lamp6bit位

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
* 函数名称  : lamp_init
* 描    述  : 按键背光灯初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void lamp_init(void)
{
    unsigned char i;
    
    //1.开机全亮并延时2秒
    for( i = 0; i < LAMP_MAX; i++ )
    {
        lamp_on( i, LAMP_BLINK, 2000, 100, 1, 0);
    }  
}

/*******************************************************************************
* 函数名称  : lamp_on
* 描    述  : 灯参数设置
* 输    入  : id:灯id号，onoff: 状态 ，blink:闪烁标志位
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void lamp_on( unsigned char id, unsigned char cmd,
              unsigned short ontime, unsigned short offtime, 
              unsigned char times, unsigned char flag )
{
    //1.参数设置
    if( m_lamp.lamp[id].cmd != LAMP_BLINK )
        m_lamp.lamp[id].lastcmd = m_lamp.lamp[id].cmd;
    m_lamp.lamp[id].cmd = cmd;
    m_lamp.lamp[id].ontime = 0;
    m_lamp.lamp[id].offtime = 0;
    m_lamp.lamp[id].backontime = ontime/10;
    m_lamp.lamp[id].backofftime = offtime/10;
    m_lamp.lamp[id].times = times;
    //2.显示数据
    if( !flag )
    {
        if( cmd == LAMP_OFF )
            m_lamp.buff &= ~m_lampbit[id];
        else
            m_lamp.buff |= m_lampbit[id];  
    }
    else
    {
        m_lamp.buff = m_lampbit[id];
    }
}

/*******************************************************************************
* 函数名称  : lamp_off
* 描    述  : 灯参数设置
* 输    入  : id:灯id号
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void lamp_off( unsigned char id )
{
    unsigned char i;
    //1.关闭所有
    if( id == LAMP_ALL )
    {
        for( i = 0; i < LAMP_MAX; i++ )
        {
            if( m_lamp.lamp[i].cmd != LAMP_OFF )
            {
                m_lamp.lamp[i].cmd = LAMP_OFF;
                m_lamp.lamp[i].ontime = 0;
                m_lamp.lamp[i].offtime = 0;
                m_lamp.lamp[i].backontime = 0;
                m_lamp.lamp[i].backofftime = 0;
                m_lamp.lamp[i].times = 0;
                m_lamp.buff &= ~m_lampbit[i];
            }
        }  
    }
    //2.单个关闭
    else
    {
        if( m_lamp.lamp[id].cmd != LAMP_OFF )
        {
            m_lamp.lamp[id].cmd = LAMP_OFF;
            m_lamp.lamp[id].ontime = 0;
            m_lamp.lamp[id].offtime = 0;
            m_lamp.lamp[id].backontime = 0;
            m_lamp.lamp[id].backofftime = 0;
            m_lamp.lamp[id].times = 0;
            m_lamp.buff &= ~m_lampbit[id];
        }
    }
}

/*******************************************************************************
* 函数名称  : lamp_single
* 描    述  : 单个背光灯处理函数
* 输    入  : id:背光灯id号
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void lamp_single( unsigned char id )
{
    //1.闪烁状态
    if(  m_lamp.lamp[id].cmd == LAMP_BLINK )
    {
        if( m_lamp.lamp[id].times > 0 )
        {
            //1.1.当前关闭状态
            if( m_lamp.lamp[id].onoff == 0x00 )
            {
                //1.1.1.关闭时间到
                if( m_lamp.lamp[id].offtime == 0 )
                {
                    //1.1.1.1.写入数据
                    m_lamp.buff |= m_lampbit[id]; 
                    m_lamp.lamp[id].ontime = m_lamp.lamp[id].backontime;
                    m_lamp.lamp[id].onoff = 1;
                }
            }
            else //1.2.当前开启状态
            {
                //1.2.1.开启时间到
                if( m_lamp.lamp[id].ontime == 0 )
                {
                    //1.2.1.1.写入数据
                    m_lamp.buff &= ~m_lampbit[id]; 
                    m_lamp.lamp[id].offtime = m_lamp.lamp[id].backofftime;
                    m_lamp.lamp[id].onoff = 0;
					//次数递减
					if( m_lamp.lamp[id].times < 0xff )
						m_lamp.lamp[id].times--;
                }
            }
            if( m_lamp.lamp[id].ontime > 0 )m_lamp.lamp[id].ontime--;//10ms为单位递减
            if( m_lamp.lamp[id].offtime > 0 )m_lamp.lamp[id].offtime--;//10ms为单位递减
        }
        else
        {
            m_lamp.lamp[id].cmd = LAMP_OFF;
        }
    }
    else //2.非闪烁状态
    {
        //2.1.开启状态
        if( m_lamp.lamp[id].cmd == LAMP_ON )
        {
            //2.1.1.当前关闭状态
            if( m_lamp.lamp[id].onoff == 0 )
            {
                m_lamp.buff |= m_lampbit[id]; 
                m_lamp.lamp[id].onoff = !m_lamp.lamp[id].onoff;
            }
        }
        else//2.2.关闭状态
        {
            //2.2.1.当前开启状体
            if( m_lamp.lamp[id].onoff == 1 )
            {
                m_lamp.buff &= ~m_lampbit[id]; 
                m_lamp.lamp[id].onoff = !m_lamp.lamp[id].onoff;
            }
        }
    }
}

/*******************************************************************************
* 函数名称  : lamp_process
* 描    述  : 
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void lamp_process(void)
{
  	unsigned char i;
	
    //1.6个指示灯处理
	for( i = 0; i < LAMP_MAX; i++ )
		lamp_single(i);
    //2.写入数据
    if( m_lamp.buff != m_lamp.prebuff )
    {
        tm1650_WriteDisplay( ID_NIXIE, ADDR_LAMP, m_lamp.buff );
        m_lamp.prebuff = m_lamp.buff; 
    }
}

/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
