/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: light.h
* 描    述: 光强度.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/5/6

* 修改历史: 时间       内容           修改者
            2017/5/6: 文件创建        王晶东
*******************************************************************************/
#ifndef _LIGHT_H_
#define _LIGHT_H_


#define THD_LIGHT	2750  	//黑暗环境临界值
#define WAVE_LIGHT	100		//允许光强度波动范围


extern void light_process(void);
#endif //_LIGHT_H_
/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
