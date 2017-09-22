/**
  ******************************************************************************
  * @file    flash.c
  * @author  王晶东
  * @version V1.0
  * @date    1/6/2017
  * @brief   flash操作模块.c文件(读写操作)
  ******************************************************************************
  * @copy
  *
  * 片内flash操作(读和写)主要是存储modbus通讯配置信息。
  *
  */
/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* 局部宏定义 -------------------------------------------------------------*/
#define FLASH_PAGE_SIZE 0x800   //FLASH页大小

/* 局部变量定义 -----------------------------------------------------------*/
/* 局部函数声明 -----------------------------------------------------------*/


/*========================================================================
  * @brief  flash_e().
  * @param  -startaddress:写入数据起始地址(最好以页为单位)
  *         -lenghth:写入数据长度
  * @retval 无
  ========================================================================*/
void flash_e(unsigned int startaddress,unsigned int lenghth)
{
    unsigned int i;
    unsigned int numofpage = 0x0;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

    //1.解除写保护
    FLASH_Unlock();

    //2.Flash页数计算
    numofpage = lenghth/FLASH_PAGE_SIZE;
    if( lenghth%FLASH_PAGE_SIZE > 0 )
    {
        numofpage++;
    }
    
    //3.flash擦除
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    for( i = 0; (i < numofpage)&&(FLASHStatus == FLASH_COMPLETE);i++ )
    {
        FLASHStatus = FLASH_ErasePage(startaddress + (FLASH_PAGE_SIZE * i));
    }
}

/*========================================================================
  * @brief  flash_w().
  * @param  -pdata:写入数据(以字节为单位)
  *         -address:写入目标起始地址
  *         -lenghth:写入数据长度
  * @retval 无
  ========================================================================*/
void flash_w(unsigned short* pdata,unsigned int address,unsigned int lenghth)
{
    unsigned int i;

    //1.解除写保护
    lenghth = lenghth/2;
    for( i = 0; i < lenghth; i++ )
    {
        FLASH_ProgramHalfWord(address,pdata[i]);
        address+=2;
    }
}

/*========================================================================
  * @brief  flash_r().
  * @param  -pbuff:读取数据缓存
  *         -address:读取目标目标起始地址
  *         -lenghth:读取数据长度
  * @retval 无
  ========================================================================*/
void flash_r(unsigned char* pbuff,unsigned int address,unsigned int lenghth)
{
    unsigned int i;
    
    for( i = 0; i < lenghth; i++ )
    {
        *(pbuff+i) = *(unsigned char*)(address+i);
    }
}


/***************COPYRIGHT 2017 杭州卡丽智能科技股份有限公司********************/
