/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: touch.c
* ��    ��: ������.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/3

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/3 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "touch.h"
#include "data.h"
#include "ledctrl.h"
//��GPIOD->BSRR��16λ��ĳλΪ'1'�����Ӧ��I/O�˿���'1'������GPIOD->BSRR��16λ��ĳλΪ'0'�����Ӧ��I/O�˿ڲ��䡣0X00000040��ӦPA6
#define CLK_WRITE(m)	(GPIOA->BSRR = m?0x00000040:0x00400000) //ʱ��clk��� //�˿����ú�����Ĵ���-BSRR
#define DATA_READ		(GPIOA->IDR&0x0020)  //����data��ȡ 	//�˿�����������ЩλΪֻ����ֻ������(16λ)����ʽ������������ֵΪ��ӦI/O�ڵ�״̬
//��ЩλΪֻ����ֻ������(16λ)����ʽ������������ֵΪ��ӦI/O�ڵ�״̬.0X0020��ӦPA5
unsigned short m_key;
unsigned short m_old_key = 0;
unsigned short m_keytime = 0;

/*******************************************************************************
* ��������  : touch_int
* ��    ��  : ������ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void touch_init( void )
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	
	//clock�������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//data��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* ��������  : touch_get
* ��    ��  : ��ȡ������ֵ
* ��    ��  : None
* ��    ��  : None
* ��    ��  : ����ֵ
*******************************************************************************/
unsigned short touch_get( void )
{	
  	unsigned char i, n;
    unsigned short key, value = 0;
	
    key = 0;
	for( i = 0; i < 16; i++ )
	{
	  	value >>= 1;
	  	CLK_WRITE(0);//PA6�˿ڲ���
		DelayMicroSeconds(3);
		CLK_WRITE(1);//PA6�˿�Ϊ1
		DelayMicroSeconds(3);
		if( DATA_READ ) value |= 0x8000;//��ȡ������status
	}
	//�����λ������ԶΪ1010
	if( (value & 0xF000) == 0xA000 )
	{
	  	n = 0;
		for( i = 0; i < 8; i++ )
		{
			if( ((value>>i)& 0x01) == 0x01 )
				n++;  
		}
		//У��ͨ��
		if( ((value>>8)&0x07) == (8-n) )
		{
			key = (~value)&0x00ff;
		}
	}
	
	return key;
}

/*******************************************************************************
* ��������  : touch_process
* ��    ��  : ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void touch_process( void )
{
  	unsigned short key;
	
	//1.��ȡ����ֵ
	key = touch_get();
    //2.�жϳ����Ͷ̰�
	if( key != T_None)
	{
	  	if( m_keytime < 500 )
		{
			m_keytime++;
			//������3���ʾ����
			if( m_keytime == 300 )
			{
				m_key = key | T_L_X;
				m_keytime = 500;
			}
			m_old_key = key;
		}
	}
	else
	{
	  	if( (m_keytime > 0) && (m_keytime < 300) )
		{
			m_key = m_old_key;
		}
		if( m_keytime > 0 ) m_keytime = 0;
	}
    //3.��������
    if( m_key > T_None )
    {
        m_DevData.actived = ACTTIME;//���״̬//����ʱ��180��
        m_DevData.devtim_5min = ESCTIME;//�޲����Զ��˳�ʱ��60��
        m_DevData.blink = BLINK_TIME;//����ʱ��60s
        //3.1.����ر��ȵ�������
        if( (m_DevData.lcdonoff == 0) && (m_RunData.runmode > 0) )//m_RunData.runmode--//��ǰ����ģʽ    lcdonoff;       //����ر�
        {
            m_key = T_None;
            m_DevData.lcdonoff = 1;
        }
        //3.2.�ڿ���״̬����������
        m_DevData.lcdonoff = 1;
    }
    //4.�޲����Զ��˳�
    if( m_DevData.devtim_5min == 1 )
    {
        if( m_key == T_None )
            m_key = T_AUTO_ESC;
         m_DevData.devtim_5min = 0;
    }                            
}
/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
