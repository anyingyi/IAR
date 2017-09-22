/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************
* 文件名称:    key.c
* 描    述:    按键.c文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/6/1

* 修改历史:    时间        内容                  修改者
               2017/6/1:  文件创建              王晶东
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
  	{GPIOA, GPIO_Pin_10 }, //滤芯复位按钮
};

unsigned short m_settime = 0;
//----local functions-----------------------------------------------------------
//----export functions-----------------------------------------------------------
/*******************************************************************************
* 函数名称  : key_init
* 描    述  : 按键初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void key_init(void)
{
    unsigned char i;
    GPIO_InitTypeDef  GPIO_InitStructure;
    //按键信号线
    for( i = 0; i < KEY_INPUT; i++ )
    {
        GPIO_InitStructure.GPIO_Pin   = m_KEY_CTRL[i].pinx;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
        GPIO_Init(m_KEY_CTRL[i].GPIOx, &GPIO_InitStructure);
    }
}


/*******************************************************************************
* 函数名称  : key_check
* 描    述  : 读取状态
* 输    入  : None
* 输    出  : None
* 返    回  : unsigned char 状态字
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
		//低电平
		if( bit == 0 )
		{
			if( key_time[i] < 600 )
			{
				key_time[i]++;
				if( key_time[i] == 500 )//长按
				{
					key |= (1<<(i+4));
					key_time[i] = 300;
				}
			}
		}
		//按键放开
		else
		{
			if( key_time[i] > 4 )
			{
				if( key_time[i] <= 500 )//短按
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
* 函数名称  : key_process
* 描    述  : 读取键值
* 输    入  : None
* 输    出  : None
* 返    回  : None
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
	
	//1.长按动作，清零运行时间
	/*if( key == KEY_LONG )
	{
        if( m_RunData.totaltime > 0 )
        {
            buzzer_on( BUZZER_0, 200, 100, 3 );
            m_SetData.usedfilter = 0;
            m_RunData.totaltime = 0;
            m_RunData.filter = 100;
            //存储数据
            flash_e( ADDR_SETTING, sizeof( struct SetData ) );
			flash_w( (unsigned short*)&m_SetData,ADDR_SETTING,sizeof( struct SetData ));
        }
    }*/	  	
}
/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************/
