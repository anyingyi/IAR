/*************** (C) COPYRIGHT 2012 杭州卡丽智能科技股份有限公司 ***************
* 文件名称: comm_pm25.h
* 描    述: pm2.5.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/2

* 修改历史: 时间       内容           修改者
            2017/6/2 :文件创建       王晶东
*******************************************************************************/
#ifndef _COMM_PM25_H_
#define _COMM_PM25_H_
/* Includes ------------------------------------------------------------------*/
extern int Com_PM25_Rsp( unsigned char* buf,unsigned char id );
extern int Com_PM25_Dly( unsigned char* buf,unsigned char id );
#endif //_COMM_PM25_H_
