/************* (C) COPYRIGHT 2010 杭州卡丽智能科技股份有限公司 *****************
* 文件名称:    sysctrl.c
* 描    述:    系统控制.C文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/6/6

* 修改历史:    时间        内容                  修改者
               2017/6/6:  文件创建              王晶东
*******************************************************************************/
//includes
#include "sysctrl.h"
#include "fanctrl.h"
#include "ledctrl.h"
#include "touch.h"
#include "buzzer.h"
#include "rtc.h"
#include "usart.h"
#include "comm.h"
#include "stdio.h"
#include "light.h"
#include "remote.h"
#include "lamp.h"
#include "nixie.h"
#include "remote.h"
#include "fanctrl.h"
#include "nixie.h"

//----local definitions---------------------------------------------------------
#define MAX_PM25_BUF 5

unsigned char  m_pm25_index = 0;
unsigned char  m_pm25_len = 0;
unsigned short m_pm25_buf[MAX_PM25_BUF];
unsigned short m_pm25_sum = 0;
unsigned char  m_runmode = 0;
unsigned short m_timedelay = 0;

/* Exported functions ------------------------------------------------------- */
void display_enable( unsigned char status, unsigned char lamponoff );

/*******************************************************************************
* 函数名称  : average_pm25
* 描    述  : pm2.5取平均值
* 输    入  : None
* 输    出  : None
* 返    回  : 1：定时滤波成功 0：未到定时滤波
*******************************************************************************/
unsigned char average_pm25( unsigned short value )
{
  	unsigned char rt = 0;
	
	//1.pm2.5传感器运行状态下
	if( m_DevData.pm25status || m_DevData.net_automode )
	{
	  	//1.1未到滤波长度
		if( m_pm25_len < MAX_PM25_BUF )
		{
			m_pm25_buf[ m_pm25_index ] = value;
			m_pm25_sum += value;
			m_pm25_index++;  
			m_pm25_len++;
			
			rt = 0;
		}
		else
		{
			if( m_pm25_index == MAX_PM25_BUF )
				m_pm25_index = 0;
			//重计累计和
			m_pm25_sum = m_pm25_sum - m_pm25_buf[ m_pm25_index ] + value ;
			m_pm25_buf[ m_pm25_index ] = value;
			m_pm25_index++;  
			//自动标志位清零
			m_DevData.net_automode = 0;
			
			rt = 1;
		}
	}
	else  //2.pm2.5传感器未在运行状态下，滤波数据清零
	{
		if( m_pm25_len > 0 )
		{
			m_pm25_index = 0;
			m_pm25_sum = 0;
			m_pm25_len = 0;
		} 
		rt = 0;
	}

	return rt;
}

/*******************************************************************************
* 函数名称  : level_search
* 描    述  : 档位查询
* 输    入  : week:所在星期
* 输    出  : None
* 返    回  : 0:关机 1/2/3:开机 0xffff:继续查询
*******************************************************************************/
unsigned short level_search( unsigned char week )
{
  	unsigned short level = 0; 
	unsigned short ontime, offtime, curtime;
	
	ontime = m_SetData.onofftime[week][0];
	offtime = m_SetData.onofftime[week][1];
	curtime = m_time_now.tm_hour*100+m_time_now.tm_min;
	//开机时间早于关机时间
	if( ontime < offtime )
	{
	  	if( curtime >= offtime )  //关机
			level = 0;
		else if( curtime >= ontime )  //开机
		  	level = m_SetData.setmode[week]&0x002F;
		else  //继续查询
	  		level = 0xffff;
	}
	else
	{
	  	if( curtime >= ontime )  //开机
			level = m_SetData.setmode[week]&0x002F;
		else if( curtime >= offtime )  //关机
		  	level = 0;
		else  //继续查询
	  		level = 0xffff;
	}
	
	return level;
}

/*******************************************************************************
* 函数名称  : level_action
* 描    述  : 档位执行
* 输    入  : None
* 输    出  : None
* 返    回  : 外机档位
*******************************************************************************/
unsigned char level_action( void )
{
	unsigned char week, repeat, curweek, rt;
    unsigned short level;
	
	rt = 0;
	//1.重复性查询
	for( week = 0; week < 7; week++ )
	{
	  	if( m_SetData.onofftime[week][0] != m_SetData.onofftime[week][1])
			repeat |= (1<<week);
	}
	//2.判断开关机
	curweek = m_time_now.tm_wday; 
	week = 7;
	while( week-- )
	{
	  	//有效时间段
	  	if( ((repeat >> curweek) & 0x01) == 0x01)
		{
		  	//执行ON or OFF
		  	level = level_search( curweek );
			if( level != 0xffff ) //开关机动作
			{
			  	rt = level;
			  	break;
			}
			else //继续查询
			{
				if( !curweek )
					curweek = 6;
				else
					curweek--;
			}
		}
		else //继续查询
		{
			if( !curweek )
				curweek = 6;
			else
			  	curweek--;
		}
	}
	
	return rt;
}

/*******************************************************************************
* 函数名称  : mode_process
* 描    述  : 模式控制函数
* 输    入  : value：当前pm2.5数值
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void mode_process( void )
{
    unsigned char i, mode, hour, min;
	unsigned char week1, tmode;
	unsigned short level, data;
    
    //定时、智能、远程锁定
    mode = (m_RunData.runmode & 0x30);
    tmode = (m_RunData.runmode & MODE_TIME);
    if( mode >= MODE_TIME )
    {
        //智能模式
        if( (m_RunData.runmode & MODE_INTEL) == MODE_INTEL )
        {
            data = m_RunData.pm25;
            //20s平均值
            if( average_pm25( data ) )
            {
                data = m_pm25_sum/MAX_PM25_BUF;
                //室内PM2.5指标优
                if( ((m_RunData.runmode & 0x0F) == 0 ) ||
                    (data <= (m_SetData.pm25[0]-WAVE_C_PM25)) )
                {
                    fan_ctrl( tmode|MODE_LEVEL1|MODE_INTEL );
                }
                else if( (data >= (m_SetData.pm25[0]+WAVE_C_PM25)) && 
                         (data <= (m_SetData.pm25[2]-WAVE_C_PM25)) )
                {
                    fan_ctrl( tmode|MODE_LEVEL2|MODE_INTEL );
                }
                else if( data >= (m_SetData.pm25[2]+WAVE_C_PM25) )
                {
                    fan_ctrl( tmode|MODE_LEVEL3|MODE_INTEL );
                }
            }
        }
        //定时模式
        if( (m_RunData.runmode & MODE_TIME) == MODE_TIME )
        {
            //手动定时
            if( !m_DevData.timemode )
            {
                //定时倒计时结束
                if( m_DevData.timecount == 1 )
                {
                    m_DevData.timecount = 0;
                    //开机状态，执行关机
                    if( (m_RunData.runmode & 0x000F) > 0 )
                    {
                        fan_ctrl( MODE_LEVEL0 );
                        //关闭显示
                        display_enable(0, 1);
                    }
                    //关机状态，执行开机
                    else
                    {
                        level = m_DevData.oldrunmode;
                        if( level > 0 )
                            fan_ctrl( level );  
                        else
                            fan_ctrl( MODE_LEVEL1 );
                    }
                }
            }
            else
            {
                week1 = m_time_now.tm_wday;
                //开关机时间不重合
                if( m_SetData.onofftime[week1][0] != m_SetData.onofftime[week1][1])
                {
                    for( i = 0; i < 2; i++  )
                    {
                        hour = m_SetData.onofftime[week1][i]%10000/100;
                        min = m_SetData.onofftime[week1][i]%100;
                        //定时时间有效
                        if( (hour == m_time_now.tm_hour) && (min == m_time_now.tm_min) )
                        {
                            //开机档位
                            level = m_SetData.setmode[week1]&0x002F;
                            //智能模式
                            if( (m_RunData.runmode & MODE_INTEL) == MODE_INTEL )
                                level = m_RunData.runmode&0x002F;
                            //定时开机时间到
                            if( i == 0 ) 
                                fan_ctrl( level|MODE_TIME );
                            else//定时关机时间到
                                fan_ctrl( MODE_LEVEL0|MODE_TIME );
                        }
                    }
                }
                //中间时间点自动判断
                if( m_timedelay > 1 )
                {
                    m_timedelay--;
                    if( m_timedelay == 1 )
                    {
                        //1.倒计时清零
                        m_timedelay = 0;
                        //2.查询档位
                        level = level_action();
                        //3.若为智能模式，提取档位
                        if( level == MODE_INTEL )
                            level = MODE_INTEL | (m_RunData.runmode&0x000F);  
                        //4.执行动作定时
                        fan_ctrl( MODE_TIME|level );
                    }
                }
            }
        }
    } 
}

/*******************************************************************************
* 函数名称  : PVD_init(void)
* 描    述  : 低电压保护初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void pvd_init( void )
{
  	EXTI_InitTypeDef EXTI_InitStructure;
    
    EXTI_StructInit(&EXTI_InitStructure);  
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    PWR_PVDLevelConfig(PWR_PVDLevel_2V9);
    PWR_PVDCmd(ENABLE);
}

/*******************************************************************************
* 函数名称  : time_process(void)
* 描    述  : 秒任务处理
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void time_process( void )
{
    //1.5min时间
    if( m_DevData.devtim_5min > 1 )m_DevData.devtim_5min--;
    //2.遥控学习倒计时
    if( m_remote.studytimes > 1 )m_remote.studytimes--;
    //3.风机启动延时
    if( m_fan.time > 0 )m_fan.time--;
    //4.定时时间倒计时
    if( m_DevData.timeactived > 0 )
        m_DevData.timeactived--;
    else
    {
        if( m_DevData.timecount > 1 )
            m_DevData.timecount--;
    }
    //5.背光倒计时
    if( m_DevData.blink > 0 )m_DevData.blink--;
    //6.传感器激活状态
    if(m_DevData.actived > 0 )m_DevData.actived--;
    //7.遥控学习
    if( m_remote.studysucc > 1 )m_remote.studysucc--;
    //8.滤芯剩余百分比显示倒计时
    if( m_DevData.filtertime > 0 )m_DevData.filtertime--;
    //9.定时秒处理
    if( m_timedelay > 1)m_timedelay--;
	//2.wifi通信处理
	/*if( m_NetData.real_time  > 0 )m_NetData.real_time--;
	if( m_NetData.heart_time > 0 )m_NetData.heart_time--;
	if( m_NetData.real_send > 1 )m_NetData.real_send--;
    if( m_NetData.para_time > 0 )m_NetData.para_time--;
    if( m_NetData.status_time > 0 )m_NetData.status_time--;*/
}

/*******************************************************************************
* 函数名称  : get_display
* 描    述  : None
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void get_display( void )
{
    unsigned char str[7] = { " .0h"};
    unsigned char blink = 0;
    unsigned int tm;
    
    //1.背光开启和开机状态
    if( m_DevData.lcdonoff && (m_RunData.runmode > 0) )
    {
        if( m_DevData.filtertime > 0 )
        {
            if( m_RunData.filter == 100 )
                num2str(str, 0x30|ZS1, m_RunData.filter); 
            else
            {
                str[0] = 'F';
                num2str(&str[1], 0x20|ZS1, m_RunData.filter); 
            }
        }
        //1.显示定时时间
        else if( m_DevData.timeactived > 0 )
        {
            //1.1.数值
            tm = m_DevData.timecount;
            str[0] = tm/TIME_HOUR+0x30;
            str[2] = '0';
            tm %= TIME_HOUR;
            if( tm > 0 )
                str[2] = tm*10/TIME_HOUR+0x30;
            blink = 1;  
        }
        //2.显示PM2.5数据
        else
        {
            //2.1.数值
            num2str(str, 0x30|ZS0, m_RunData.pm25);
        }
        nixie_set(str, 1, 7, blink);
    }
}

/*******************************************************************************
* 函数名称  : display_init(void)
* 描    述  : 显示初始化处理
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void display_init( void )
{
    //1.数码管初始化
    nixie_init();
    //2.指示灯初始化
    lamp_init();
}
/*******************************************************************************
* 函数名称  : display_process(void)
* 描    述  : 显示任务处理
* 输    入  : status:0-显示关（数码管和背光灯），1-显示开，
              lamponoff:0-背光灯不复位，1- 背光灯复位              
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void display_enable( unsigned char status, unsigned char lamponoff )
{  
    //1.背光清零
    if( !status )
        m_DevData.blink = 0;
    //2.关显示
    if( m_DevData.lcdonoff != status )
    {
        m_DevData.lcdonoff = status;
        //1.关闭数码显示
        nixie_set("", status, 7, 0 );
    }
    //3.关闭lamp显示
    if( lamponoff )
        lamp_off( LAMP_ALL );
}

/*******************************************************************************
* 函数名称  : display_process(void)
* 描    述  : 显示任务处理
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void display_process( void )
{
    //0.获取显示数据
    get_display();
    //1.数码管显示    
    nixie_display();
    //2.指示灯显示
    lamp_process();
}

/*******************************************************************************
* 函数名称  : blink_process(void)
* 描    述  : 背光任务处理
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void blink_process( void )
{
    unsigned char lamp;
    
    //1.开机状态才会有背光自动控制
    if( m_RunData.runmode > 0 )
    {
        //1.1.黑暗环境关闭光源
        if( m_DevData.blink == 0 )
        {
            //2.1.关闭背光
            if( !m_DevData.lightonoff )
                display_enable(0, 1);
            //2.光照环境开启光源
            else
                display_enable(1, 0);
        }
    }
    else
    {
        if( (!m_DevData.blink) && m_DevData.lcdonoff )
            m_DevData.lcdonoff = 0;
    }
    //2.模式指示灯
    if( m_DevData.lcdonoff )
    {
        if(  m_runmode != m_RunData.runmode )
        {
            if( m_RunData.runmode > 0 )
            {
                //2.2.1.仅是定时开机
                if( (m_RunData.runmode == MODE_TIME) && ( !m_DevData.timemode ) )
                    lamp_on( LAMP_TIME, LED_ON, 0, 0, 0, 1 );
                else
                {
                    lamp_on( LAMP_ONOFF, LED_ON, 0, 0, 0, 1 );
                    //2.2.1.定时模式
                    if( (m_RunData.runmode & MODE_TIME) == MODE_TIME )
                        lamp_on( LAMP_TIME, LED_ON, 0, 0, 0, 0 );
                    //智能模式
                    if( (m_RunData.runmode & MODE_INTEL) == MODE_INTEL )
                        lamp_on( LAMP_AUTO, LED_ON, 0, 0, 0, 0 );
                    //2.2.2.档位模式
                    else
                    {
                        lamp = (m_RunData.runmode & 0x0003)+2;
                        lamp_on( lamp, LED_ON, 0, 0, 0, 0 );
                    }
                } 
            }
            m_runmode = m_RunData.runmode;
        }  
    }
    else
    {
        if( m_runmode != 0xff)
            m_runmode = 0xff;
    }
}

/*******************************************************************************
* 函数名称  : func_studyen
* 描    述  : 学习模式使能
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void func_studyen( unsigned char status )
{
    //1.学习模式
    if( status == STUDY )
    {
        remote_studyen(1);
        m_DevData.setmode = STUDY;
        lamp_on( LAMP_ONOFF, LED_BLINK, 500, 500, 0xff, 0 );
        buzzer_on( 0, MUSIC_STUDY );
    }
    //2.退出学习模式
    else
    {
        remote_studyen(0);
        m_DevData.setmode = NORMAL;
        //2.1.开机状态，点亮开机键
        if( ( m_RunData.runmode > 0 ) && (m_RunData.runmode  != MODE_TIME) )
            lamp_on( LAMP_ONOFF, LED_ON, 0, 0, 0xff, 0 );
        else
            lamp_off( LAMP_ONOFF );
    } 
}

/*******************************************************************************
* 函数名称  : func_normal
* 描    述  : 正常模式按键处理
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void func_normal( unsigned char runmode )
{
    unsigned char mode = 0xff;
    unsigned char ytm, level;
    unsigned char onoff = 0;

    ytm = 0;
    //1.开关机
    if( runmode == MODE_LEVEL0 )
    {
        //1.1.肯定关机
        if( (!m_RunData.runmode) )
            onoff = 0;
        //1.2.再判断
        else
        {
            //1.2.1.关机状态，定时开机模式
            if( (!m_DevData.timemode) && ( m_RunData.runmode == MODE_TIME) )
                onoff = 0;
            //1.2.2.开关机状态
            else
                onoff = 1;
        }
        //1.3.开机处理
        if( !onoff )
        {
            level = m_DevData.oldrunmode;
            if( level != 0 )
            {
                //1.3.1.定时智能模式
                if( (level & 0x0030) == 0x0030 )
                    level = 0x0030;
                fan_ctrl( level );
            }
            else
                fan_ctrl( MODE_LEVEL1 );//设置档位
            //1.3.1.开机音效
            mode = MUSIC_ON;
            //1.3.2.定时数据清零
            m_DevData.lcdonoff = 1;
        }
        //1.4.关机处理
        else
        {
            //1.4.1.设置关机
            fan_ctrl( MODE_LEVEL0 );
            display_enable(0, 1);
            //1.4.2.关机音效
            mode = MUSIC_OFF;
        }
    }
    //2.开机状态且非预约开机状态
    else if( ( m_RunData.runmode > 0 ) && (m_RunData.runmode != MODE_TIME) )
    {
        //2.智能模式
        if( runmode == MODE_INTEL )
        {
            mode = MUSIC_AUTO;
            level = m_RunData.runmode&0x0f;
            fan_ctrl( MODE_INTEL|level );
        }
        //3.定时模式
        else if( runmode == MODE_TIME ) 
        {
            mode = MUSIC_TIME;
            //3.1.非联网状态
            if( !m_NetData.connect )
            {
                ytm = 1;
                m_DevData.timemode = 0;
            }
            //3.2.联网状态
            else
            {
                level = m_RunData.runmode&0x2f;
                fan_ctrl( MODE_TIME|level );
                m_DevData.timemode = 1;
            }
        }
        //4.一档模式,切换到一档模式
        else if( runmode == MODE_LEVEL1 ) 
        {
            mode = MUSIC_LEVEL1; 
            fan_ctrl( MODE_LEVEL1 );
        }
        //5.二档模式,切换到二档模式
        else if( runmode == MODE_LEVEL2 ) 
        {
            mode = MUSIC_LEVEL2;
            fan_ctrl( MODE_LEVEL2 );
        }
        //6.三档模式,换到三档模式
        else if( runmode == MODE_LEVEL3 )
        {
            mode = MUSIC_LEVEL3;
            fan_ctrl( MODE_LEVEL3 );
        } 
        //7.微调模式,切换到微调模式
        else
        {  
            //2.6.1.在非智能或定时模式
            if( !(m_RunData.runmode & 0x0030) )
            {
                mode = MUSIC_ADJUST;
                fan_ctrl( m_RunData.runmode|MODE_ADJUST );
            }
        }
    }
    //3.关机状态或预约开机状态
    else
    {
        //2.2.1.定时开机设置
        if( (runmode == MODE_TIME) &&(!m_NetData.connect) )
        {
            mode = MUSIC_TIME;
            ytm = 1;
            m_DevData.timemode = 0;
        }
        else
        {
           lamp_on( LAMP_ONOFF, LED_BLINK, 200, 200, 3, 0 );
           mode = MUSIC_LONG;
        }
    }
    //4.模式音效
    if( mode != 0xff )
        buzzer_on( 0, mode );
    //5.定时控制
    if( ytm )
    {
        //5.1.激活状态或其他模式更新
        if( m_DevData.timeactived > 0 || ((m_RunData.runmode&MODE_TIME) != MODE_TIME) )
        {
            m_DevData.timecount -= m_DevData.timecount%TIME_STEP;
            if( m_DevData.timecount < TIME_MAX )
            {
                m_DevData.timecount += TIME_STEP;
                m_DevData.timeactived = TIME_ACTIVED;
                fan_ctrl( MODE_TIME|m_RunData.runmode );
            }
            else
            {
                m_DevData.timecount = 0;
                m_DevData.timeactived = 0;
                if( m_RunData.runmode == MODE_TIME )
                    display_enable(0, 1);
                fan_ctrl( m_RunData.runmode & (~MODE_TIME) );
            } 
        }
        else
        {
            m_DevData.timeactived = TIME_ACTIVED;
        }
    }
    //6.非定时模式数据清零
    if( (runmode != MODE_TIME) && ( m_RunData.runmode != MODE_TIME ))
    {
        m_DevData.timecount = 0;
        m_DevData.timeactived = 0;
    }
}

/*******************************************************************************
* 函数名称  : mode_set
* 描    述  : 模式设置
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void mode_set( unsigned short status )
{
    unsigned char level;
    
    level = (status & 0x002f);
    if( (status & MODE_TIME) == MODE_TIME )
    {
        m_timedelay = TIME_DELAY;
        m_RunData.runmode = MODE_TIME|level;
    }
    else
    {
        m_timedelay = 0;
        fan_ctrl( status );
    }
}

/*******************************************************************************
* 函数名称  : func_study(void)
* 描    述  : 学习模式按键处理
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void func_study( void )
{
    //0.遥控器学习倒计时结束，恢复正常模式
    if( m_remote.studytimes == 1 )
    {
        remote_studyen(0);
        lamp_off( LAMP_ONOFF );
        buzzer_on( 0, MUSIC_LONG );
        //切换到正常模式
        m_DevData.setmode = NORMAL;
    }
    //1.遥控学习模式
    remote_study();
}

/*******************************************************************************
* 函数名称  : func_process(void)
* 描    述  : 功能任务处理
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void func_process( void )
{  
    unsigned char remote;
    unsigned char runmode = 0xff;
    
    //1.非学习模式
    if( m_DevData.setmode == NORMAL )
    {
        remote = m_remote.keyid;
        //1.1.有按键动作
        if( (m_key > T_None) || ( remote > REMOTE_NONE) )
        {
            //1.1.1.进入学习模式
            if( m_key == T_L_K4K5K6 )
            {
                func_studyen( STUDY );
            }
            //1.1.2.配网状态
            else if( m_key == T_L_K1 )
            {
                m_NetData.setnet = 1;
                buzzer_on( 0, MUSIC_LONG );
            }
            //1.1.3.显示滤芯剩余百分比
            else if( m_key == T_L_K2 )
            {
                m_DevData.filtertime = FILTER_TIME;
                buzzer_on( 0, MUSIC_LONG );
            }
            //1.1.3.其他模式
            else
            {
                //1.1.3.1.模式按钮分析
                if(( m_key == T_K1 ) || ( remote == REMOTE_ONOFF ))
                    runmode = MODE_LEVEL0;
                else if(( m_key == T_K2 ) || ( remote == REMOTE_AUTO ))
                    runmode = MODE_INTEL;
                else if(( m_key == T_K3 ) || ( remote == REMOTE_TIME ))
                    runmode = MODE_TIME;
                else if(( m_key == T_K4 ) || ( remote == REMOTE_LEVEL1 ))
                    runmode = MODE_LEVEL1;
                else if(( m_key == T_K5 ) || ( remote == REMOTE_LEVEL2 ))
                    runmode = MODE_LEVEL2;
                else if(( m_key == T_K6 ) || ( remote == REMOTE_LEVEL3 ))
                    runmode = MODE_LEVEL3;
                else if( remote == REMOTE_ADJUST )
                    runmode = MODE_ADJUST;  
                //1.1.3.2.模式按键 
                if( runmode != 0xff )
                    func_normal( runmode );
            }
            //1.1.4.键值清零
            m_key = T_None;
            m_remote.keyid = REMOTE_NONE;
        }
    }
    //2.学习模式
    else  
    {   
        //2.1.退出到正常模式
        if( (m_key == T_L_K4K5K6) || (m_remote.studysucc == 1) )
        {
            if( m_key == T_L_K4K5K6 )
                buzzer_on( 0, MUSIC_LONG );
            func_studyen( NORMAL );
            m_key = T_None;
        }
        //2.2.遥控器学习
        func_study();
    }
}

/*******************************************************************************
* 函数名称  : check_process(void)
* 描    述  : 模块自检处理
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
/*void check_process( void )
{
    unsigned short timeout;
    unsigned char str[4];
    unsigned char light, err = 0;
    
    //1.获取按键
    timeout = 200;
    while( timeout )
    {
        if( m_timer_10ms == 0 )
        { 
            touch_process();
            buzzer_process();
            timeout--;
            m_timer_10ms = 10;
        }
        if( m_key != T_None )
        {
            buzzer_off( BUZZER_0 ); 
            break;
        }
    }
    //2.判断是否进入自检状态
    if( (m_key == T_K3) || (m_key == T_L_K3) )
    {
        //2.1.发送自检指令
        Com_Send(0, m_cmd_check, 10, 0, 0 );
        m_DevData.check = 0xff;
        //2.2.准备接受
        timeout = 6;
        while( timeout )
        {
            if( m_DevData.devtim_second )
            {
                sprintf( (char*)&str[0], " %d ", --timeout );
                display_check( str, 0 );
                m_DevData.devtim_second = 0;
            }
            ComNet_check( WIFI_UP, 10 );
            //2.2.1.光强度采样
            get_light();
            //2.2.2.接收到返回数据
            if( m_DevData.check != 0xff )
              break;   
        }
        //2.3.光传感器故障判断
        light = 1;
        if( (m_DevData.lightvalue >= 4000) || (m_DevData.lightvalue < 100) )
            light = 0;
        //2.4.接收超时，模块故障
        if( !timeout )
        {
            sprintf( (char*)&str[0], "%s", "out" );
            display_check( str, light );
            err = 1;
        }
        //2.5.成功接收
        else
        {
            //2.5.1.模块正常
            if( m_DevData.check )
            {
                sprintf( (char*)&str[0], "%s", "OK" );
                display_check( str, light );
            }
            else//2.5.2.模块故障
            {
                sprintf( (char*)&str[0], "%s", "Er" );
                display_check( str, light );
                err = 1;
            }
        }
        //2.6.模块错误时蜂鸣器响
        if( err || (light == 0) )
            buzzer_on( BUZZER_0, 100, 100, 0xff ); 
        //2.7.长按锁定键退出
        while( m_key != T_L_K3 )
        {
            if(  m_timer_10ms == 0 )
            {
                touch_process();
                buzzer_process();
                m_timer_10ms = 10;
            }
        }
    }
}*/