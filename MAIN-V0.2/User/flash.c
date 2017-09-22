/**
  ******************************************************************************
  * @file    flash.c
  * @author  ������
  * @version V1.0
  * @date    1/6/2017
  * @brief   flash����ģ��.c�ļ�(��д����)
  ******************************************************************************
  * @copy
  *
  * Ƭ��flash����(����д)��Ҫ�Ǵ洢modbusͨѶ������Ϣ��
  *
  */
/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* �ֲ��궨�� -------------------------------------------------------------*/
#define FLASH_PAGE_SIZE 0x800   //FLASHҳ��С

/* �ֲ��������� -----------------------------------------------------------*/
/* �ֲ��������� -----------------------------------------------------------*/


/*========================================================================
  * @brief  flash_e().
  * @param  -startaddress:д��������ʼ��ַ(�����ҳΪ��λ)
  *         -lenghth:д�����ݳ���
  * @retval ��
  ========================================================================*/
void flash_e(unsigned int startaddress,unsigned int lenghth)
{
    unsigned int i;
    unsigned int numofpage = 0x0;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

    //1.���д����
    FLASH_Unlock();

    //2.Flashҳ������
    numofpage = lenghth/FLASH_PAGE_SIZE;
    if( lenghth%FLASH_PAGE_SIZE > 0 )
    {
        numofpage++;
    }
    
    //3.flash����
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    for( i = 0; (i < numofpage)&&(FLASHStatus == FLASH_COMPLETE);i++ )
    {
        FLASHStatus = FLASH_ErasePage(startaddress + (FLASH_PAGE_SIZE * i));
    }
}

/*========================================================================
  * @brief  flash_w().
  * @param  -pdata:д������(���ֽ�Ϊ��λ)
  *         -address:д��Ŀ����ʼ��ַ
  *         -lenghth:д�����ݳ���
  * @retval ��
  ========================================================================*/
void flash_w(unsigned short* pdata,unsigned int address,unsigned int lenghth)
{
    unsigned int i;

    //1.���д����
    lenghth = lenghth/2;
    for( i = 0; i < lenghth; i++ )
    {
        FLASH_ProgramHalfWord(address,pdata[i]);
        address+=2;
    }
}

/*========================================================================
  * @brief  flash_r().
  * @param  -pbuff:��ȡ���ݻ���
  *         -address:��ȡĿ��Ŀ����ʼ��ַ
  *         -lenghth:��ȡ���ݳ���
  * @retval ��
  ========================================================================*/
void flash_r(unsigned char* pbuff,unsigned int address,unsigned int lenghth)
{
    unsigned int i;
    
    for( i = 0; i < lenghth; i++ )
    {
        *(pbuff+i) = *(unsigned char*)(address+i);
    }
}


/***************COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾********************/
