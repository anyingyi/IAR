/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************
* �ļ�����: main.c
* ��    ��: �߿���������
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/08/25

* �޸���ʷ: ʱ��       ����           �޸���
            2017/08/25 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "adc.h"
#include "data.h"
#include "usart.h"
#include "comm.h"
#include "buzzer.h"
#include "flash.h"
#include "key.h"
#include "sensor.h"
#include "touch.h"
#include "fanctrl.h"
#include "sysctrl.h"
#include "remote.h"
#include "light.h"
#include "ledctrl.h"
#include "warn.h"

//���غ���
void RCCx_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void Timx_Configuration(void);
void IWDG_Init(void);


////////////////////////////////////////////////////////////////////////////////
//ϵͳ������
int main( void )
{
    //0.1.ϵͳ��ʼ��
    SystemInit();
    //0.2.RCC��ʼ��
    RCCx_Configuration();
    //0.3.IO�ڳ�ʼ��
    GPIO_Configuration();
    //0.4.�ж���������
    NVIC_Configuration();
    //0.5.ʱ�ӳ�ʼ��
    Timx_Configuration();
    //0.6.AD��ʼ��
    //Adc_Init();
    //0.7.RTC��ʱ�ӳ�ʼ��
    //rtc_init();
    //0.8.���ݳ�ʼ��
    //Data_Init(0);
    //0.9.���ڳ�ʼ��
    Usart_Init();
    //0.10.����ͨѶ��ʼ��
    Comm_Init();
    //0.11.��ʾ��ʼ��
    display_init();
    //0.13.ָʾ�Ƴ�ʼ��
    led_init();
	//0.14.��������ʼ��
	buzzer_init();
	//0.16.������ʼ��
	touch_init();
	//0.17.��������ʼ��
	sensor_init();
    //0.18.������Ƴ�ʼ��
    fan_init();
    //0.19.ң�س�ʼ��
    remote_init();
    //0.19.�͵�ѹ����ʼ��
    pvd_init();
    //0.20.ģ���Լ�ģʽ
    //check_process();
    //0.21.���Ź�
	//IWDG_Init();
    while(1)
    {
        //1.10ms�������
        if( m_timer_10ms == 0 )
        {
		  	//1.4.ʱ���ȡ
			//rtc_time_get();
            //���⴦��
            blink_process();  
            //��ʾ
            display_process();
            //���Ӧ
		  	light_process();
            //ң��
            remote_process();
		    //��������
			touch_process();
            //ָʾ�ƴ���
			led_process();  
            //��ť���ܴ���
            func_process();
            //������ƴ���
            fan_process();
            
            m_timer_10ms = 10;
        }
	  	 //2.1000ms�������
        if( m_timer_1000ms == 0 )
        {
		  	/*//2.5.�澯�ж�*/
            warn_process();
            //����ۼƼ�ʱ
            fan_totaltime();
            //��������
            time_process();
            //pm2.5����
            pm25_process();   
            //ģʽ����
            mode_process();
			
		  	m_timer_1000ms = 1000;
		}
		//3.WIFI����
		ComNet_check( WIFI_UP, 10 );
		//4.PM2.5����
		ComPM25_check( PM25_DOWN, 32 );
        //5.WIFI���� 
        mMod_Chk[WIFI_UP].mreqfunc( 0, WIFI_UP );
        //6.����������
        buzzer_process();
        //7.watch dog
		IWDG_ReloadCounter();
    }
}

////////////////////////////////////////////////////////////////////////////////
void RCCx_Configuration(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|
                           RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
    //ADC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    //��Ϊ���붨ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    //adc
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //TIM3
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //TIM4
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //dmaʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//����1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); 
	//����2ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    //��Դʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE); 
}

///////////////////////////////////////////////////////////////////
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
	/*-----------------------����1��ʼ��-------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    /*----------------------����2��ʼ��-------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
    /*-----------------------ADC�����ܽ�����--------------------------*/ 
    //�����������
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

///////////////////////////////////////////////////////////////////
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    //�ж�������
#ifdef  VECT_TAB_RAM
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //��ռʽ���ȼ�������Ϊ����ռ���ȼ�
    //�ж�����  
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
    
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
   
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);   
	
    /*NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);*/
}

///////////////////////////////////////////////////////////////////
void Timx_Configuration(void)
{
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;

	//1��1ms��ʱ��
    TIM_TimeBaseStructure.TIM_Prescaler = 999;
    TIM_TimeBaseStructure.TIM_Period = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter  = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM1, ENABLE);    
}

///////////////////////////////////////////////////////////////////
void IWDG_Init(void)            
{
    //�������Ź���ʼ��
  	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //�����Ĵ�����д
   	IWDG_SetPrescaler(IWDG_Prescaler_64);  //64��Ƶ һ������1.6ms
	IWDG_SetReload(625);  //�12λ [0,4096] 625*1.6=1S
   	IWDG_ReloadCounter();  //����������
   	IWDG_Enable();  //�������Ź�
}

/************* (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************/ 
