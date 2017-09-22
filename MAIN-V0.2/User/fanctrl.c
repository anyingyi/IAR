/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: fan.c
* ��    ��: ���.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/8/25

* �޸���ʷ: ʱ��       ����           �޸���
            2017/8/25 :�ļ�����       ������
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "fanctrl.h"
#include "data.h"
#include "flash.h"

#define  ADD_1DAY_TIME      86400 //60*60*24��
#define  ADD_1MIN_TIME      60     //��

//��������ź�IO
typedef struct CFANCTRL_t
{
    GPIO_TypeDef*   GPIOx;      //GPIO
    unsigned short  GPIO_Pin;   //pin
}CFANCTRL;

const CFANCTRL mFANCTRL = 
{
    GPIOA, GPIO_Pin_7
}; 

//����������ݽṹ
CFAN m_fan;
//��λ�ź�
const unsigned char m_fan_duty[4] = { FAN_PWM_OFF,          //����ػ� 
                                      FAN_PWM_LEVEL1,       //���һ��
                                      FAN_PWM_LEVEL2,       //�������
                                      FAN_PWM_LEVEL3 };     //�������
//΢���ٽ�ֵ
const unsigned char m_fan_pwm[4] = {  FAN_PWM_OFF,          //����ػ�
                                      FAN_PWM_MAX,          //�ɵ����ֵ
                                      FAN_PWM_LEVEL1,       //��������ɵ����ֵ
                                      FAN_PWM_LEVEL2 };     //��������ɵ����ֵ 


unsigned int  m_1DayTimeInc, m_1MinTimeInc;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
* ��������  : fan_init
* ��    ��  : �����ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void fan_init(void)   
{   
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;
    GPIO_InitTypeDef  		  GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin   = mFANCTRL.GPIO_Pin;;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(mFANCTRL.GPIOx, &GPIO_InitStructure);
    //PWM����
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 99;//20KHz
    TIM_TimeBaseStructure.TIM_Prescaler = 35; 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    /* Output Compare Active Mode configuration: Channel2 */    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = 0; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //ʹ�����״̬
    TIM_OC2Init(TIM3, &TIM_OCInitStructure); //TIM3_CH2ͨ��ʹ�� 
	TIM_Cmd(TIM3, ENABLE);
	TIM_SetCompare2(TIM3, FAN_PWM_OFF);
    
    //������ʼ��
    m_fan.status = 0;
    m_fan.cmd = 0;
    m_fan.flag = 0;
    m_fan.duty = FAN_PWM_OFF;  
}

/*******************************************************************************
* ��������  : fan_set
* ��    ��  : ������Ʋ�������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void fan_set( unsigned char cmd )
{
    unsigned char level;
    
    //1.���ڹػ�״̬ʱ��������״̬
    if( m_fan.status == FAN_STATUS_OFF )
    {
        if( cmd > FAN_OFF )
        {
            //1.1.��������
            m_fan.status = FAN_STATUS_START; 
            m_fan.duty = FAN_PWM_START;
            m_fan.time = FAN_START_TIME;
            m_fan.flag = 1;
        }
    }
    //2.���ڿ���״̬
    else
    {
        //2.1.�ػ�����
        if( cmd == FAN_OFF )
        {
            m_fan.duty = FAN_PWM_OFF;
            m_fan.status = FAN_STATUS_OFF;
        }
        //2.2.һ������״̬
        else if( (cmd >= FAN_LEVEL1) && (cmd <= FAN_LEVEL3) )
        {
            m_fan.duty = m_fan_duty[cmd];
            m_fan.status = FAN_STATUS_ON; 
        }
        //2.3.΢��״̬
        else if( cmd == FAN_ADJUST )
        {
            level = m_RunData.runmode & 0x0003;
            if( m_fan.duty < m_fan_pwm[level] ) 
                m_fan.duty += FAN_PWM_DWN;
            m_fan.status = FAN_STATUS_ON; 
        }
    }
    //3.�ڷ�����״̬ʱ���¿����ź�
    if( m_fan.status != FAN_STATUS_START )
        m_fan.flag = 1;
    m_fan.cmd = cmd;
}

/*******************************************************************************
* ��������  : fan_ctrl
* ��    ��  : ������ƺ���
* ��    ��  : status������ģʽ
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void fan_ctrl( unsigned short status )
{
    unsigned char level;
    
    //1.���ģʽ�б仯
    if( m_DevData.prerunmode != status )
    {
        //1.1.�ػ�״̬��¼�ػ���λ
        if( status == FAN_OFF )
        {
            if( !m_DevData.timemode )
                m_DevData.oldrunmode = m_RunData.runmode&0x2f;
            else
                m_DevData.oldrunmode = m_RunData.runmode;  
        }
        //1.2.ģʽ����
        m_RunData.runmode = status;
        //1.1.΢��ģʽ
        if( (m_RunData.runmode & FAN_ADJUST) == FAN_ADJUST )
        {
            //1.1.1.΢������
            fan_set( FAN_ADJUST );
            //1.1.2.��־λ����
            m_RunData.runmode &= ~FAN_ADJUST; 
        }
        //1.2.����ģʽ
        else
        {
            //1.2.1.��λ
            level = status & 0x0F;  
            //1.2.2.��ȷ��λ����
            if( level <= FAN_LEVEL3 )
                fan_set( level );
        }
        //1.3.��������ģʽ
        m_DevData.prerunmode = m_RunData.runmode;
    }
}

/*******************************************************************************
* ��������  : fan_process
* ��    ��  : ���������̺���
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void fan_process( void )
{
    //1.�����������״̬
    if( ( m_fan.status == FAN_STATUS_START) && (!m_fan.time) )
    {
        m_fan.status = FAN_STATUS_ON;  
        fan_set( m_fan.cmd );
    }
    //2.�����ź��и���
    if( m_fan.flag )
    {
        TIM_SetCompare2(TIM3, m_fan.duty );
        m_fan.flag = 0;
    }
}

/*******************************************************************************
* ��������  : fan_totaltime
* ��    ��  : ���������̺���
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void fan_totaltime(void)
{
    unsigned char save;
    float rate;
    
    //�������״̬
    save = 0;
	//1.�е�λ���ݣ���ʱ
	if( (m_RunData.runmode & 0x0F) > FAN_OFF )
    {
        m_1DayTimeInc++;
        m_1MinTimeInc++;
         //1.�ۼ�1�챣��һ������
        if( m_1DayTimeInc == ADD_1DAY_TIME )
        {
            m_1MinTimeInc = 0;
            m_1DayTimeInc = 0;
            m_RunData.totaltime += ADD_1DAY_TIME/60;
            m_SetData.usedfilter = m_RunData.totaltime;
            save |= 1;
        }
        else
        {
            //�ۼ�1���Ӹ���һ������
            if( m_1MinTimeInc == ADD_1MIN_TIME )
            {
                m_1MinTimeInc = 0;
                m_RunData.totaltime += ADD_1MIN_TIME/60;
                m_SetData.usedfilter = m_RunData.totaltime;
                //������оʣ��ٷֱ�
                if( m_SetData.filtertime > m_RunData.totaltime )
                {
                    rate = m_SetData.filtertime - m_RunData.totaltime;
                    rate = rate*100/m_SetData.filtertime+0.5;
                }
                else
                {
                    rate = 0;
                }
                m_RunData.filter = (unsigned short)rate;
            }        
        }
    }
    if( save == 1 )
    {
        //��������
        flash_e( ADDR_SETTING, sizeof( struct SetData ) );
        flash_w( (unsigned short*)&m_SetData, ADDR_SETTING, sizeof( struct SetData ));
    }
}

/**************** (C) COPYRIGHT 2012 ���ݿ����Ƽ��ɷ����޹�˾ *****************/
