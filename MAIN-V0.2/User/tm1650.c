/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: tm1650.c
* ��    ��: LED�ͼ�������.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/8/8

* �޸���ʷ: ʱ��       ����           �޸���
            2017/8/8 :�ļ�����       ������
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
* ��������  : tm1650_Init
* ��    ��  : ��ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void tm1650_Init(unsigned char id)
{  
    // init I2C
	I2c_Init( id ); 
}

/*******************************************************************************
* ��������  : tm1650_ReadKeycode
* ��    ��  : ��ȡ����ֵ
* ��    ��  : id:id�ţ�status:����ֵ
* ��    ��  : None
* ��    ��  : None
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
* ��������  : tm1650_WriteConfig
* ��    ��  : ��������
* ��    ��  : id:id�ţ�config:���ò���
* ��    ��  : None
* ��    ��  : None
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
* ��������  : tm1650_WriteDisplay
* ��    ��  : ��Ӧ��ַд����ʾ����
* ��    ��  : id:id�ţ�addr:��ʾ��ַ��display:��ʾ����
* ��    ��  : None
* ��    ��  : None
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

// =============(C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾=================