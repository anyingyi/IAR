/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 *****************
* 文件名称:    buzzer.c
* 描    述:    蜂鸣器模块.c文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/8/16
* 修改历史:    时间        内容                  修改者
               2017/8/16:  文件创建              王晶东
*******************************************************************************/
//includes
#include "buzzer.h"
#include "data.h"

//8音度
#define PITCH_1         0       //1音度
#define PITCH_2         1       //2音度
#define PITCH_3         2       //3音度
#define PITCH_4         3       //4音度
#define PITCH_5         4       //5音度
#define PITCH_6         5       //6音度
#define PITCH_7         6       //7音度
#define PITCH_i         7       //8音度
#define PITCH_X         8       //NONE音度
//音阶
#define GAMUT_1         1       //一阶音
#define GAMUT_2         2       //一阶音
#define GAMUT_3         3       //三阶音
#define GAMUT_4         4       //四阶音
//电压控制时间
#define VTIME_ON        40      //电源开启持续时间40ms
#define VTIME_OFF       80      //电源关闭持续时间80ms

typedef struct CBUZZERCTRL_t
{
    GPIO_TypeDef*   PGPIOx;      //电源控制端口
    unsigned short  PGPIO_Pin;   //电源控制引脚
    GPIO_TypeDef*   FGPIOx;      //频率控制端口
    unsigned short  FGPIO_Pin;   //频率控制引脚
}CBUZZERCTRL;

const CBUZZERCTRL mBUZZERCTRL[MAX_BUZZER] = 
{
     GPIOB, GPIO_Pin_8, GPIOB, GPIO_Pin_9      //蜂鸣器1
};

//8音度频率
//const unsigned short m_Pitch8[8] = { 2300, 2463, 2625, 2733, 2950, 3167, 3438, 3600 };
//const unsigned short m_Pitch8[8] = { 4600, 4926, 5250, 5466, 5900, 6334, 6876, 7200 };
const unsigned short m_Pitch8[8] = { 5600, 5926, 6250, 6466, 6900, 7334, 7876, 8200 };
//const unsigned short m_Pitch8[8] = { 2731, 2731, 2731, 2731, 2731, 2731, 2731, 2731 };
//音阶频率持续时间
const unsigned short m_Gamut[4][4] = { {3040, 0   , 0   , 0    },     //一阶音
                                       {200 , 3040, 0   , 0    },     //二阶音
                                       {200 , 120 , 3040, 0    },     //三阶音
                                       {200 , 120 , 120 , 3040 } };   //四阶音
//和旋音调
typedef struct CMUSIC_t
{
    unsigned char   gamut;         //音阶
    unsigned char   sound[4];      //音谱
}CMUSIC;

const CMUSIC m_Music[MUSICS_MAX] = { GAMUT_4, {PITCH_1, PITCH_3, PITCH_5, PITCH_i },     //135i
                                     GAMUT_4, {PITCH_i, PITCH_5, PITCH_3, PITCH_1 },     //135i
                                     GAMUT_3, {PITCH_6, PITCH_4, PITCH_2, PITCH_X },
                                     GAMUT_3, {PITCH_2, PITCH_4, PITCH_6, PITCH_X },     //246
                                     GAMUT_1, {PITCH_2, PITCH_X, PITCH_X, PITCH_X },     //i53i
                                     GAMUT_1, {PITCH_3, PITCH_X, PITCH_X, PITCH_X },     //642 
                                     GAMUT_1, {PITCH_4, PITCH_X, PITCH_X, PITCH_X },     //5
                                     GAMUT_1, {PITCH_5, PITCH_X, PITCH_X, PITCH_X },     //5
                                     GAMUT_2, {PITCH_6, PITCH_5, PITCH_X, PITCH_X },     //i5
                                     GAMUT_4, {PITCH_i, PITCH_4, PITCH_6, PITCH_i }};    //357

CBUZZER_TIME mBUZZER_TIME[MAX_BUZZER];
//----local definitions---------------------------------------------------------
//----local functions-----------------------------------------------------------
//----export functions-----------------------------------------------------------
/*******************************************************************************
* 函数名称  : buzzer_init
* 描    述  : 蜂鸣器初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void buzzer_init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    unsigned char i = 0;
	
    //1.初始化点亮
	for( i = 0; i < MAX_BUZZER; i++ )
	{
		GPIO_InitStructure.GPIO_Pin = mBUZZERCTRL[i].PGPIO_Pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(mBUZZERCTRL[i].PGPIOx, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = mBUZZERCTRL[i].FGPIO_Pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(mBUZZERCTRL[i].FGPIOx, &GPIO_InitStructure);
	}
    //1.1.初始化状态
    GPIO_ResetBits(mBUZZERCTRL[0].PGPIOx, mBUZZERCTRL[0].PGPIO_Pin);
    GPIO_ResetBits(mBUZZERCTRL[0].FGPIOx, mBUZZERCTRL[0].FGPIO_Pin);
	//2、0.1ms定时期
    TIM_TimeBaseStructure.TIM_Prescaler = 499; //2Khz
    TIM_TimeBaseStructure.TIM_Period = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter  = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  
    //3、开机音效
    buzzer_on(0, MUSIC_ON);
}


/*******************************************************************************
* 函数名称  : buzzer_freqturn
* 描    述  : 蜂鸣器频率信号翻转
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void buzzer_freqturn( unsigned char id )
{
    //1.关闭
    if( mBUZZER_TIME[id].fonoff )
    {
        GPIO_ResetBits(mBUZZERCTRL[id].FGPIOx, 
                       mBUZZERCTRL[id].FGPIO_Pin);
        mBUZZER_TIME[id].fonoff = 0;
    }
    //2.开启
    else
    {
        GPIO_SetBits(mBUZZERCTRL[id].FGPIOx, 
                     mBUZZERCTRL[id].FGPIO_Pin);
        mBUZZER_TIME[id].fonoff = 1;
    }
}

/*******************************************************************************
* 函数名称  : buzzer_on
* 描    述  : 蜂鸣器开启(10ms为单位)
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void buzzer_on(unsigned char id, unsigned char musicid )
{
    unsigned int freq = 0;
    
    //1.运行参数设定
    mBUZZER_TIME[id].onoff = 1;
    mBUZZER_TIME[id].fonoff = 0;
    mBUZZER_TIME[id].musicid = musicid;
    mBUZZER_TIME[id].findex = 0;
    mBUZZER_TIME[id].vindex = 0;
    mBUZZER_TIME[id].vonoff = 0;
    mBUZZER_TIME[id].vtime = VTIME_OFF;
    mBUZZER_TIME[id].ftime = m_Gamut[m_Music[musicid].gamut-1][0];;
    //2.电源关闭
    GPIO_ResetBits(mBUZZERCTRL[id].PGPIOx, mBUZZERCTRL[id].PGPIO_Pin);
    //3.频率计算和加载
    freq = m_Pitch8[m_Music[musicid].sound[0]];
    freq = (unsigned int)(500000/freq+0.5)-1;
    TIM_SetAutoreload( TIM2, freq );
    TIM_Cmd(TIM2, ENABLE);  
}

/*******************************************************************************
* 函数名称  : buzzer_freqoff
* 描    述  : 蜂鸣器关闭
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void buzzer_freqoff( unsigned char id )
{
    GPIO_ResetBits(mBUZZERCTRL[id].FGPIOx, mBUZZERCTRL[id].FGPIO_Pin);
	mBUZZER_TIME[id].fonoff = 0;
}

/*******************************************************************************
* 函数名称  : buzzer_single
* 描    述  : 蜂鸣器控制
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void buzzer_single( unsigned char id )
{
    unsigned int freq = 0;
    unsigned char idm, index1, index2, gamut;
    
    //1.开启状态
    if( mBUZZER_TIME[id].onoff )
    {
        idm = mBUZZER_TIME[id].musicid;
        gamut = m_Music[idm].gamut;
        //1.1.频率控制持续时间到
        if( mBUZZER_TIME[id].ftime == 0 )
        {  
            //1.1.1.多阶音调
            mBUZZER_TIME[id].findex++;
            index1 = mBUZZER_TIME[id].findex;
            if( index1 < gamut )
            {
                //1.1.1.1.频率持续时间
                mBUZZER_TIME[id].ftime = m_Gamut[gamut-1][index1];
                //1.1.1.2.频率计算和加载
                freq = m_Pitch8[m_Music[idm].sound[index1]];
                freq = (unsigned int)(500000/freq+0.5)-1;
                TIM_SetAutoreload( TIM2, freq );
            }
            else
            {
                //1.1.1.3.关闭
                mBUZZER_TIME[id].onoff  = 0;
                GPIO_ResetBits(mBUZZERCTRL[id].PGPIOx, mBUZZERCTRL[id].PGPIO_Pin);
                GPIO_ResetBits(mBUZZERCTRL[id].FGPIOx, mBUZZERCTRL[id].FGPIO_Pin);
                TIM_Cmd(TIM2, DISABLE); 
            }
        }
        //1.2.电源持续时间到
        if( mBUZZER_TIME[id].vtime == 0 )
        {
            //1.2.1.电源控制
            index2 = mBUZZER_TIME[id].vindex;
            if( index2 < gamut )
            {
                //1.2.1.1.原来关闭现需要开启
                if( !mBUZZER_TIME[id].vonoff )
                {
                    mBUZZER_TIME[id].vonoff = 1;
                    mBUZZER_TIME[id].vtime = VTIME_ON;
                    GPIO_SetBits(mBUZZERCTRL[id].PGPIOx, mBUZZERCTRL[id].PGPIO_Pin);    
                }
                //1.2.1.2.原来开启现需要关闭
                else 
                {
                    mBUZZER_TIME[id].vonoff = 0;
                    mBUZZER_TIME[id].vtime = VTIME_OFF;
                    GPIO_ResetBits(mBUZZERCTRL[id].PGPIOx, mBUZZERCTRL[id].PGPIO_Pin); 
                    mBUZZER_TIME[id].vindex++;
                }  
            }
        }
    }
}

/*******************************************************************************
* 函数名称  : buzzer_process
* 描    述  : 蜂鸣器控制
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void buzzer_process( void )
{
	unsigned char i;
	
	for( i = 0; i < MAX_BUZZER; i++ )
	{
		buzzer_single(i);
	}
}

/************(C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司*******************/