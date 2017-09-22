/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: ledctrl.c
* 描    述: 上下行通讯定义.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/1

* 修改历史: 时间       内容           修改者
            2017/6/1 :文件创建       王晶东
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "ledctrl.h"
#include "data.h"

typedef struct CLEDCTRL_t
{
    GPIO_TypeDef*   GPIOx;  //GPIO
    unsigned short  GPIO_Pin;   //pin
}CLEDCTRL;

const CLEDCTRL mLEDCTRL[MAX_LED] = 
{
	{ GPIOB, GPIO_Pin_15 },      //run
    { GPIOB, GPIO_Pin_7  },      //run
};

//LED控制
typedef struct CLED_TIME_t
{
    unsigned char   lastcmd;      //上一LED控制命令
    unsigned char   onoff;        //LED状态:0x00-灭,0x01-亮
    unsigned char   cmd;          //LED控制命令
    unsigned short  ontime;       //LED亮时间
    unsigned short  offtime;      //LED灭时间
    unsigned short  backontime;   //LED亮时间备份
    unsigned short  backofftime;  //LED灭时间备份
    unsigned char   times;        //闪灭次数:0x00常亮
}CLED_TIME;

CLED_TIME mCLED_TIME[MAX_LED];
unsigned char m_flag = 0;
unsigned char m_led_on = 0;

/*******************************************************************************
* 函数名称  : led_init
* 描    述  : led初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void led_init( void )
{
 	GPIO_InitTypeDef GPIO_InitStructure;
    unsigned char i = 0;
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE );
    //1.初始化点亮
	for( i = 0; i < MAX_LED; i++ )
	{
		GPIO_InitStructure.GPIO_Pin = mLEDCTRL[i].GPIO_Pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(mLEDCTRL[i].GPIOx, &GPIO_InitStructure);
		//初始化状态
	   	//GPIO_ResetBits(mLEDCTRL[i].GPIOx, mLEDCTRL[i].GPIO_Pin);
	}
    //2.led初始化
	led_on( LED_RUN,  LED_BLINK, 500, 500, 0xff, 0 );
    GPIO_ResetBits(mLEDCTRL[LED_RX480].GPIOx, mLEDCTRL[LED_RX480].GPIO_Pin);
}

/*******************************************************************************
* 函数名称  : led_on
* 描    述  : led开启
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void led_on(unsigned char id,unsigned char cmd,
            unsigned short ontime,unsigned short offtime,unsigned char times,unsigned char flag)
{
    if( mCLED_TIME[id].cmd != LED_BLINK )
        mCLED_TIME[id].lastcmd = mCLED_TIME[id].cmd;
    mCLED_TIME[id].cmd = cmd;
    mCLED_TIME[id].ontime = 0;
    mCLED_TIME[id].offtime = 0;
    mCLED_TIME[id].backontime = ontime/10;
    mCLED_TIME[id].backofftime = offtime/10;
    mCLED_TIME[id].times = times;
    mCLED_TIME[id].onoff = flag;
}

/*******************************************************************************
* 函数名称  : led_off
* 描    述  : led关闭
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void led_off(unsigned char id )
{
    if( mCLED_TIME[id].cmd != LED_OFF )
    {
        mCLED_TIME[id].cmd = LED_OFF;
        mCLED_TIME[id].ontime = 0;
        mCLED_TIME[id].offtime = 0;
        mCLED_TIME[id].backontime = 0;
        mCLED_TIME[id].backofftime = 0;
        mCLED_TIME[id].times = 0;
    }
}

/*========================================================================
  * @brief  led_single().
  * @param  -pled led指示灯
  * @param  -pctrl led指示灯控制
  * @retval 无
  ========================================================================*/
static void led_single(unsigned char id, unsigned char status )
{
    if(  mCLED_TIME[id].cmd == LED_BLINK )
    {
        if( mCLED_TIME[id].times > 0 )
        {
            if( mCLED_TIME[id].onoff == 0x00 )
            {
                if( mCLED_TIME[id].offtime == 0 )
                {
				  	if( status == LED_ON_LOW )
                    	GPIO_ResetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
					else
					  	GPIO_SetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
                    mCLED_TIME[id].ontime = mCLED_TIME[id].backontime;
                    mCLED_TIME[id].onoff = 1;
                }
            }
            else
            {
                if( mCLED_TIME[id].ontime == 0 )
                {
				  	if( status == LED_ON_LOW )
                    	GPIO_SetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
					else
					  	GPIO_ResetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
                    mCLED_TIME[id].offtime = mCLED_TIME[id].backofftime;
                    mCLED_TIME[id].onoff = 0;
					//次数递减
					if( mCLED_TIME[id].times < 0xff )
						mCLED_TIME[id].times--;
                }
            }
            if( mCLED_TIME[id].ontime > 0 )mCLED_TIME[id].ontime--;//10ms为单位递减
            if( mCLED_TIME[id].offtime > 0 )mCLED_TIME[id].offtime--;//10ms为单位递减
        }
        else
        {
            mCLED_TIME[id].cmd = mCLED_TIME[id].lastcmd;
			/*if( status == LED_ON_LOW )
            	GPIO_SetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
			else
			  	GPIO_ResetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );*/
        }
    }
    else
    {
        if( mCLED_TIME[id].cmd == LED_ON )
        {
            if( mCLED_TIME[id].onoff == 0 )
            {
			  	if( status == LED_ON_LOW )
                	GPIO_ResetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
				else
				  	GPIO_SetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
                mCLED_TIME[id].onoff = !mCLED_TIME[id].onoff;
            }
        }
        else
        {
            if( mCLED_TIME[id].onoff == 1 )
            {
			  	if( status == LED_ON_LOW )
                	GPIO_SetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
				else
				  	GPIO_ResetBits( mLEDCTRL[id].GPIOx,mLEDCTRL[id].GPIO_Pin );
                mCLED_TIME[id].onoff = !mCLED_TIME[id].onoff;
            }
        }
    }
}

/*========================================================================
  * @brief  Module_Led_Handle().1ms为单位处理所有灯
  * @param  无
  * @retval 无
  ========================================================================*/
void led_process(void)
{
  	unsigned char i;
	
	for( i = 0; i < MAX_LED; i++ )
		led_single(i, LED_ON_HIGH);
}

/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************/