/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: tm1650.c
* 描    述: LED和键盘驱动.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/8/8

* 修改历史: 时间       内容           修改者
            2017/8/8 :文件创建       王晶东
*******************************************************************************/
//-- Includes ------------------------------------------------------------------
#include "tm1650.h"

const unsigned char m_keycode[4][7] = { 0x44, 0x4C, 0x54, 0x5C, 0x64, 0x6C, 0x74,
                                        0x45, 0x4D, 0x55, 0x5D, 0x65, 0x6D, 0x75,
                                        0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E, 0x76,
                                        0x47, 0x4F, 0x57, 0x5F, 0x67, 0x6F, 0x77,
                                       };

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
etError tm1650_WriteConfig(unsigned char id, unsigned char config );

/*******************************************************************************
* 函数名称  : tm1650_Init
* 描    述  : 初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void tm1650_Init(unsigned char id)
{  
    // init I2C
	I2c_Init( id ); 
}

/*******************************************************************************
* 函数名称  : tm1650_ReadKeycode
* 描    述  : 读取按键值
* 输    入  : id:id号，status:按键值
* 输    出  : None
* 返    回  : None
*******************************************************************************/
etError tm1650_ReadKeycode(unsigned char id, unsigned char *status )
{
	etError error; // error code
	
    // write a start condition
	I2c_StartCondition( id );
    // write the cmd
	error = I2c_WriteByte(id, CMD_RD_KEY );
	// if no error, read status
	error = I2c_ReadByte(id, status, NACK, 0);
	// write a stop condition
	I2c_StopCondition( id );
	
	return error;
}

/*******************************************************************************
* 函数名称  : tm1650_WriteConfig
* 描    述  : 配置命令
* 输    入  : id:id号，config:配置参数
* 输    出  : None
* 返    回  : None
*******************************************************************************/
etError tm1650_WriteConfig(unsigned char id, unsigned char config )
{	
    etError error; // error code
    
    // write a start condition
	I2c_StartCondition( id );
    // write the cmd
	error = I2c_WriteByte(id, CMD_MODE );	
    // write the config data
    error = I2c_WriteByte(id, config );	
	// write a stop condition
	I2c_StopCondition( id );

    return error;
}

/*******************************************************************************
* 函数名称  : tm1650_WriteDisplay
* 描    述  : 对应地址写入显示数据
* 输    入  : id:id号，addr:显示地址，display:显示数据
* 输    出  : None
* 返    回  : None
*******************************************************************************/
etError tm1650_WriteDisplay(unsigned char id, unsigned char addr, unsigned char display )
{
	etError error; // error code
    
    // write a start condition
	I2c_StartCondition( id );
    // write the addr
	error = I2c_WriteByte(id, addr );	 
    // write the display data
	error = I2c_WriteByte(id, display );	
	// write a stop condition
	I2c_StopCondition( id );

    return error;
}

// =============(C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司=================