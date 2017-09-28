#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f10x.h"

#define ADC_CHNUM       1  //ADCת��ͨ����Ŀ
#define ADC_SAMPLE      10//ADC��������
//#define ADC_CNT     250
//#define ADC1_DR_Address    ((uint32_t)0x4001244C)

//extern  volatile unsigned short m_ADC_RegConvValTab[ADC_CHNUM];
//extern  volatile unsigned short m_ADC_InjConvSmv[ADC_CNT];
//extern  volatile unsigned short m_ADC_InjConvSmpCnt;
//extern  volatile unsigned short m_ADC_InjConvArgSmv;

typedef struct ADC_CTRL_t
{
    unsigned char   smpcnt;             //��������
    unsigned char   busy;               //����״̬
    unsigned int    samplesum;          //�����ܺ�
    unsigned short  argsmv;             //������ֵ
}ADC_CTRL;

extern ADC_CTRL m_ADC_CTRL;

extern void Adc_Init(void);
extern unsigned char adc_check(void);
/*void Get_InjectAdcValue(void);
unsigned char Adc_Check(void);*/
//unsigned char Adc_Check( unsigned char id );


#endif //_ADC_H_

/**************(C) COPYRIGHT 2012 ������*****End of this file*****************/
