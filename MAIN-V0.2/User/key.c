/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************
* �ļ�����:    key.c
* ��    ��:    ����.c�ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/6/1

* �޸���ʷ:    ʱ��        ����                  �޸���
               2017/6/1:  �ļ�����              ������
*******************************************************************************/
//includes
#include "key.h"
#include "data.h"
#include "flash.h"
#include "buzzer.h"

//----local definitions---------------------------------------------------------
typedef struct CKEY_CTRL_t
{
    GPIO_TypeDef* GPIOx;
    u16           pinx;
}CKEY_CTRL;

const CKEY_CTRL m_KEY_CTRL[KEY_INPUT] =
{
  	{GPIOA, GPIO_Pin_10 }, //��о��λ��ť
};

unsigned short m_settime = 0;
//----local functions-----------------------------------------------------------
//----export functions-----------------------------------------------------------
/*******************************************************************************
* ��������  : key_init
* ��    ��  : ������ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void key_init(void)
{
    unsigned char i;
    GPIO_InitTypeDef  GPIO_InitStructure;
    //�����ź���
    for( i = 0; i < KEY_INPUT; i++ )
    {
        GPIO_InitStructure.GPIO_Pin   = m_KEY_CTRL[i].pinx;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
        GPIO_Init(m_KEY_CTRL[i].GPIOx, &GPIO_InitStructure);
    }
}


/*******************************************************************************
* ��������  : key_check
* ��    ��  : ��ȡ״̬
* ��    ��  : None
* ��    ��  : None
* ��    ��  : unsigned char ״̬��
*******************************************************************************/
unsigned short key_time[KEY_INPUT] = {0};
unsigned char key_check(void)
{
    unsigned char i,bit;
	unsigned short key = KEY_NON;
    key = 0;
    for( i = 0; i < KEY_INPUT; i++ )
    {
		bit = GPIO_ReadInputDataBit(m_KEY_CTRL[i].GPIOx,m_KEY_CTRL[i].pinx);
		//�͵�ƽ
		if( bit == 0 )
		{
			if( key_time[i] < 600 )
			{
				key_time[i]++;
				if( key_time[i] == 500 )//����
				{
					key |= (1<<(i+4));
					key_time[i] = 300;
				}
			}
		}
		//�����ſ�
		else
		{
			if( key_time[i] > 4 )
			{
				if( key_time[i] <= 500 )//�̰�
				{
					key |= 1<<i;
				}
			}
			if( key_time[i] > 0 )
			{
				key_time[i] = 0;
			}
		}
    }
	
    return key;
}

/*******************************************************************************
* ��������  : key_process
* ��    ��  : ��ȡ��ֵ
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
unsigned char flag = 0;
void key_process( void )
{
  	unsigned char key = KEY_NON;
	
	key = key_check(); 
    
    if( key == KEY_SHORT )
    {
        if( flag == 0 )
        {
            if( m_test_pwm > 20)
                m_test_pwm -= 2;
            if( m_test_pwm == 20 )
                flag = 1;
        }
        else
        {
            if( m_test_pwm < 94 )
                m_test_pwm += 2;  
            if( m_test_pwm == 94)
                flag = 0;
        }
    }
	
	//1.������������������ʱ��
	/*if( key == KEY_LONG )
	{
        if( m_RunData.totaltime > 0 )
        {
            buzzer_on( BUZZER_0, 200, 100, 3 );
            m_SetData.usedfilter = 0;
            m_RunData.totaltime = 0;
            m_RunData.filter = 100;
            //�洢����
            flash_e( ADDR_SETTING, sizeof( struct SetData ) );
			flash_w( (unsigned short*)&m_SetData,ADDR_SETTING,sizeof( struct SetData ));
        }
    }*/	  	
}
/************* (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************/
