/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: touch.c
* 描    述: 触摸键.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/3

* 修改历史: 时间       内容           修改者
            2017/6/3 :文件创建       王晶东
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "touch.h"
#include "data.h"
#include "ledctrl.h"
//置GPIOD->BSRR低16位的某位为'1'，则对应的I/O端口置'1'；而置GPIOD->BSRR低16位的某位为'0'，则对应的I/O端口不变。0X00000040对应PA6
#define CLK_WRITE(m)	(GPIOA->BSRR = m?0x00000040:0x00400000) //时钟clk输出 //端口设置和清除寄存器-BSRR
#define DATA_READ		(GPIOA->IDR&0x0020)  //数据data读取 	//端口输入数据这些位为只读并只能以字(16位)的形式读出。读出的值为对应I/O口的状态
//这些位为只读并只能以字(16位)的形式读出。读出的值为对应I/O口的状态.0X0020对应PA5
unsigned short m_key;
unsigned short m_old_key = 0;
unsigned short m_keytime = 0;

/*******************************************************************************
* 函数名称  : touch_int
* 描    述  : 触摸初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void touch_init( void )
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	
	//clock引脚输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//data引脚输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;//下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名称  : touch_get
* 描    述  : 获取触摸键值
* 输    入  : None
* 输    出  : None
* 返    回  : 按键值
*******************************************************************************/
unsigned short touch_get( void )
{	
  	unsigned char i, n;
    unsigned short key, value = 0;
	
    key = 0;
	for( i = 0; i < 16; i++ )
	{
	  	value >>= 1;
	  	CLK_WRITE(0);//PA6端口不变
		DelayMicroSeconds(3);
		CLK_WRITE(1);//PA6端口为1
		DelayMicroSeconds(3);
		if( DATA_READ ) value |= 0x8000;//获取按键的status
	}
	//最高四位数据永远为1010
	if( (value & 0xF000) == 0xA000 )
	{
	  	n = 0;
		for( i = 0; i < 8; i++ )
		{
			if( ((value>>i)& 0x01) == 0x01 )
				n++;  
		}
		//校验通过
		if( ((value>>8)&0x07) == (8-n) )
		{
			key = (~value)&0x00ff;
		}
	}
	
	return key;
}

/*******************************************************************************
* 函数名称  : touch_process
* 描    述  : 触摸
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void touch_process( void )
{
  	unsigned short key;
	
	//1.获取按键值
	key = touch_get();
    //2.判断长按和短按
	if( key != T_None)
	{
	  	if( m_keytime < 500 )
		{
			m_keytime++;
			//连续按3秒表示长按
			if( m_keytime == 300 )
			{
				m_key = key | T_L_X;
				m_keytime = 500;
			}
			m_old_key = key;
		}
	}
	else
	{
	  	if( (m_keytime > 0) && (m_keytime < 300) )
		{
			m_key = m_old_key;
		}
		if( m_keytime > 0 ) m_keytime = 0;
	}
    //3.按键处理
    if( m_key > T_None )
    {
        m_DevData.actived = ACTTIME;//检测状态//背光时间180秒
        m_DevData.devtim_5min = ESCTIME;//无操作自动退出时间60秒
        m_DevData.blink = BLINK_TIME;//背光时间60s
        //3.1.背光关闭先点亮背光
        if( (m_DevData.lcdonoff == 0) && (m_RunData.runmode > 0) )//m_RunData.runmode--//当前运行模式    lcdonoff;       //背光关闭
        {
            m_key = T_None;
            m_DevData.lcdonoff = 1;
        }
        //3.2.在开机状态，开启背光
        m_DevData.lcdonoff = 1;
    }
    //4.无操作自动退出
    if( m_DevData.devtim_5min == 1 )
    {
        if( m_key == T_None )
            m_key = T_AUTO_ESC;
         m_DevData.devtim_5min = 0;
    }                            
}
/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
