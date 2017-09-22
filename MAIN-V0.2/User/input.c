/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: input.c
* ��    ��: input.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/1

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/1 :�ļ�����       ������
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "input.h"
#include "data.h"

typedef struct CINPUT_t
{
    GPIO_TypeDef*   GPIOx;  //GPIO
    unsigned short  GPIO_Pin;   //pin
}CINPUT;

const CINPUT mINPUT[MAX_INPUT] = 
{
    {GPIOA, GPIO_Pin_10},  //EMW_LED1

};

unsigned char m_inputBit[MAX_INPUT];
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


void input_init(void)   
{   
	unsigned char i = 0;
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    //input������
    for( i = 0; i < MAX_INPUT; i++ )
    {
        GPIO_InitStructure.GPIO_Pin   = mINPUT[i].GPIO_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
        GPIO_Init(mINPUT[i].GPIOx, &GPIO_InitStructure);
        //���������ʼ��
        m_inputBit[i] = 0;
    }
}  

//
void input_process( void )
{  
    unsigned char i,bit;
    
    for( i = 0; i < MAX_INPUT; i++ )
    {
        bit = GPIO_ReadInputDataBit(mINPUT[i].GPIOx,mINPUT[i].GPIO_Pin);
        if( bit == 0 )
        {
            if( m_inputBit[i] < 4 )
                m_inputBit[i]++;
        }
        else
        {
            if( m_inputBit[i] > 0 )
                m_inputBit[i]--;
        }
        //��⵽�����ź�
        if( m_inputBit[i] == 4 )
       		m_DevData.inputword |= (0x01 << i);          
        else if( m_inputBit[i] == 0 )//�źŶ�ʧ
           	m_DevData.inputword &= ~(0x01 << i);
    }  
}

/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************/
