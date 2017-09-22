/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************
* 文件名称: main.c
* 描    述: 线控器主程序
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/08/25

* 修改历史: 时间       内容           修改者
            2017/08/25 :文件创建       王晶东
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

//本地函数
void RCCx_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void Timx_Configuration(void);
void IWDG_Init(void);


////////////////////////////////////////////////////////////////////////////////
//系统主函数
int main( void )
{
    //0.1.系统初始化
    SystemInit();
    //0.2.RCC初始化
    RCCx_Configuration();
    //0.3.IO口初始化
    GPIO_Configuration();
    //0.4.中断向量设置
    NVIC_Configuration();
    //0.5.时钟初始化
    Timx_Configuration();
    //0.6.AD初始化
    //Adc_Init();
    //0.7.RTC秒时钟初始化
    //rtc_init();
    //0.8.数据初始化
    //Data_Init(0);
    //0.9.串口初始化
    Usart_Init();
    //0.10.串口通讯初始化
    Comm_Init();
    //0.11.显示初始化
    display_init();
    //0.13.指示灯初始化
    led_init();
	//0.14.蜂鸣器初始化
	buzzer_init();
	//0.16.触摸初始化
	touch_init();
	//0.17.传感器初始化
	sensor_init();
    //0.18.电机控制初始化
    fan_init();
    //0.19.遥控初始化
    remote_init();
    //0.19.低电压检测初始化
    pvd_init();
    //0.20.模块自检模式
    //check_process();
    //0.21.看门狗
	//IWDG_Init();
    while(1)
    {
        //1.10ms间隔处理
        if( m_timer_10ms == 0 )
        {
		  	//1.4.时间获取
			//rtc_time_get();
            //背光处理
            blink_process();  
            //显示
            display_process();
            //光感应
		  	light_process();
            //遥控
            remote_process();
		    //触摸按键
			touch_process();
            //指示灯处理
			led_process();  
            //按钮功能处理
            func_process();
            //电机控制处理
            fan_process();
            
            m_timer_10ms = 10;
        }
	  	 //2.1000ms间隔处理
        if( m_timer_1000ms == 0 )
        {
		  	/*//2.5.告警判断*/
            warn_process();
            //风机累计计时
            fan_totaltime();
            //秒任务处理
            time_process();
            //pm2.5处理
            pm25_process();   
            //模式处理
            mode_process();
			
		  	m_timer_1000ms = 1000;
		}
		//3.WIFI接收
		ComNet_check( WIFI_UP, 10 );
		//4.PM2.5接收
		ComPM25_check( PM25_DOWN, 32 );
        //5.WIFI发送 
        mMod_Chk[WIFI_UP].mreqfunc( 0, WIFI_UP );
        //6.蜂鸣器处理
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
    //作为毫秒定时器
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    //adc
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //TIM3
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //TIM4
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //dma时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//串口1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); 
	//串口2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    //电源时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE); 
}

///////////////////////////////////////////////////////////////////
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
	/*-----------------------串口1初始化-------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    /*----------------------串口2初始化-------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
    /*-----------------------ADC采样管脚配置--------------------------*/ 
    //光敏电阻采样
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

///////////////////////////////////////////////////////////////////
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    //中断向量表
#ifdef  VECT_TAB_RAM
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //抢占式优先级别设置为无抢占优先级
    //中断设置  
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

	//1、1ms定时期
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
    //独立看门狗初始化
  	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //启动寄存器读写
   	IWDG_SetPrescaler(IWDG_Prescaler_64);  //64分频 一个周期1.6ms
	IWDG_SetReload(625);  //最长12位 [0,4096] 625*1.6=1S
   	IWDG_ReloadCounter();  //重启计数器
   	IWDG_Enable();  //启动看门狗
}

/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************/ 
