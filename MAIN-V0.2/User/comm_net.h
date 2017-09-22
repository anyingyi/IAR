/****************** (C) COPYRIGHT 2012 浙江中碳科技有限公司 ******************
* 文件名称: comm_net.h
* 描    述: 对上网络通信.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2015/4/27

* 修改历史: 时间       内容           修改者
            2015/4/27 :文件创建       王晶东
*******************************************************************************/
#ifndef _COMM_NET_H_
#define _COMM_NET_H_
/* Includes ------------------------------------------------------------------*/

extern int Com_Net_Req( unsigned char* buf,unsigned char id );
extern int Com_Net_Rsp( unsigned char* buf,unsigned char id );
#endif //_COMM_NET_H_
