/*******************************************************************************
* File   :  input.h
* Author :  王晶东   
* Date   :  2017.6.1
* Version:  V01
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INPUT_H
#define __INPUT_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define	MAX_INPUT	1

/* Exported functions ------------------------------------------------------- */
extern void input_init(void);
extern void input_process(void);

#endif /* __INPUT_H */

/*************** (C) COPYRIGHT 2012 杭州卡丽智能科技股份有限公司 **************/
