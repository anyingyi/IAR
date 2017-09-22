/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: lamp.c
* ��    ��: ������ʾ.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/8/23

* �޸���ʷ: ʱ��       ����           �޸���
            2017/8/23 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "lamp.h"
#include "data.h"

#define ADDR_LAMP   0x68

//LED����
typedef struct CLAMPCTRL_t
{
    unsigned char   lastcmd;      //��һLED��������
    unsigned char   onoff;        //LAMP״̬:0x00-��,0x01-��
    unsigned char   cmd;          //LAMP��������
    unsigned short  ontime;       //LAMP��ʱ��
    unsigned short  offtime;      //LAMP��ʱ��
    unsigned short  backontime;   //LAMP��ʱ�䱸��
    unsigned short  backofftime;  //LAMP��ʱ�䱸��
    unsigned char   times;        //�������:0x00����
}CLAMPCTRL;

typedef struct CLAMP_t
{
    unsigned char   buff;               //����
    unsigned char   prebuff;            //��һ������
    CLAMPCTRL       lamp[LAMP_MAX];     //������ʾ
}CLAMP;

CLAMP m_lamp;

const unsigned char m_lampbit[LAMP_MAX] = { BIT_LAMP1,      //lamp1bitλ
                                            BIT_LAMP2,      //lamp2bitλ 
                                            BIT_LAMP3,      //lamp3bitλ 
                                            BIT_LAMP4,      //lamp4bitλ 
                                            BIT_LAMP5,      //lamp5bitλ
                                            BIT_LAMP6 };    //lamp6bitλ

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
* ��������  : lamp_init
* ��    ��  : ��������Ƴ�ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void lamp_init(void)
{
    unsigned char i;
    
    //1.����ȫ������ʱ2��
    for( i = 0; i < LAMP_MAX; i++ )
    {
        lamp_on( i, LAMP_BLINK, 2000, 100, 1, 0);
    }  
}

/*******************************************************************************
* ��������  : lamp_on
* ��    ��  : �Ʋ�������
* ��    ��  : id:��id�ţ�onoff: ״̬ ��blink:��˸��־λ
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void lamp_on( unsigned char id, unsigned char cmd,
              unsigned short ontime, unsigned short offtime, 
              unsigned char times, unsigned char flag )
{
    //1.��������
    if( m_lamp.lamp[id].cmd != LAMP_BLINK )
        m_lamp.lamp[id].lastcmd = m_lamp.lamp[id].cmd;
    m_lamp.lamp[id].cmd = cmd;
    m_lamp.lamp[id].ontime = 0;
    m_lamp.lamp[id].offtime = 0;
    m_lamp.lamp[id].backontime = ontime/10;
    m_lamp.lamp[id].backofftime = offtime/10;
    m_lamp.lamp[id].times = times;
    //2.��ʾ����
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
* ��������  : lamp_off
* ��    ��  : �Ʋ�������
* ��    ��  : id:��id��
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void lamp_off( unsigned char id )
{
    unsigned char i;
    //1.�ر�����
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
    //2.�����ر�
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
* ��������  : lamp_single
* ��    ��  : ��������ƴ�����
* ��    ��  : id:�����id��
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void lamp_single( unsigned char id )
{
    //1.��˸״̬
    if(  m_lamp.lamp[id].cmd == LAMP_BLINK )
    {
        if( m_lamp.lamp[id].times > 0 )
        {
            //1.1.��ǰ�ر�״̬
            if( m_lamp.lamp[id].onoff == 0x00 )
            {
                //1.1.1.�ر�ʱ�䵽
                if( m_lamp.lamp[id].offtime == 0 )
                {
                    //1.1.1.1.д������
                    m_lamp.buff |= m_lampbit[id]; 
                    m_lamp.lamp[id].ontime = m_lamp.lamp[id].backontime;
                    m_lamp.lamp[id].onoff = 1;
                }
            }
            else //1.2.��ǰ����״̬
            {
                //1.2.1.����ʱ�䵽
                if( m_lamp.lamp[id].ontime == 0 )
                {
                    //1.2.1.1.д������
                    m_lamp.buff &= ~m_lampbit[id]; 
                    m_lamp.lamp[id].offtime = m_lamp.lamp[id].backofftime;
                    m_lamp.lamp[id].onoff = 0;
					//�����ݼ�
					if( m_lamp.lamp[id].times < 0xff )
						m_lamp.lamp[id].times--;
                }
            }
            if( m_lamp.lamp[id].ontime > 0 )m_lamp.lamp[id].ontime--;//10msΪ��λ�ݼ�
            if( m_lamp.lamp[id].offtime > 0 )m_lamp.lamp[id].offtime--;//10msΪ��λ�ݼ�
        }
        else
        {
            m_lamp.lamp[id].cmd = LAMP_OFF;
        }
    }
    else //2.����˸״̬
    {
        //2.1.����״̬
        if( m_lamp.lamp[id].cmd == LAMP_ON )
        {
            //2.1.1.��ǰ�ر�״̬
            if( m_lamp.lamp[id].onoff == 0 )
            {
                m_lamp.buff |= m_lampbit[id]; 
                m_lamp.lamp[id].onoff = !m_lamp.lamp[id].onoff;
            }
        }
        else//2.2.�ر�״̬
        {
            //2.2.1.��ǰ����״��
            if( m_lamp.lamp[id].onoff == 1 )
            {
                m_lamp.buff &= ~m_lampbit[id]; 
                m_lamp.lamp[id].onoff = !m_lamp.lamp[id].onoff;
            }
        }
    }
}

/*******************************************************************************
* ��������  : lamp_process
* ��    ��  : 
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void lamp_process(void)
{
  	unsigned char i;
	
    //1.6��ָʾ�ƴ���
	for( i = 0; i < LAMP_MAX; i++ )
		lamp_single(i);
    //2.д������
    if( m_lamp.buff != m_lamp.prebuff )
    {
        tm1650_WriteDisplay( ID_NIXIE, ADDR_LAMP, m_lamp.buff );
        m_lamp.prebuff = m_lamp.buff; 
    }
}

/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
