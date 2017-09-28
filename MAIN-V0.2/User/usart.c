/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************
* �ļ�����: usart.c
* ��    ��: xxxx���ڶ���.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/1

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/1 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "data.h"

USART_CTRL m_USART_CTRL[USART_MAX];

unsigned char m_USART1_RECV_BUFF[USART1_RECV_MAX];
unsigned char m_USART2_RECV_BUFF[USART2_RECV_MAX];
//��ʼ������
void Usart_Init(void)
{
    unsigned char i;
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStructure;
    //������
	
    USART_TypeDef* USARTx[USART_MAX] = {USART1,USART2};//............////
	
    for( i = 0; i < USART_MAX; i++ )
    {
        USART_DeInit( USARTx[i] );//������ USARTx �Ĵ�������Ϊȱʡֵ
		if( i == 0 )  //��������ͨ��
		  	USART_InitStructure.USART_BaudRate = 9600;  //netͨѶ������
        else
		  	USART_InitStructure.USART_BaudRate = 9600;  //����ͨ�Ų�����    
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No ;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        //USART Clock
        USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
        USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
        USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
        USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
        USART_Init( USARTx[i], &USART_InitStructure);
        USART_ClockStructInit(&USART_ClockInitStructure);
        USART_ClockInit( USARTx[i],&USART_ClockInitStructure);
        /* Enable the USARTx */
        USART_Cmd( USARTx[i], ENABLE);
        //�����жϽ���ʹ��
        USART_ITConfig( USARTx[i], USART_IT_RXNE, ENABLE);
    }
    //��ʼ��
    for( i = 0; i < USART_MAX; i++ )
    {
        m_USART_CTRL[i].USARTx = USARTx[i];
        m_USART_CTRL[i].recvidx = 0;
        m_USART_CTRL[i].recvbusy = 0;
        m_USART_CTRL[i].psendbuff = 0;
        m_USART_CTRL[i].sendlen = 0;
        m_USART_CTRL[i].sendedlen = 0;
        m_USART_CTRL[i].backlen = 0;
        m_USART_CTRL[i].sendbusy = 0;
    }
    m_USART_CTRL[0].recvbuf = m_USART1_RECV_BUFF;
	m_USART_CTRL[1].recvbuf = m_USART2_RECV_BUFF;
}

/*========================================================================
  * @brief  usart_send().
  * @param  -id:led����id
  * @param  -pdata:��������
  * @param  -sendlen:���ͳ���
  * @param  -returnflag:���ر��:0x00���践�أ�0x01��Ҫ����
  * @retval ��
  ========================================================================*/
void Usart_Send(unsigned char id,unsigned char* psendbuff,unsigned short sendlen,unsigned char backlen)
{
    //�Ƿ�busy
    if( m_USART_CTRL[id].sendbusy == 0 )//����
    {
        m_USART_CTRL[id].psendbuff = psendbuff;
        m_USART_CTRL[id].sendlen = sendlen;
        m_USART_CTRL[id].sendedlen = 0;
        m_USART_CTRL[id].backlen = backlen;
        m_USART_CTRL[id].sendbusy = 1;
        //�����жϿ�ʼ����
        USART_ITConfig( m_USART_CTRL[id].USARTx, USART_IT_TXE, ENABLE );
    }
}




/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
