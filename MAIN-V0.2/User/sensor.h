/*******************************************************************************
* File   :  sensor.h
* Author :  allen   
* Date   :  2017.6.1
* Version:  V01
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSOR_H
#define __SENSOR_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define PM25_MODE_STOP 	0x00
#define PM25_MODE_RUN 	0x01

#define SENSOR_PM25 	0x00
#define SENSOR_CO2 		0x01

#define TIME_RESET		1000
#define PM25_PERIOD		180 	//180s
#define PM25_TEST		60  	//60s


/* Exported functions --------
----------------------------------------------- */
extern void sensor_init( void );
extern void pm25_mode( unsigned char mode );
extern void sensor_wkup( void );
extern void pm25_process( void );
extern void sensor_reset( unsigned char id );
#endif /* __SENSOR_H */

/******************* (C) COPYRIGHT 2008 HZZH *****END OF FILE****/
