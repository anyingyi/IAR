/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称:    rtc.c
* 描    述:    外部时钟.c文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2017/6/2

* 修改历史:    时间        内容                  修改者
               2017/6/12:  文件创建              王晶东
*******************************************************************************/
//includes
#include "rtc.h"
#include "data.h"
#include <stdio.h>
//----local definitions---------------------------------------------------------
//struct tm m_time_now;  //现在的日期时间
//----local functions-----------------------------------------------------------
void rtc_config(void);
//----export functions-----------------------------------------------------------
/*******************************************************************************
* 函数名称  : rtc_init
* 描    述  : 外部时钟初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void rtc_init(void)
{
    //我们在BKP的后备寄存器1中，存了一个特殊字符0xA5A5
    //第一次上电或后备电源掉电后，该寄存器数据丢失，
    //表明RTC数据丢失，需要重新配置
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        //重新配置RTC
        rtc_config();
        //配置完成后，向后备寄存器中写特殊字符0xA5A5
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
        //运行时间后备寄存器清零
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
        //等待RTC时钟与APB1时钟同步
        RTC_WaitForSynchro();
        //使能秒中断
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        //等待操作完成
        RTC_WaitForLastTask();
        //读取后备寄存器中的运行时间
        m_RunData.totaltime = (BKP_ReadBackupRegister(BKP_DR3)<<16)|BKP_ReadBackupRegister(BKP_DR2);
    }
    
    m_DevData.devtim_now = RTC_GetCounter();
    m_DevData.devtim_on = m_DevData.devtim_now;
    m_time_now = *gmtime(&m_DevData.devtim_now);
    return;
}

/*******************************************************************************
* 函数名称  : rtc_time_get
* 描    述  : 获取时间，转换成以格式字符串形式输出
* 输    入  : - timestr
*           : - timetype
* 输    出  : None
* 返    回  : None
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
      sprintf((char*)timestr,"%04d年%02d月%02d日",ttime.tm_year+1900,ttime.tm_mon+1,ttime.tm_mday);
      break;
    case 0x02:
      sprintf((char*)timestr,"%02d时%02d分%02d秒",ttime.tm_hour,ttime.tm_min,ttime.tm_sec);
      break;
    case 0x03:
      sprintf((char*)timestr,"%02d%02d%02d %02d:%02d:%02d",(ttime.tm_year+1900)%100,ttime.tm_mon+1,ttime.tm_mday,
                                                   ttime.tm_hour,ttime.tm_min,ttime.tm_sec);
      break;
    }
}

/*******************************************************************************
* 函数名称  : rtc_time_set
* 描    述  : 设置时间
* 输    入  : - year, - month, - day
*           : - hour, - minute, second
* 输    出  : None
* 返    回  : None
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
* 函数名称  : rtc_time_set1
* 描    述  : 设置时间
* 输    入  : mtime:时间结构
* 输    出  : None
* 返    回  : None
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
* 函数名称  : rtc_time_get
* 描    述  : 获取时间
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void rtc_time_get( void )
{
  	//秒更新
  	if( m_DevData.devtim_second == 1 )
	{
		//获取时间
		m_time_now = *gmtime(&m_DevData.devtim_now);
		m_DevData.devtim_second = 0;
	}

}

/*******************************************************************************
* 函数名称  : rtc_config
* 描    述  : rtc配置
* 输    入  : None
* 输    出  : None
* 返    回  : None
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

/*************(C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司******************/