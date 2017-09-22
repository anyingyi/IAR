//==============================================================================
//    杭州卡丽智能科技股份有限公司 
//==============================================================================
// Project   :  I2C_HAL 模拟I2C
// File      :  i2c_hal.h (V1.0)
// Author    :  RFU
// Date      :  1-6-2017
// Controller:  
// IDE       :  
// Compiler  :  王晶东
// Brief     :  I2C hardware abstraction layer 
//==============================================================================

#ifndef I2C_HAL_H
#define I2C_HAL_H

//-- Includes ------------------------------------------------------------------
#include "data.h"

//-- Enumerations --------------------------------------------------------------

// I2C acknowledge
typedef enum{
  ACK  = 0,
  NACK = 1,
}etI2cAck;

typedef enum{
  NO_ERROR       = 0x00, // no error
  ACK_ERROR      = 0x01, // no acknowledgment error
  CHECKSUM_ERROR = 0x02, // checksum mismatch error
  TIMEOUT_ERROR  = 0x04, // timeout error
  PARM_ERROR     = 0x80, // parameter out of range error
}etError;

#define	 ID_NIXIE	0
#define  ID_LED	    1

//==============================================================================
void I2c_Init( unsigned char sensor );
//==============================================================================
// Initializes the ports for I2C interface.
//------------------------------------------------------------------------------

//==============================================================================
void I2c_StartCondition( unsigned char sensor );
//==============================================================================
// Writes a start condition on I2C-Bus.
//------------------------------------------------------------------------------
// remark: Timing (delay) may have to be changed for different microcontroller.
//       _____
// SDA:       |_____
//       _______
// SCL:         |___

//==============================================================================
void I2c_StopCondition( unsigned char sensor );
//==============================================================================
// Writes a stop condition on I2C-Bus.
//------------------------------------------------------------------------------
// remark: Timing (delay) may have to be changed for different microcontroller.
//              _____
// SDA:   _____|
//            _______
// SCL:   ___|

//==============================================================================
etError I2c_WriteByte( unsigned char sensor, unsigned char txByte );
//==============================================================================
// Writes a byte to I2C-Bus and checks acknowledge.
//------------------------------------------------------------------------------
// input:  txByte       transmit byte
//
// return: error:       ACK_ERROR = no acknowledgment from sensor
//                      NO_ERROR  = no error
//
// remark: Timing (delay) may have to be changed for different microcontroller.

//==============================================================================
etError I2c_ReadByte( unsigned char sensor, unsigned char *rxByte, etI2cAck ack, unsigned char timeout);
//==============================================================================
// Reads a byte on I2C-Bus.
//------------------------------------------------------------------------------
// input:  ack          Acknowledge: ACK or NACK
//
// return: rxByte
//
// remark: Timing (delay) may have to be changed for different microcontroller.

//==============================================================================
etError I2c_WaitWhileClockStreching( unsigned char sensor, unsigned char timeout );
//==============================================================================

etError I2c_GeneralCallReset( unsigned char sensor );

#endif
