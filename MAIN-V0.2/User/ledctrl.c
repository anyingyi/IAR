/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: ledctrl.c
* ��    ��: ������ͨѶ����.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/1

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/1 :�ļ�����       ������
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

//LED����
typedef struct CLED_TIME_t
{
    unsigned char   lastcmd;      //��һLED��������
    unsigned char   onoff;        //LED״̬:0x00-��,0x01-��
    unsigned char   cmd;          //LED��������
    unsigned short  ontime;       //LED��ʱ��
    unsigned short  offtime;      //LED��ʱ��
    unsigned short  backontime;   //LED��ʱ�䱸��
    unsigned short  backofftime;  //LED��ʱ�䱸��
    unsigned char   times;        //�������:0x00����
}CLED_TIME;

CLED_TIME mCLED_TIME[MAX_LED];
unsigned char m_flag = 0;
unsigned char m_led_on = 0;

/*******************************************************************************
* ��������  : led_init
* ��    ��  : led��ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void led_init( void )
{
 	GPIO_InitTypeDef GPIO_InitStructure;
    unsigned char i = 0;
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE );
    //1.��ʼ������
	for( i = 0; i < MAX_LED; i++ )
	{
		GPIO_InitStructure.GPIO_Pin = mLEDCTRL[i].GPIO_Pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(mLEDCTRL[i].GPIOx, &GPIO_InitStructure);
		//��ʼ��״̬
	   	//GPIO_ResetBits(mLEDCTRL[i].GPIOx, mLEDCTRL[i].GPIO_Pin);
	}
    //2.led��ʼ��
	led_on( LED_RUN,  LED_BLINK, 500, 500, 0xff, 0 );
    GPIO_ResetBits(mLEDCTRL[LED_RX480].GPIOx, mLEDCTRL[LED_RX480].GPIO_Pin);
}

/*******************************************************************************
* ��������  : led_on
* ��    ��  : led����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
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
* ��������  : led_off
* ��    ��  : led�ر�
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
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
  * @param  -pled ledָʾ��
  * @param  -pctrl ledָʾ�ƿ���
  * @retval ��
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
					//�����ݼ�
					if( mCLED_TIME[id].times < 0xff )
						mCLED_TIME[id].times--;
                }
            }
            if( mCLED_TIME[id].ontime > 0 )mCLED_TIME[id].ontime--;//10msΪ��λ�ݼ�
            if( mCLED_TIME[id].offtime > 0 )mCLED_TIME[id].offtime--;//10msΪ��λ�ݼ�
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
  * @brief  Module_Led_Handle().1msΪ��λ�������е�
  * @param  ��
  * @retval ��
  ========================================================================*/
void led_process(void)
{
  	unsigned char i;
	
	for( i = 0; i < MAX_LED; i++ )
		led_single(i, LED_ON_HIGH);
}

/************* (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************/