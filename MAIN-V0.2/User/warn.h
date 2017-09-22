/****************** (C) COPYRIGHT 2010 开发路上工作小组 ***********************
* 文件名称:    warn.h
* 描    述:    告警判断模块ｈ文件
* 作    者:    王晶东
* 版    本:    V1.0
* 日    期:    2015/11/12

* 修改历史:    时间       内容                  修改者
               2015/11/12: 文件创建              王晶东
*******************************************************************************/
#ifndef _WARN_H_
#define _WARN_H_
//includes
#include "data.h"

#define WARN_TIME					3  					//告警判断周期3s
/* @defgroup Warn_bits_define
*/
/*#define Warn_NONE					0x00000000			//无故障
#define Warn_PM25SENSOR				0x00000001			//PM2.5传感器故障
#define Warn_THSENSOR				0x00000002			//温湿度传感器故障
#define Warn_CO2SENSOR				0x00000004			//CO2传感器故障
#define Warn_TVOCSENSOR				0x00000008	    	//TVOC传感器故障
#define Warn_PM25					0x00000010			//PM2.5告警
#define Warn_TEMP					0x00000020			//温度告警
#define Warn_HUMI					0x00000040			//湿度告警
#define Warn_CO2					0x00000080			//CO2告警
#define Warn_TVOC					0x00000100			//TVOC告警
#define Warn_FILTER					0x00000200			//滤网堵塞告警*/

/* @defgroup 波动值
*/
#define	WAVE_PM25	                3					//3
#define	WAVE_CO2	                40					//40
#define	WAVE_TVOC	                10					//10
#define	WAVE_TEMP	                30   				//0.3
#define	WAVE_HUMI	                30   				//0.3


#define RATE_FILTER                 15                  //滤芯剩余15%时告警

/*typedef enum
{
  	CMD_Warn_NONE					= 0x00,
  	CMD_Warn_PM25SENSOR				= 0x01, 			//PM2.5传感器故障
	CMD_Warn_THSENSOR				= 0x02,				//温湿度传感器故障
  	CMD_Warn_CO2SENSOR				= 0x03,				//CO2传感器故障
	CMD_Warn_TVOCSENSOR				= 0x04,         	//甲醛传感器故障
	CMD_Warn_PM25					= 0x05,				//PM2.5告警
	CMD_Warn_TEMP					= 0x06,				//温度告警
	CMD_Warn_HUMI					= 0x07,				//湿度告警
	CMD_Warn_CO2					= 0x08,				//CO2告警
	CMD_Warn_TVOC					= 0x09,				//TVOC告警
	CMD_Warn_FILTER					= 0x0A,				//电池告警

	CMD_Warn_MAX					= 0x0B,				//告警数量
}CMD_WARN_ID;*/


/* Exported macro ------------------------------------------------------------*/
extern void analy_warn( void );
extern void warn_process( void );
extern unsigned char analy_warnitem( unsigned int warnbits );


#endif //_WARN_H_
/*********(C) COPYRIGHT 2010 开发路上工作小组*****End of this file*************/