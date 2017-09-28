/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************
* 文件名称: usart.c
* 描    述: xxxx串口定义.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/1

* 修改历史: 时间       内容           修改者
            2017/6/1 :文件创建       王晶东
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "data.h"

USART_CTRL m_USART_CTRL[USART_MAX];

unsigned char m_USART1_RECV_BUFF[USART1_RECV_MAX];
unsigned char m_USART2_RECV_BUFF[USART2_RECV_MAX];
//初始化函数
void Usart_Init(void)
{
    unsigned char i;
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStructure;
    //波特率
	
    USART_TypeDef* USARTx[USART_MAX] = {USART1,USART2};//............////
	
    for( i = 0; i < USART_MAX; i++ )
    {
        USART_DeInit( USARTx[i] );//将外设 USARTx 寄存器重设为缺省值
		if( i == 0 )  //对上网络通信
		  	USART_InitStructure.USART_BaudRate = 9600;  //net通讯波特率
        else
		  	USART_InitStructure.USART_BaudRate = 9600;  //其他通信波特率    
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
        //开启中断接收使能
        USART_ITConfig( USARTx[i], USART_IT_RXNE, ENABLE);
    }
    //初始化
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
  * @param  -id:led串口id
  * @param  -pdata:发送数据
  * @param  -sendlen:发送长度
  * @param  -returnflag:返回表记:0x00无需返回，0x01需要返回
  * @retval 无
  ========================================================================*/
void Usart_Send(unsigned char id,unsigned char* psendbuff,unsigned short sendlen,unsigned char backlen)
{
    //是否busy
    if( m_USART_CTRL[id].sendbusy == 0 )//空闲
    {
        m_USART_CTRL[id].psendbuff = psendbuff;
        m_USART_CTRL[id].sendlen = sendlen;
        m_USART_CTRL[id].sendedlen = 0;
        m_USART_CTRL[id].backlen = backlen;
        m_USART_CTRL[id].sendbusy = 1;
        //开启中断开始发送
        USART_ITConfig( m_USART_CTRL[id].USARTx, USART_IT_TXE, ENABLE );
    }
}




/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
