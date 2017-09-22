/*******************************************************************************
* File   :  key.h
* Author :  王晶东   
* Date   :  2017.6.1
* Version:  V0.1
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_H
#define __KEY_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


#define KEY_INPUT     	1

#define	KEY_NON			0x00
#define KEY_SHORT	    0x01
#define KEY_LONG		0x10



/* Exported functions ------------------------------------------------------- */
extern void key_init(void);
extern void key_process(void);

#endif /* __LCD_H */

/************* (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************/
