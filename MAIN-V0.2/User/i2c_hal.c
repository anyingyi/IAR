//==============================================================================
// =============(C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司=================
//==============================================================================
// Project   :  I2C Code (V1.0)
// File      :  i2c_hal.c (V1.0)
// Author    :  
// Date      :  1-6-2017
// Controller:  
// IDE       :  
// Compiler  :  王晶东
// Brief     :  I2C hardware abstraction layer
//==============================================================================

//-- Includes ------------------------------------------------------------------
#include "i2c_hal.h"

//-- Defines -------------------------------------------------------------------
//NIXIE
// SDA on port B, bit11 bit8
#define SDA_LOW(m)  	(GPIOB->BSRR = m?0x01000000:0x08000000) // set SDA to low
#define SDA_OPEN(m) 	(GPIOB->BSRR = m?0x00000100:0x00000800) // set SDA to open-drain
#define SDA_READ(m)   	(GPIOB->IDR & (m?0x0100:0x0800))     // read SDA

// SCL on port B, bit7 bit9            /* -- adapt the defines for your uC -- */
#define SCL_LOW(m)  	(GPIOB->BSRR = m?0x02000000:0x04000000) // set SCL to low
#define SCL_OPEN(m) 	(GPIOB->BSRR = m?0x00000200:0x00000400) // set SCL to open-drain
#define SCL_READ(m)   	(GPIOB->IDR & (m?0x0200:0x0400))     // read SCL

//==============================================================================
void I2c_Init( unsigned char sensor ){                      /* -- adapt the init for your uC -- */
//==============================================================================
	SDA_OPEN(sensor);                  // I2C-bus idle mode SDA released
	SCL_OPEN(sensor);                  // I2C-bus idle mode SCL released
	if( sensor == ID_NIXIE )
	{
		// SDA on port B, bit 10
		// SCL on port B, bit 11
		GPIOB->CRH   &= 0xFFFF00FF;  // set open-drain output for SDA and SCL
		GPIOB->CRH   |= 0x00005500;  // 
	}
	else
	{
		// SDA on port B, bit 8
		// SCL on port B, bit 9
		GPIOB->CRH   &= 0xFFFFFF00;  // set open-drain output for SDA and SCL
		GPIOB->CRH   |= 0x00000055;  // 
	}
}

//==============================================================================
void I2c_StartCondition( unsigned char sensor ){
//==============================================================================
	
	SDA_OPEN(sensor);
	DelayMicroSeconds(1);
	SCL_OPEN(sensor);
	DelayMicroSeconds(1);
	SDA_LOW(sensor);
	DelayMicroSeconds(10);  // hold time start condition (t_HD;STA)
	SCL_LOW(sensor);
	DelayMicroSeconds(10);
}

//==============================================================================
void I2c_StopCondition( unsigned char sensor ){
//==============================================================================
	SCL_LOW(sensor);
	DelayMicroSeconds(1);
	SDA_LOW(sensor);
	DelayMicroSeconds(1);
	SCL_OPEN(sensor);
	DelayMicroSeconds(10);  // set-up time stop condition (t_SU;STO)
	SDA_OPEN(sensor);
	DelayMicroSeconds(10);
}

//==============================================================================
etError I2c_WriteByte( unsigned char sensor, unsigned char txByte ){
//==============================================================================
	etError error = NO_ERROR;
	unsigned char     mask;
	for(mask = 0x80; mask > 0; mask >>= 1)// shift bit for masking (8 times)
	{
		if((mask & txByte) == 0) SDA_LOW(sensor); // masking txByte, write bit to SDA-Line
		else                     SDA_OPEN(sensor);
		DelayMicroSeconds(1);               // data set-up time (t_SU;DAT)
		SCL_OPEN(sensor);                         // generate clock pulse on SCL
		DelayMicroSeconds(5);               // SCL high time (t_HIGH)
		SCL_LOW(sensor);
		DelayMicroSeconds(1);               // data hold time(t_HD;DAT)
	}
	SDA_OPEN(sensor);                           // release SDA-line
	SCL_OPEN(sensor);                           // clk #9 for ack
	DelayMicroSeconds(1);                 // data set-up time (t_SU;DAT)
	if(SDA_READ(sensor)) error = ACK_ERROR;       // check ack from i2c slave
	SCL_LOW(sensor);
	DelayMicroSeconds(10);                // wait to see byte package on scope
	return error;                         // return error code
}

//==============================================================================
etError I2c_ReadByte( unsigned char sensor, unsigned char *rxByte, etI2cAck ack, unsigned char timeout){
//==============================================================================
	etError error = NO_ERROR;
	unsigned char mask;
	*rxByte = 0x00;
	SDA_OPEN(sensor);                            // release SDA-line
	for(mask = 0x80; mask > 0; mask >>= 1) // shift bit for masking (8 times)
	{ 
		SCL_OPEN(sensor);                          // start clock on SCL-line
		DelayMicroSeconds(1);                // clock set-up time (t_SU;CLK)
		error = I2c_WaitWhileClockStreching(sensor, timeout);// wait while clock streching
		DelayMicroSeconds(3);                // SCL high time (t_HIGH)
		if(SDA_READ(sensor)) *rxByte |= mask;        // read bit
		SCL_LOW(sensor);
		DelayMicroSeconds(1);                // data hold time(t_HD;DAT)
	}
	if(ack == ACK) SDA_LOW(sensor);              // send acknowledge if necessary
	else           SDA_OPEN(sensor);
	DelayMicroSeconds(1);                  // data set-up time (t_SU;DAT)
	SCL_OPEN(sensor);                            // clk #9 for ack
	DelayMicroSeconds(5);                  // SCL high time (t_HIGH)
	SCL_LOW(sensor);
	SDA_OPEN(sensor);                            // release SDA-line
	DelayMicroSeconds(20);                 // wait to see byte package on scope
	
	return error;                          // return with no error
}

//==============================================================================
etError I2c_WaitWhileClockStreching( unsigned char sensor, unsigned char timeout ){
//==============================================================================
	etError error = NO_ERROR;
	
	while(SCL_READ(sensor) == 0)
	{
		if(timeout-- == 0)	return TIMEOUT_ERROR;
		DelayMicroSeconds(1000);
	}
	
	return error;
}

//==============================================================================
etError I2c_GeneralCallReset( unsigned char sensor ){
//==============================================================================
	etError error;
	
	I2c_StartCondition(sensor);
	error = I2c_WriteByte(sensor, 0x00);
	if(error == NO_ERROR) error = I2c_WriteByte(sensor, 0x06);
	
	return error;
}

// =============(C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司=================
