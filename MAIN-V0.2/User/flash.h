/**
  ******************************************************************************
  * @file    flash.h
  * @author  王晶东
  * @version V1.0
  * @date    1/6/2017
  * @brief   flash操作模块头文件(读写操作)
  ******************************************************************************
  * @copy
  *
  * 片内flash操作(读和写)主要是存储modbus通讯配置信息。
  *
  */
#ifndef _FLASH_H
#define _FLASH_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* 全局宏定义 -------------------------------------------------------------*/

/* 全局变量声明 -----------------------------------------------------------*/

/* 全局函数声明 -----------------------------------------------------------*/
extern void flash_e(unsigned int,unsigned int);
extern void flash_w(unsigned short*,unsigned int,unsigned int);
extern void flash_r(unsigned char*,unsigned int,unsigned int);

#endif //_FLASH_H

/***************** COPYRIGHT 2017 杭州卡丽智能科技股份有限公司*****************/
