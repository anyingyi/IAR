/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����:    rtc.c
* ��    ��:    �ⲿʱ��.c�ļ�
* ��    ��:    ������
* ��    ��:    V1.0
* ��    ��:    2017/6/2

* �޸���ʷ:    ʱ��        ����                  �޸���
               2017/6/12:  �ļ�����              ������
*******************************************************************************/
//includes
#include "rtc.h"
#include "data.h"
#include <stdio.h>
//----local definitions---------------------------------------------------------
//struct tm m_time_now;  //���ڵ�����ʱ��
//----local functions-----------------------------------------------------------
void rtc_config(void);
//----export functions-----------------------------------------------------------
/*******************************************************************************
* ��������  : rtc_init
* ��    ��  : �ⲿʱ�ӳ�ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void rtc_init(void)
{
    //������BKP�ĺ󱸼Ĵ���1�У�����һ�������ַ�0xA5A5
    //��һ���ϵ��󱸵�Դ����󣬸üĴ������ݶ�ʧ��
    //����RTC���ݶ�ʧ����Ҫ��������
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        //��������RTC
        rtc_config();
        //������ɺ���󱸼Ĵ�����д�����ַ�0xA5A5
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
        //����ʱ��󱸼Ĵ�������
        BKP_WriteBackupRegister(BKP_DR2, 0);
        BKP_WriteBackupRegister(BKP_DR3, 0);
        m_RunData.totaltime = 0;
    }
    else
    {
        /* Enable PWR and BKP clocks */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);
        //�ȴ�RTCʱ����APB1ʱ��ͬ��
        RTC_WaitForSynchro();
        //ʹ�����ж�
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        //�ȴ��������
        RTC_WaitForLastTask();
        //��ȡ�󱸼Ĵ����е�����ʱ��
        m_RunData.totaltime = (BKP_ReadBackupRegister(BKP_DR3)<<16)|BKP_ReadBackupRegister(BKP_DR2);
    }
    
    m_DevData.devtim_now = RTC_GetCounter();
    m_DevData.devtim_on = m_DevData.devtim_now;
    m_time_now = *gmtime(&m_DevData.devtim_now);
    return;
}

/*******************************************************************************
* ��������  : rtc_time_get
* ��    ��  : ��ȡʱ�䣬ת�����Ը�ʽ�ַ�����ʽ���
* ��    ��  : - timestr
*           : - timetype
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void rtc_time_string( unsigned char* timestr,unsigned char timetype,unsigned int timecnt)
{
    struct tm ttime = *gmtime(&timecnt);
    switch(timetype)
    {
    case 0x00:
      sprintf((char*)timestr,"%04d-%02d-%02d %02d:%02d:%02d",ttime.tm_year+1900,ttime.tm_mon+1,ttime.tm_mday,
                                      ttime.tm_hour,ttime.tm_min,ttime.tm_sec);
      break;
    case 0x01:
      sprintf((char*)timestr,"%04d��%02d��%02d��",ttime.tm_year+1900,ttime.tm_mon+1,ttime.tm_mday);
      break;
    case 0x02:
      sprintf((char*)timestr,"%02dʱ%02d��%02d��",ttime.tm_hour,ttime.tm_min,ttime.tm_sec);
      break;
    case 0x03:
      sprintf((char*)timestr,"%02d%02d%02d %02d:%02d:%02d",(ttime.tm_year+1900)%100,ttime.tm_mon+1,ttime.tm_mday,
                                                   ttime.tm_hour,ttime.tm_min,ttime.tm_sec);
      break;
    }
}

/*******************************************************************************
* ��������  : rtc_time_set
* ��    ��  : ����ʱ��
* ��    ��  : - year, - month, - day
*           : - hour, - minute, second
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void rtc_time_set(  unsigned short year,unsigned char month,unsigned char day,
                    unsigned char  hour,unsigned char minute,unsigned char second)
{
    struct tm ttime;
    unsigned int count;
    if( year == 0 )
    {
        ttime.tm_year = m_time_now.tm_year;
        ttime.tm_mon = m_time_now.tm_mon;
        ttime.tm_mday = m_time_now.tm_mday;
    }
    else
    {
        ttime.tm_year = year-1900;
        ttime.tm_mon = month-1;
        ttime.tm_mday = day;
    }
    if( hour == 0 && minute == 0 && second == 0 )
    {
        ttime.tm_sec = m_time_now.tm_sec;
        ttime.tm_min = m_time_now.tm_min;
        ttime.tm_hour = m_time_now.tm_hour+1;//w
    }
    else
    {
        ttime.tm_sec = second%60;
        ttime.tm_min = minute%60;
        ttime.tm_hour = hour%24;
    }
    count = mktime(&ttime);
    RTC_WaitForLastTask();
    RTC_SetCounter(count);
    RTC_WaitForLastTask();
}

/*******************************************************************************
* ��������  : rtc_time_set1
* ��    ��  : ����ʱ��
* ��    ��  : mtime:ʱ��ṹ
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void rtc_time_set1( struct tm mtime )
{
    unsigned int count;
    
    mtime.tm_year = 100 + mtime.tm_year%100;
    count = mktime(&mtime);
    RTC_WaitForLastTask();
    RTC_SetCounter(count);
    RTC_WaitForLastTask();
}

/*******************************************************************************
* ��������  : rtc_time_get
* ��    ��  : ��ȡʱ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void rtc_time_get( void )
{
  	//�����
  	if( m_DevData.devtim_second == 1 )
	{
		//��ȡʱ��
		m_time_now = *gmtime(&m_DevData.devtim_now);
		m_DevData.devtim_second = 0;
	}

}

/*******************************************************************************
* ��������  : rtc_config
* ��    ��  : rtc����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void rtc_config(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

/*************(C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾******************/