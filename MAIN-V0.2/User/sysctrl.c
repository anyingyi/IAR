/************* (C) COPYRIGHT 2010 ���ݿ������ܿƼ��ɷ����޹�˾ *****************
* �ļ�����:    sysctrl.c
* ��    ��:    ϵͳ����.C�ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/6/6

* �޸���ʷ:    ʱ��        ����                  �޸���
               2017/6/6:  �ļ�����              ������
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
* ��������  : average_pm25
* ��    ��  : pm2.5ȡƽ��ֵ
* ��    ��  : None
* ��    ��  : None
* ��    ��  : 1����ʱ�˲��ɹ� 0��δ����ʱ�˲�
*******************************************************************************/
unsigned char average_pm25( unsigned short value )
{
  	unsigned char rt = 0;
	
	//1.pm2.5����������״̬��
	if( m_DevData.pm25status || m_DevData.net_automode )
	{
	  	//1.1δ���˲�����
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
			//�ؼ��ۼƺ�
			m_pm25_sum = m_pm25_sum - m_pm25_buf[ m_pm25_index ] + value ;
			m_pm25_buf[ m_pm25_index ] = value;
			m_pm25_index++;  
			//�Զ���־λ����
			m_DevData.net_automode = 0;
			
			rt = 1;
		}
	}
	else  //2.pm2.5������δ������״̬�£��˲���������
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
* ��������  : level_search
* ��    ��  : ��λ��ѯ
* ��    ��  : week:��������
* ��    ��  : None
* ��    ��  : 0:�ػ� 1/2/3:���� 0xffff:������ѯ
*******************************************************************************/
unsigned short level_search( unsigned char week )
{
  	unsigned short level = 0; 
	unsigned short ontime, offtime, curtime;
	
	ontime = m_SetData.onofftime[week][0];
	offtime = m_SetData.onofftime[week][1];
	curtime = m_time_now.tm_hour*100+m_time_now.tm_min;
	//����ʱ�����ڹػ�ʱ��
	if( ontime < offtime )
	{
	  	if( curtime >= offtime )  //�ػ�
			level = 0;
		else if( curtime >= ontime )  //����
		  	level = m_SetData.setmode[week]&0x002F;
		else  //������ѯ
	  		level = 0xffff;
	}
	else
	{
	  	if( curtime >= ontime )  //����
			level = m_SetData.setmode[week]&0x002F;
		else if( curtime >= offtime )  //�ػ�
		  	level = 0;
		else  //������ѯ
	  		level = 0xffff;
	}
	
	return level;
}

/*******************************************************************************
* ��������  : level_action
* ��    ��  : ��λִ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : �����λ
*******************************************************************************/
unsigned char level_action( void )
{
	unsigned char week, repeat, curweek, rt;
    unsigned short level;
	
	rt = 0;
	//1.�ظ��Բ�ѯ
	for( week = 0; week < 7; week++ )
	{
	  	if( m_SetData.onofftime[week][0] != m_SetData.onofftime[week][1])
			repeat |= (1<<week);
	}
	//2.�жϿ��ػ�
	curweek = m_time_now.tm_wday; 
	week = 7;
	while( week-- )
	{
	  	//��Чʱ���
	  	if( ((repeat >> curweek) & 0x01) == 0x01)
		{
		  	//ִ��ON or OFF
		  	level = level_search( curweek );
			if( level != 0xffff ) //���ػ�����
			{
			  	rt = level;
			  	break;
			}
			else //������ѯ
			{
				if( !curweek )
					curweek = 6;
				else
					curweek--;
			}
		}
		else //������ѯ
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
* ��������  : mode_process
* ��    ��  : ģʽ���ƺ���
* ��    ��  : value����ǰpm2.5��ֵ
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void mode_process( void )
{
    unsigned char i, mode, hour, min;
	unsigned char week1, tmode;
	unsigned short level, data;
    
    //��ʱ�����ܡ�Զ������
    mode = (m_RunData.runmode & 0x30);
    tmode = (m_RunData.runmode & MODE_TIME);
    if( mode >= MODE_TIME )
    {
        //����ģʽ
        if( (m_RunData.runmode & MODE_INTEL) == MODE_INTEL )
        {
            data = m_RunData.pm25;
            //20sƽ��ֵ
            if( average_pm25( data ) )
            {
                data = m_pm25_sum/MAX_PM25_BUF;
                //����PM2.5ָ����
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
        //��ʱģʽ
        if( (m_RunData.runmode & MODE_TIME) == MODE_TIME )
        {
            //�ֶ���ʱ
            if( !m_DevData.timemode )
            {
                //��ʱ����ʱ����
                if( m_DevData.timecount == 1 )
                {
                    m_DevData.timecount = 0;
                    //����״̬��ִ�йػ�
                    if( (m_RunData.runmode & 0x000F) > 0 )
                    {
                        fan_ctrl( MODE_LEVEL0 );
                        //�ر���ʾ
                        display_enable(0, 1);
                    }
                    //�ػ�״̬��ִ�п���
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
                //���ػ�ʱ�䲻�غ�
                if( m_SetData.onofftime[week1][0] != m_SetData.onofftime[week1][1])
                {
                    for( i = 0; i < 2; i++  )
                    {
                        hour = m_SetData.onofftime[week1][i]%10000/100;
                        min = m_SetData.onofftime[week1][i]%100;
                        //��ʱʱ����Ч
                        if( (hour == m_time_now.tm_hour) && (min == m_time_now.tm_min) )
                        {
                            //������λ
                            level = m_SetData.setmode[week1]&0x002F;
                            //����ģʽ
                            if( (m_RunData.runmode & MODE_INTEL) == MODE_INTEL )
                                level = m_RunData.runmode&0x002F;
                            //��ʱ����ʱ�䵽
                            if( i == 0 ) 
                                fan_ctrl( level|MODE_TIME );
                            else//��ʱ�ػ�ʱ�䵽
                                fan_ctrl( MODE_LEVEL0|MODE_TIME );
                        }
                    }
                }
                //�м�ʱ����Զ��ж�
                if( m_timedelay > 1 )
                {
                    m_timedelay--;
                    if( m_timedelay == 1 )
                    {
                        //1.����ʱ����
                        m_timedelay = 0;
                        //2.��ѯ��λ
                        level = level_action();
                        //3.��Ϊ����ģʽ����ȡ��λ
                        if( level == MODE_INTEL )
                            level = MODE_INTEL | (m_RunData.runmode&0x000F);  
                        //4.ִ�ж�����ʱ
                        fan_ctrl( MODE_TIME|level );
                    }
                }
            }
        }
    } 
}

/*******************************************************************************
* ��������  : PVD_init(void)
* ��    ��  : �͵�ѹ������ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
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
* ��������  : time_process(void)
* ��    ��  : ��������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void time_process( void )
{
    //1.5minʱ��
    if( m_DevData.devtim_5min > 1 )m_DevData.devtim_5min--;
    //2.ң��ѧϰ����ʱ
    if( m_remote.studytimes > 1 )m_remote.studytimes--;
    //3.���������ʱ
    if( m_fan.time > 0 )m_fan.time--;
    //4.��ʱʱ�䵹��ʱ
    if( m_DevData.timeactived > 0 )
        m_DevData.timeactived--;
    else
    {
        if( m_DevData.timecount > 1 )
            m_DevData.timecount--;
    }
    //5.���⵹��ʱ
    if( m_DevData.blink > 0 )m_DevData.blink--;
    //6.����������״̬
    if(m_DevData.actived > 0 )m_DevData.actived--;
    //7.ң��ѧϰ
    if( m_remote.studysucc > 1 )m_remote.studysucc--;
    //8.��оʣ��ٷֱ���ʾ����ʱ
    if( m_DevData.filtertime > 0 )m_DevData.filtertime--;
    //9.��ʱ�봦��
    if( m_timedelay > 1)m_timedelay--;
	//2.wifiͨ�Ŵ���
	/*if( m_NetData.real_time  > 0 )m_NetData.real_time--;
	if( m_NetData.heart_time > 0 )m_NetData.heart_time--;
	if( m_NetData.real_send > 1 )m_NetData.real_send--;
    if( m_NetData.para_time > 0 )m_NetData.para_time--;
    if( m_NetData.status_time > 0 )m_NetData.status_time--;*/
}

/*******************************************************************************
* ��������  : get_display
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void get_display( void )
{
    unsigned char str[7] = { " .0h"};
    unsigned char blink = 0;
    unsigned int tm;
    
    //1.���⿪���Ϳ���״̬
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
        //1.��ʾ��ʱʱ��
        else if( m_DevData.timeactived > 0 )
        {
            //1.1.��ֵ
            tm = m_DevData.timecount;
            str[0] = tm/TIME_HOUR+0x30;
            str[2] = '0';
            tm %= TIME_HOUR;
            if( tm > 0 )
                str[2] = tm*10/TIME_HOUR+0x30;
            blink = 1;  
        }
        //2.��ʾPM2.5����
        else
        {
            //2.1.��ֵ
            num2str(str, 0x30|ZS0, m_RunData.pm25);
        }
        nixie_set(str, 1, 7, blink);
    }
}

/*******************************************************************************
* ��������  : display_init(void)
* ��    ��  : ��ʾ��ʼ������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void display_init( void )
{
    //1.����ܳ�ʼ��
    nixie_init();
    //2.ָʾ�Ƴ�ʼ��
    lamp_init();
}
/*******************************************************************************
* ��������  : display_process(void)
* ��    ��  : ��ʾ������
* ��    ��  : status:0-��ʾ�أ�����ܺͱ���ƣ���1-��ʾ����
              lamponoff:0-����Ʋ���λ��1- ����Ƹ�λ              
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void display_enable( unsigned char status, unsigned char lamponoff )
{  
    //1.��������
    if( !status )
        m_DevData.blink = 0;
    //2.����ʾ
    if( m_DevData.lcdonoff != status )
    {
        m_DevData.lcdonoff = status;
        //1.�ر�������ʾ
        nixie_set("", status, 7, 0 );
    }
    //3.�ر�lamp��ʾ
    if( lamponoff )
        lamp_off( LAMP_ALL );
}

/*******************************************************************************
* ��������  : display_process(void)
* ��    ��  : ��ʾ������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void display_process( void )
{
    //0.��ȡ��ʾ����
    get_display();
    //1.�������ʾ    
    nixie_display();
    //2.ָʾ����ʾ
    lamp_process();
}

/*******************************************************************************
* ��������  : blink_process(void)
* ��    ��  : ����������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void blink_process( void )
{
    unsigned char lamp;
    
    //1.����״̬�Ż��б����Զ�����
    if( m_RunData.runmode > 0 )
    {
        //1.1.�ڰ������رչ�Դ
        if( m_DevData.blink == 0 )
        {
            //2.1.�رձ���
            if( !m_DevData.lightonoff )
                display_enable(0, 1);
            //2.���ջ���������Դ
            else
                display_enable(1, 0);
        }
    }
    else
    {
        if( (!m_DevData.blink) && m_DevData.lcdonoff )
            m_DevData.lcdonoff = 0;
    }
    //2.ģʽָʾ��
    if( m_DevData.lcdonoff )
    {
        if(  m_runmode != m_RunData.runmode )
        {
            if( m_RunData.runmode > 0 )
            {
                //2.2.1.���Ƕ�ʱ����
                if( (m_RunData.runmode == MODE_TIME) && ( !m_DevData.timemode ) )
                    lamp_on( LAMP_TIME, LED_ON, 0, 0, 0, 1 );
                else
                {
                    lamp_on( LAMP_ONOFF, LED_ON, 0, 0, 0, 1 );
                    //2.2.1.��ʱģʽ
                    if( (m_RunData.runmode & MODE_TIME) == MODE_TIME )
                        lamp_on( LAMP_TIME, LED_ON, 0, 0, 0, 0 );
                    //����ģʽ
                    if( (m_RunData.runmode & MODE_INTEL) == MODE_INTEL )
                        lamp_on( LAMP_AUTO, LED_ON, 0, 0, 0, 0 );
                    //2.2.2.��λģʽ
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
* ��������  : func_studyen
* ��    ��  : ѧϰģʽʹ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void func_studyen( unsigned char status )
{
    //1.ѧϰģʽ
    if( status == STUDY )
    {
        remote_studyen(1);
        m_DevData.setmode = STUDY;
        lamp_on( LAMP_ONOFF, LED_BLINK, 500, 500, 0xff, 0 );
        buzzer_on( 0, MUSIC_STUDY );
    }
    //2.�˳�ѧϰģʽ
    else
    {
        remote_studyen(0);
        m_DevData.setmode = NORMAL;
        //2.1.����״̬������������
        if( ( m_RunData.runmode > 0 ) && (m_RunData.runmode  != MODE_TIME) )
            lamp_on( LAMP_ONOFF, LED_ON, 0, 0, 0xff, 0 );
        else
            lamp_off( LAMP_ONOFF );
    } 
}

/*******************************************************************************
* ��������  : func_normal
* ��    ��  : ����ģʽ��������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void func_normal( unsigned char runmode )
{
    unsigned char mode = 0xff;
    unsigned char ytm, level;
    unsigned char onoff = 0;

    ytm = 0;
    //1.���ػ�
    if( runmode == MODE_LEVEL0 )
    {
        //1.1.�϶��ػ�
        if( (!m_RunData.runmode) )
            onoff = 0;
        //1.2.���ж�
        else
        {
            //1.2.1.�ػ�״̬����ʱ����ģʽ
            if( (!m_DevData.timemode) && ( m_RunData.runmode == MODE_TIME) )
                onoff = 0;
            //1.2.2.���ػ�״̬
            else
                onoff = 1;
        }
        //1.3.��������
        if( !onoff )
        {
            level = m_DevData.oldrunmode;
            if( level != 0 )
            {
                //1.3.1.��ʱ����ģʽ
                if( (level & 0x0030) == 0x0030 )
                    level = 0x0030;
                fan_ctrl( level );
            }
            else
                fan_ctrl( MODE_LEVEL1 );//���õ�λ
            //1.3.1.������Ч
            mode = MUSIC_ON;
            //1.3.2.��ʱ��������
            m_DevData.lcdonoff = 1;
        }
        //1.4.�ػ�����
        else
        {
            //1.4.1.���ùػ�
            fan_ctrl( MODE_LEVEL0 );
            display_enable(0, 1);
            //1.4.2.�ػ���Ч
            mode = MUSIC_OFF;
        }
    }
    //2.����״̬�ҷ�ԤԼ����״̬
    else if( ( m_RunData.runmode > 0 ) && (m_RunData.runmode != MODE_TIME) )
    {
        //2.����ģʽ
        if( runmode == MODE_INTEL )
        {
            mode = MUSIC_AUTO;
            level = m_RunData.runmode&0x0f;
            fan_ctrl( MODE_INTEL|level );
        }
        //3.��ʱģʽ
        else if( runmode == MODE_TIME ) 
        {
            mode = MUSIC_TIME;
            //3.1.������״̬
            if( !m_NetData.connect )
            {
                ytm = 1;
                m_DevData.timemode = 0;
            }
            //3.2.����״̬
            else
            {
                level = m_RunData.runmode&0x2f;
                fan_ctrl( MODE_TIME|level );
                m_DevData.timemode = 1;
            }
        }
        //4.һ��ģʽ,�л���һ��ģʽ
        else if( runmode == MODE_LEVEL1 ) 
        {
            mode = MUSIC_LEVEL1; 
            fan_ctrl( MODE_LEVEL1 );
        }
        //5.����ģʽ,�л�������ģʽ
        else if( runmode == MODE_LEVEL2 ) 
        {
            mode = MUSIC_LEVEL2;
            fan_ctrl( MODE_LEVEL2 );
        }
        //6.����ģʽ,��������ģʽ
        else if( runmode == MODE_LEVEL3 )
        {
            mode = MUSIC_LEVEL3;
            fan_ctrl( MODE_LEVEL3 );
        } 
        //7.΢��ģʽ,�л���΢��ģʽ
        else
        {  
            //2.6.1.�ڷ����ܻ�ʱģʽ
            if( !(m_RunData.runmode & 0x0030) )
            {
                mode = MUSIC_ADJUST;
                fan_ctrl( m_RunData.runmode|MODE_ADJUST );
            }
        }
    }
    //3.�ػ�״̬��ԤԼ����״̬
    else
    {
        //2.2.1.��ʱ��������
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
    //4.ģʽ��Ч
    if( mode != 0xff )
        buzzer_on( 0, mode );
    //5.��ʱ����
    if( ytm )
    {
        //5.1.����״̬������ģʽ����
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
    //6.�Ƕ�ʱģʽ��������
    if( (runmode != MODE_TIME) && ( m_RunData.runmode != MODE_TIME ))
    {
        m_DevData.timecount = 0;
        m_DevData.timeactived = 0;
    }
}

/*******************************************************************************
* ��������  : mode_set
* ��    ��  : ģʽ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
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
* ��������  : func_study(void)
* ��    ��  : ѧϰģʽ��������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void func_study( void )
{
    //0.ң����ѧϰ����ʱ�������ָ�����ģʽ
    if( m_remote.studytimes == 1 )
    {
        remote_studyen(0);
        lamp_off( LAMP_ONOFF );
        buzzer_on( 0, MUSIC_LONG );
        //�л�������ģʽ
        m_DevData.setmode = NORMAL;
    }
    //1.ң��ѧϰģʽ
    remote_study();
}

/*******************************************************************************
* ��������  : func_process(void)
* ��    ��  : ����������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void func_process( void )
{  
    unsigned char remote;
    unsigned char runmode = 0xff;
    
    //1.��ѧϰģʽ
    if( m_DevData.setmode == NORMAL )
    {
        remote = m_remote.keyid;
        //1.1.�а�������
        if( (m_key > T_None) || ( remote > REMOTE_NONE) )
        {
            //1.1.1.����ѧϰģʽ
            if( m_key == T_L_K4K5K6 )
            {
                func_studyen( STUDY );
            }
            //1.1.2.����״̬
            else if( m_key == T_L_K1 )
            {
                m_NetData.setnet = 1;
                buzzer_on( 0, MUSIC_LONG );
            }
            //1.1.3.��ʾ��оʣ��ٷֱ�
            else if( m_key == T_L_K2 )
            {
                m_DevData.filtertime = FILTER_TIME;
                buzzer_on( 0, MUSIC_LONG );
            }
            //1.1.3.����ģʽ
            else
            {
                //1.1.3.1.ģʽ��ť����
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
                //1.1.3.2.ģʽ���� 
                if( runmode != 0xff )
                    func_normal( runmode );
            }
            //1.1.4.��ֵ����
            m_key = T_None;
            m_remote.keyid = REMOTE_NONE;
        }
    }
    //2.ѧϰģʽ
    else  
    {   
        //2.1.�˳�������ģʽ
        if( (m_key == T_L_K4K5K6) || (m_remote.studysucc == 1) )
        {
            if( m_key == T_L_K4K5K6 )
                buzzer_on( 0, MUSIC_LONG );
            func_studyen( NORMAL );
            m_key = T_None;
        }
        //2.2.ң����ѧϰ
        func_study();
    }
}

/*******************************************************************************
* ��������  : check_process(void)
* ��    ��  : ģ���Լ촦��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
/*void check_process( void )
{
    unsigned short timeout;
    unsigned char str[4];
    unsigned char light, err = 0;
    
    //1.��ȡ����
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
    //2.�ж��Ƿ�����Լ�״̬
    if( (m_key == T_K3) || (m_key == T_L_K3) )
    {
        //2.1.�����Լ�ָ��
        Com_Send(0, m_cmd_check, 10, 0, 0 );
        m_DevData.check = 0xff;
        //2.2.׼������
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
            //2.2.1.��ǿ�Ȳ���
            get_light();
            //2.2.2.���յ���������
            if( m_DevData.check != 0xff )
              break;   
        }
        //2.3.�⴫���������ж�
        light = 1;
        if( (m_DevData.lightvalue >= 4000) || (m_DevData.lightvalue < 100) )
            light = 0;
        //2.4.���ճ�ʱ��ģ�����
        if( !timeout )
        {
            sprintf( (char*)&str[0], "%s", "out" );
            display_check( str, light );
            err = 1;
        }
        //2.5.�ɹ�����
        else
        {
            //2.5.1.ģ������
            if( m_DevData.check )
            {
                sprintf( (char*)&str[0], "%s", "OK" );
                display_check( str, light );
            }
            else//2.5.2.ģ�����
            {
                sprintf( (char*)&str[0], "%s", "Er" );
                display_check( str, light );
                err = 1;
            }
        }
        //2.6.ģ�����ʱ��������
        if( err || (light == 0) )
            buzzer_on( BUZZER_0, 100, 100, 0xff ); 
        //2.7.�����������˳�
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