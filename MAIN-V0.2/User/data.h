/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************
* 文件名称: data.h
* 描    述: 数据结构定义和数据定义.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/2

* 修改历史: 时间       内容           修改者
            2017/6/2 :文件创建       王晶东
*******************************************************************************/
#ifndef _DATA_H_
#define _DATA_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "time.h"

/*
STM32F103RBT6 FLASH SIZE: 128K,MEM SIZE 20K
*/
//配置数据保存地址
#define ADDR_SETTING	 0x0801F800

/*------------------------------------------------------------*/
#define IS_CHAR(x)	(((x >= '0')&&(x <= '9')) || \
					 ((x >= 'A')&&(x <= 'Z')) || \
  					 ((x >= 'a')&&(x <= 'z')))

#define ZS0				0x00 	//不支持0
#define ZS1   			0x01	//支持0
#define DEC1  			0x02	//1位小数
#define DEC2  			0x04	//2位小数
#define DEC3  			0x06	//3位小数


#define ACTTIME			180		//背光时间180秒
#define ESCTIME			60		//无操作自动退出时间60秒
#define BLINK_TIME      60      //背光时间60s
#define FILTER_TIME     3       //滤芯剩余百分比显示时间3s

#define PRODUCT_DATE	170720

struct SetData
{
    unsigned char  	serialnumber[8]; 	//序列号
    unsigned short 	hardversion; 		//硬件版本，通过该版本号区分是否带WIFI模块
    unsigned short 	softversion; 		//软件版本
    unsigned short 	productdate; 		//出产日期
	unsigned short 	pm25[5]; 			//PM2.5等级划分，0位小数
	  signed short 	temp[2]; 			//温度上下限，2位小数
	unsigned short  humi[2]; 			//湿度上下限，2位小数
	unsigned short  co2[3]; 			//CO2等级划分，0位小数
	unsigned short  tvoc[3]; 			//TVOC等级划分，0位小数
	unsigned short  hearttime; 			//心跳时间间隔，单位秒
	unsigned short  realtime; 			//上报时间间隔，单位秒
	unsigned int	filtertime;			//滤网寿命时间
	unsigned short	onofftime[7][2];	//定时开关机时间
	unsigned short	setmode[7]; 		//定时开机运行模式
    unsigned int    usedfilter;         //滤网已使用时间
    unsigned short  advancetime;        //定时开关机时间，单位：h
    unsigned int    keycode[6];         //按键码
	unsigned short	reserved[8]; 		//保留
};

struct RunData
{
    unsigned short  status; 			//状态信息,从低位到高位分别为，PM2.5传感器、温度传感器、湿度传感器、CO2传感器、TVOC传感器 
    unsigned short  pm01;				//PM1数值，0位小数
    unsigned short  pm25;				//PM2.5数值，0位小数
    unsigned short  pm10;				//PM10数值，0位小数	
      signed short  temp;				//温度数值，2位小数
	unsigned short	humi;				//湿度数值，2位小数
	unsigned short  co2;				//CO2数值，0位小数	
	unsigned short  tvoc;				//tvoc数值，0位小数	
	unsigned short  signal;				//wifi强度，0位小数	
	unsigned short  runmode;			//当前运行模式
	unsigned short  filter;				//滤芯剩余百分比
	unsigned int	totaltime;			//风机运行总时间
	unsigned int	level1time;			//一档运行总时间
	unsigned int	level2time;			//二档运行总时间
	unsigned int	level3time;			//三档运行总时间
    unsigned char   warnsensor;         //传感器状态，低位到高位 PM2.5  temp&humi  CO2  TVOC   
    unsigned char   wpm25;              //PM2.5告警信息 
    unsigned char   wtemp;              //温度告警信息 
    unsigned char   whumi;              //湿度告警信息
    unsigned char   wco2;               //co2告警信息 
    unsigned char   wtvoc;              //TVOC告警信息 
    unsigned char   wfilter;            //滤网告警信息 
    unsigned char   reserved1[3];       //保留
	unsigned short  reserved[6];		//保留
};


//网路运行数据
struct NetData
{
  	unsigned char 	status;			//wifi模块状态 0-未连接、1-连接路由、2-配网状态、3-连接苏宁云
    unsigned char   signal;         //wifi信号强度
    unsigned char   ip[16];         //ip信息
    unsigned char   mask[16];       //mask信息    
    unsigned char   gate[16];       //gate信息   
    unsigned char   dns[16];        //dns信息
    unsigned char   mac[18];        //mac信息
    
  	unsigned char	connect;		//网络连接标志
	unsigned short	err_num;		//服务器连接失败次数
	unsigned short	heart_time;		//心跳计时
	unsigned short	real_time;		//实时计时
	unsigned short	para_time;		//参数计时
    unsigned short  status_time;    //状态计时
	unsigned short	other_time;		//其他计时
	unsigned short	bit_rsp;		//其他回复数据 优先级从低位至高位：参数正确下发回复、实时数据返回、参数数据返回、参数上报 、参数错误下发回复
	unsigned char	packagenum;		//包序号
	unsigned char	rt_update;      //远程升级返回标志
	unsigned short	rt_updatetime;  //远程升级返回计时
	unsigned char 	real_send;		//实时数据更新
	unsigned char	heart_c;		//心跳超时wifi复位
    unsigned char   status_rt;      //状态上报返回
    unsigned char   setnet;         //设置网络状态标志
};

struct DevData
{
	unsigned short 	inputword; 		//输入检测
    unsigned char  	warnbuzzer;     //告警标志
    unsigned char   trans;          //有线传输标志 1-连接，0-断开
    unsigned char   check;          //模块自检状态
    //检测激活
    unsigned short  actived;        //检测状态
    unsigned char   lcdonoff;       //背光关闭
    unsigned short  blink;          //背光计时
    unsigned char   filtertime;     //滤芯剩余百分比显示时间     
    //时间数据
    unsigned int   	devtim_on;		//
    unsigned int   	devtim_runtime;
	unsigned short 	devtim_connect;
    unsigned int   	devtim_now;
	unsigned char  	devtim_second;
    unsigned short  devtim_5min;    //5分钟计时
    //传感器数据
	unsigned char  	pm25status;     //PM2.5传感器状态
	unsigned short 	time_reset;		//传感器复位时间
    //模式数据
	unsigned short 	oldrunmode;     //上一次关机时运行模式
    unsigned short 	prerunmode;     //前一次运行模式
    unsigned short  timecount;      //定时倒计时
    unsigned char   timeactived;    //定时显示时间倒计时
    unsigned char   timemode;       //定时模式：0-定时和预约（手动定时）  1-APP定时
    //按键背光数据
	unsigned short	lightvalue;		//光强度采样值	
	unsigned char	lightonoff;		//白天或黑暗
    unsigned char	keyonoff;		//按键背光开关
    unsigned char   keyblink;       //按键背光状态
    //蜂鸣器数据
	unsigned short 	buzzertime;		//蜂鸣器时间
	unsigned short 	buzzertimes;	//蜂鸣器次数
    //网络下发数据
    unsigned char   net_automode;   //网络下发智能模式
    //滤网数据
    unsigned int    usedfilter;     //滤网已使用时间
    unsigned char   setmode;        //设置状态标志
};



extern struct tm		m_time_now; //现在的日期时间
extern struct tm        m_time_set; //设置的日期时间
extern struct SetData 	m_SetData;  //参数数据 
extern struct RunData 	m_RunData;  //运行数据
extern struct NetData 	m_NetData;  //网络数据
extern struct DevData 	m_DevData;  //本地数据

extern unsigned short m_test_pwm;

extern __IO unsigned short 	m_timer_1ms;
extern __IO unsigned short 	m_timer_10ms;
extern __IO unsigned short 	m_timer_1000ms;
extern __IO unsigned short  m_buzzer_time;
extern void Data_Init( unsigned char init );
extern void num2str(unsigned char *str,unsigned char numpro,int val );
extern void DelayMicroSeconds(unsigned int nbrOfUs);
extern unsigned char Str_Cmp( unsigned char* Str, unsigned char* Src );
extern unsigned short Str_Cpy( unsigned char* Str,unsigned char* Src );
//extern unsigned short Str_Cpy( unsigned char* Str,unsigned char* Src );


#endif //_DATA_H_
/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************/
