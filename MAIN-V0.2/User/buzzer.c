/************* (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ *****************
* �ļ�����:    buzzer.c
* ��    ��:    ������ģ��.c�ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/8/16
* �޸���ʷ:    ʱ��        ����                  �޸���
               2017/8/16:  �ļ�����              ������
*******************************************************************************/
//includes
#include "buzzer.h"
#include "data.h"

//8����
#define PITCH_1         0       //1����
#define PITCH_2         1       //2����
#define PITCH_3         2       //3����
#define PITCH_4         3       //4����
#define PITCH_5         4       //5����
#define PITCH_6         5       //6����
#define PITCH_7         6       //7����
#define PITCH_i         7       //8����
#define PITCH_X         8       //NONE����
//����
#define GAMUT_1         1       //һ����
#define GAMUT_2         2       //һ����
#define GAMUT_3         3       //������
#define GAMUT_4         4       //�Ľ���
//��ѹ����ʱ��
#define VTIME_ON        40      //��Դ��������ʱ��40ms
#define VTIME_OFF       80      //��Դ�رճ���ʱ��80ms

typedef struct CBUZZERCTRL_t
{
    GPIO_TypeDef*   PGPIOx;      //��Դ���ƶ˿�
    unsigned short  PGPIO_Pin;   //��Դ��������
    GPIO_TypeDef*   FGPIOx;      //Ƶ�ʿ��ƶ˿�
    unsigned short  FGPIO_Pin;   //Ƶ�ʿ�������
}CBUZZERCTRL;

const CBUZZERCTRL mBUZZERCTRL[MAX_BUZZER] = 
{
     GPIOB, GPIO_Pin_8, GPIOB, GPIO_Pin_9      //������1
};

//8����Ƶ��
//const unsigned short m_Pitch8[8] = { 2300, 2463, 2625, 2733, 2950, 3167, 3438, 3600 };
//const unsigned short m_Pitch8[8] = { 4600, 4926, 5250, 5466, 5900, 6334, 6876, 7200 };
const unsigned short m_Pitch8[8] = { 5600, 5926, 6250, 6466, 6900, 7334, 7876, 8200 };
//const unsigned short m_Pitch8[8] = { 2731, 2731, 2731, 2731, 2731, 2731, 2731, 2731 };
//����Ƶ�ʳ���ʱ��
const unsigned short m_Gamut[4][4] = { {3040, 0   , 0   , 0    },     //һ����
                                       {200 , 3040, 0   , 0    },     //������
                                       {200 , 120 , 3040, 0    },     //������
                                       {200 , 120 , 120 , 3040 } };   //�Ľ���
//��������
typedef struct CMUSIC_t
{
    unsigned char   gamut;         //����
    unsigned char   sound[4];      //����
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
* ��������  : buzzer_init
* ��    ��  : ��������ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void buzzer_init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    unsigned char i = 0;
	
    //1.��ʼ������
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
    //1.1.��ʼ��״̬
    GPIO_ResetBits(mBUZZERCTRL[0].PGPIOx, mBUZZERCTRL[0].PGPIO_Pin);
    GPIO_ResetBits(mBUZZERCTRL[0].FGPIOx, mBUZZERCTRL[0].FGPIO_Pin);
	//2��0.1ms��ʱ��
    TIM_TimeBaseStructure.TIM_Prescaler = 499; //2Khz
    TIM_TimeBaseStructure.TIM_Period = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter  = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  
    //3��������Ч
    buzzer_on(0, MUSIC_ON);
}


/*******************************************************************************
* ��������  : buzzer_freqturn
* ��    ��  : ������Ƶ���źŷ�ת
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void buzzer_freqturn( unsigned char id )
{
    //1.�ر�
    if( mBUZZER_TIME[id].fonoff )
    {
        GPIO_ResetBits(mBUZZERCTRL[id].FGPIOx, 
                       mBUZZERCTRL[id].FGPIO_Pin);
        mBUZZER_TIME[id].fonoff = 0;
    }
    //2.����
    else
    {
        GPIO_SetBits(mBUZZERCTRL[id].FGPIOx, 
                     mBUZZERCTRL[id].FGPIO_Pin);
        mBUZZER_TIME[id].fonoff = 1;
    }
}

/*******************************************************************************
* ��������  : buzzer_on
* ��    ��  : ����������(10msΪ��λ)
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void buzzer_on(unsigned char id, unsigned char musicid )
{
    unsigned int freq = 0;
    
    //1.���в����趨
    mBUZZER_TIME[id].onoff = 1;
    mBUZZER_TIME[id].fonoff = 0;
    mBUZZER_TIME[id].musicid = musicid;
    mBUZZER_TIME[id].findex = 0;
    mBUZZER_TIME[id].vindex = 0;
    mBUZZER_TIME[id].vonoff = 0;
    mBUZZER_TIME[id].vtime = VTIME_OFF;
    mBUZZER_TIME[id].ftime = m_Gamut[m_Music[musicid].gamut-1][0];;
    //2.��Դ�ر�
    GPIO_ResetBits(mBUZZERCTRL[id].PGPIOx, mBUZZERCTRL[id].PGPIO_Pin);
    //3.Ƶ�ʼ���ͼ���
    freq = m_Pitch8[m_Music[musicid].sound[0]];
    freq = (unsigned int)(500000/freq+0.5)-1;
    TIM_SetAutoreload( TIM2, freq );
    TIM_Cmd(TIM2, ENABLE);  
}

/*******************************************************************************
* ��������  : buzzer_freqoff
* ��    ��  : �������ر�
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void buzzer_freqoff( unsigned char id )
{
    GPIO_ResetBits(mBUZZERCTRL[id].FGPIOx, mBUZZERCTRL[id].FGPIO_Pin);
	mBUZZER_TIME[id].fonoff = 0;
}

/*******************************************************************************
* ��������  : buzzer_single
* ��    ��  : ����������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void buzzer_single( unsigned char id )
{
    unsigned int freq = 0;
    unsigned char idm, index1, index2, gamut;
    
    //1.����״̬
    if( mBUZZER_TIME[id].onoff )
    {
        idm = mBUZZER_TIME[id].musicid;
        gamut = m_Music[idm].gamut;
        //1.1.Ƶ�ʿ��Ƴ���ʱ�䵽
        if( mBUZZER_TIME[id].ftime == 0 )
        {  
            //1.1.1.�������
            mBUZZER_TIME[id].findex++;
            index1 = mBUZZER_TIME[id].findex;
            if( index1 < gamut )
            {
                //1.1.1.1.Ƶ�ʳ���ʱ��
                mBUZZER_TIME[id].ftime = m_Gamut[gamut-1][index1];
                //1.1.1.2.Ƶ�ʼ���ͼ���
                freq = m_Pitch8[m_Music[idm].sound[index1]];
                freq = (unsigned int)(500000/freq+0.5)-1;
                TIM_SetAutoreload( TIM2, freq );
            }
            else
            {
                //1.1.1.3.�ر�
                mBUZZER_TIME[id].onoff  = 0;
                GPIO_ResetBits(mBUZZERCTRL[id].PGPIOx, mBUZZERCTRL[id].PGPIO_Pin);
                GPIO_ResetBits(mBUZZERCTRL[id].FGPIOx, mBUZZERCTRL[id].FGPIO_Pin);
                TIM_Cmd(TIM2, DISABLE); 
            }
        }
        //1.2.��Դ����ʱ�䵽
        if( mBUZZER_TIME[id].vtime == 0 )
        {
            //1.2.1.��Դ����
            index2 = mBUZZER_TIME[id].vindex;
            if( index2 < gamut )
            {
                //1.2.1.1.ԭ���ر�����Ҫ����
                if( !mBUZZER_TIME[id].vonoff )
                {
                    mBUZZER_TIME[id].vonoff = 1;
                    mBUZZER_TIME[id].vtime = VTIME_ON;
                    GPIO_SetBits(mBUZZERCTRL[id].PGPIOx, mBUZZERCTRL[id].PGPIO_Pin);    
                }
                //1.2.1.2.ԭ����������Ҫ�ر�
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
* ��������  : buzzer_process
* ��    ��  : ����������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void buzzer_process( void )
{
	unsigned char i;
	
	for( i = 0; i < MAX_BUZZER; i++ )
	{
		buzzer_single(i);
	}
}

/************(C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾*******************/