/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "adc.h"
#include "data.h"
#include "comm.h"
#include "usart.h"
#include "buzzer.h"
#include "flash.h"
#include "remote.h"
#include "buzzer.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*------------------------------------------------------------------------------
 * nrf24l01�ⲿ�ж�
 * EXTI3_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void EXTI3_IRQHandler(void)
{
    /*if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearFlag(EXTI_Line3); //����жϱ�־�����룩
        EXTI_ClearITPendingBit(EXTI_Line3);
        nrf24l01_interrupt(); //����жϴ������
     }*/
}

/*------------------------------------------------------------------------------
 * 1ms��ʱ���ж�
 * TIM1_UP_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void TIM1_UP_IRQHandler(void)
{
    if( TIM_GetITStatus( TIM1, TIM_IT_Update ) != RESET )
    {   
	  	//����жϱ�־λ
        TIM_ClearITPendingBit( TIM1, TIM_IT_Update );
        //1.ϵͳ��ʱ��
        if( m_timer_1ms > 0 )m_timer_1ms--;
        if( m_timer_10ms > 0 )m_timer_10ms--;
        if( m_timer_1000ms > 0 )m_timer_1000ms--; 
		//2.wifiģ�鶨ʱ��
		if( m_USART_CTRL[WIFI_UP].recvbusy  > 0 )m_USART_CTRL[WIFI_UP].recvbusy--;
		if( mMod_Chk[WIFI_UP].timeout > 1 )mMod_Chk[WIFI_UP].timeout--;
 		if( m_NetData.other_time > 0 )m_NetData.other_time--;
		//3.PM2.5ģ�鶨ʱ��
		if( m_USART_CTRL[PM25_DOWN].recvbusy  > 0 )m_USART_CTRL[PM25_DOWN].recvbusy--;
		if( mMod_Chk[PM25_DOWN].timeout > 1 )mMod_Chk[PM25_DOWN].timeout--;
        //4.��������λ��ʱ��
		if( m_DevData.time_reset > 1 )
		{
			m_DevData.time_reset--;
			if( m_DevData.time_reset == 1 )
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_1);
				m_DevData.time_reset = 0;
			}
		}
        //5.ң��֡���
        if( m_remote.busy > 0 ) m_remote.busy--;
        //6.��������ʱ��
        if( mBUZZER_TIME[0].vtime > 0 ) mBUZZER_TIME[0].vtime--;
        if( mBUZZER_TIME[0].ftime > 0 ) mBUZZER_TIME[0].ftime--;
    }
}

/*------------------------------------------------------------------------------
 * 0.1ms��ʱ���ж�
 * TIM3_IRQnHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void TIM2_IRQHandler(void)
{
	if( TIM_GetITStatus( TIM2, TIM_IT_Update ) != RESET )
	{
	  	//1.����жϱ�־λ
        TIM_ClearITPendingBit( TIM2, TIM_IT_Update );
        //2.Ƶ���źŵ�ƽ��ת
        if( mBUZZER_TIME[0].ftime > 0 )
            buzzer_freqturn(0);
        else
            buzzer_freqoff(0);
    }
}


/*------------------------------------------------------------------------------
 * ���벶���ж�
 * TIM4_IRQnHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void TIM4_IRQHandler(void)
{
    unsigned short cap1, cap2 = 0;
    
    //1.����C1�ж�
	if( TIM_GetITStatus( TIM4, TIM_IT_CC1 ) != RESET )
	{
        //1.����жϱ�־λ
        TIM_ClearITPendingBit( TIM4, TIM_IT_CC1 );
        //2.��ȡ��������ֵ
        cap1 = TIM_GetCapture1(TIM4);
        cap2 = TIM_GetCapture2(TIM4);
        //3.ͬ���ź�
        if( ((cap1 > 9800) && (cap1 < 9950)) && 
            ((cap2 > 300 ) && (cap2 < 450 )))
        {
            //3.1.������֡<200ms�Ĳ�����
            if( m_remote.busy == 0 )
            {
                m_remote.receivedbits = 0;
                m_remote.receive = 0;
                m_remote.status = 1;
            }
            else
            {
                m_remote.status = 0;
            }
        }
        //4.��ʼ��������
        else if( m_remote.status == 1 )  
        {
            //4.1.��ƽ�ж�
            if( (cap1 > 1000 ) && (cap1< 1450 ) ) 
            {
                m_remote.receive <<= 1;
                m_remote.receivedbits++;  
                //4.1.1.�ߵ�ƽ
                if ( (cap1-cap2) < cap2 )
                    m_remote.receive |= 1;
                //4.1.2.�������
                if( m_remote.receivedbits == 24 )
                {
                    m_remote.busy = 200;
                    m_remote.status = 2;
                    m_remote.keycode = m_remote.receive;
                }
            }
            else
            {
                m_remote.status = 0;
            }
        } 
        //6.����������
        TIM_SetCounter(TIM4,0);
    }
}
/*------------------------------------------------------------------------------
 * ����1�жϴ���
 * USART1_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void USART1_IRQHandler(void)
{
    USART_CTRL* pUc = &m_USART_CTRL[0];
    USART_TypeDef* USARTx = pUc->USARTx;
	
    //1.�ж��ж�
    if ((USARTx->SR & USART_FLAG_ORE) != (uint16_t)RESET)
    {
        USARTx->DR&0x01FF;
        return;
    }
    //2.�����жϴ���
    if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)
    {
        //2.1 �����ж����
        USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
        //2.2 �ַ����մ���
        pUc->recvbuf[pUc->recvidx] = USARTx->DR&0x01FF;
        pUc->recvidx++;
        pUc->recvbusy = 10;
        if( pUc->recvidx >= USART1_RECV_MAX )
            pUc->recvidx = 0;
    }
    //3.�����жϴ���
    else if(USART_GetITStatus(USARTx, USART_IT_TXE) != RESET)
    {
        //3.1 �ַ����ʹ���
        if( pUc->psendbuff > 0 )
            USARTx->DR = pUc->psendbuff[pUc->sendedlen];
        //3.2 ���ͳ��ȼ���
        pUc->sendedlen++;
        //3.3 ���ͽ����ж�
        if( pUc->sendedlen >= pUc->sendlen )
        {
            //3.4 �жϷ��ͱ�����
            USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
            //3.5 ���ݷ��س�ʱ����
            if( pUc->backlen > 0 )
                mMod_Chk[0].timeout = mMod_Chk[0].outtime;
            else
                mMod_Chk[0].timeout = 0;
            //3.6 �����������
            pUc->sendlen = 0;
            pUc->sendedlen = 0;
            pUc->sendbusy = 0;
            pUc->psendbuff = 0;
        }
    }
}

/*------------------------------------------------------------------------------
 * ����2�жϴ���
 *  USART2_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void USART2_IRQHandler(void)
{
    USART_CTRL* pUc = &m_USART_CTRL[1];
    USART_TypeDef* USARTx = pUc->USARTx;
	
    //1.�ж��ж�
    if ((USARTx->SR & USART_FLAG_ORE) != (uint16_t)RESET)
    {
        USARTx->DR&0x01FF;
        return;
    }
    //2.�����жϴ���
    if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)
    {
        //2.1 �����ж����
        USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
        //2.2 �ַ����մ���
        pUc->recvbuf[pUc->recvidx] = USARTx->DR&0x01FF;
        pUc->recvidx++;
        pUc->recvbusy = 10;
        if( pUc->recvidx >= USART2_RECV_MAX )
            pUc->recvidx = 0;
    }
    //3.�����жϴ���
    else if(USART_GetITStatus(USARTx, USART_IT_TXE) != RESET)
    {
        //3.1 �ַ����ʹ���
        if( pUc->psendbuff > 0 )
            USARTx->DR = pUc->psendbuff[pUc->sendedlen];
        //3.2 ���ͳ��ȼ���
        pUc->sendedlen++;
        //3.3 ���ͽ����ж�
        if( pUc->sendedlen >= pUc->sendlen )
        {
            //3.4 �жϷ��ͱ�����
            USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
            //3.5 ���ݷ��س�ʱ����
            if( pUc->backlen > 0 )
                mMod_Chk[1].timeout = mMod_Chk[1].outtime;
            else
                mMod_Chk[1].timeout = 0;
            //3.6 �����������
            pUc->sendlen = 0;
            pUc->sendedlen = 0;
            pUc->sendbusy = 0;
            pUc->psendbuff = 0;
        }
    }
}

/*------------------------------------------------------------------------------
 * ����3�жϴ���
 *  USART3_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void USART3_IRQHandler(void)
{
    /*if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(USART3);
        return;
    }
    unsigned idx = 2;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        m_USART_CTRL[idx].recvbuf[m_USART_CTRL[idx].recvidx] = USART_ReceiveData(USART3);
        m_USART_CTRL[idx].recvidx++;
        m_USART_CTRL[idx].recvbusy = 4;
        if( m_USART_CTRL[idx].recvidx >= USART_RECV_MAX )
            m_USART_CTRL[idx].recvidx = 0;
    }
    else if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {
        USART_SendData(USART3, m_USART_CTRL[idx].psendbuff[m_USART_CTRL[idx].sendedlen]);
        m_USART_CTRL[idx].sendedlen++;
        if( m_USART_CTRL[idx].sendedlen >= m_USART_CTRL[idx].sendlen )
        {
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
            if( m_USART_CTRL[idx].backlen > 0 )
                mMod_Chk[idx].timeout = 300;
            m_USART_CTRL[idx].sendlen = 0;
            m_USART_CTRL[idx].sendedlen = 0;
            m_USART_CTRL[idx].sendbusy = 0;
        }
    }*/
}

/*------------------------------------------------------------------------------
 * ����4�жϴ���
 *  USART4_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void UART4_IRQHandler(void)
{
    /*if (USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(UART4);
        return;
    }
    unsigned idx = 3;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
        m_USART_CTRL[idx].recvbuf[m_USART_CTRL[idx].recvidx] = USART_ReceiveData(UART4);
        m_USART_CTRL[idx].recvidx++;
        m_USART_CTRL[idx].recvbusy = 4;
        if( m_USART_CTRL[idx].recvidx >= USART_RECV_MAX )
            m_USART_CTRL[idx].recvidx = 0;
    }
    else if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
    {
        USART_SendData(UART4, m_USART_CTRL[idx].psendbuff[m_USART_CTRL[idx].sendedlen]);
        m_USART_CTRL[idx].sendedlen++;
        if( m_USART_CTRL[idx].sendedlen >= m_USART_CTRL[idx].sendlen )
        {
            USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
            if( m_USART_CTRL[idx].backlen > 0 )
                mMod_Chk[idx].timeout = 300;
            m_USART_CTRL[idx].sendlen = 0;
            m_USART_CTRL[idx].sendedlen = 0;
            m_USART_CTRL[idx].sendbusy = 0;
        }
    }*/
}

/*------------------------------------------------------------------------------
 * ����5�жϴ���
 *  USART5_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void UART5_IRQHandler(void)
{
    /*if (USART_GetFlagStatus(UART5, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(UART5);
        return;
    }
    unsigned idx = 4;
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(UART5, USART_IT_RXNE);
        m_USART_CTRL[idx].recvbuf[m_USART_CTRL[idx].recvidx] = USART_ReceiveData(UART5);
        m_USART_CTRL[idx].recvidx++;
        m_USART_CTRL[idx].recvbusy = 4;
        if( m_USART_CTRL[idx].recvidx >= USART_RECV_MAX )
            m_USART_CTRL[idx].recvidx = 0;
    }
    else if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
    {
        USART_SendData(UART5, m_USART_CTRL[idx].psendbuff[m_USART_CTRL[idx].sendedlen]);
        m_USART_CTRL[idx].sendedlen++;
        if( m_USART_CTRL[idx].sendedlen >= m_USART_CTRL[idx].sendlen )
        {
            USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
            if( m_USART_CTRL[idx].backlen > 0 )
                mMod_Chk[idx].timeout = 300;
            m_USART_CTRL[idx].sendlen = 0;
            m_USART_CTRL[idx].sendedlen = 0;
            m_USART_CTRL[idx].sendbusy = 0;
        }
    }*/
}

/*------------------------------------------------------------------------------
 * RTC���ж��ж�
 *  RTC_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void RTC_IRQHandler(void)
{
    //1.ϵͳ�߳��жϽ���
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);
        RTC_WaitForLastTask();
        m_DevData.devtim_now = RTC_GetCounter();
        m_DevData.devtim_second = 1;//����
        //��������
    }
}

/*------------------------------------------------------------------------------
 * �͵�ѹ�ж�,�������е�AD��������
 * PVD_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
void PVD_IRQHandler(void)
{
	//2.�жϱ�־λ
    if(EXTI_GetITStatus(EXTI_Line16) != RESET)
    {
        EXTI_ClearFlag(EXTI_Line16); //����жϱ�־�����룩
        EXTI_ClearITPendingBit(EXTI_Line16);
        BKP_WriteBackupRegister(BKP_DR2, (m_RunData.totaltime&0x0000ffff));
        BKP_WriteBackupRegister(BKP_DR3, ((m_RunData.totaltime>>16)&0x0000ffff));
     }
}

/*------------------------------------------------------------------------------
 * DMA�ж�,�������е�AD��������
 * DMA1_Channel1_IRQHandler
 * @���� ��
 *
 * @��� ��
 *
 -----------------------------------------------------------------------------*/
/*void DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA_IT_TC))
    {
        DMA_ClearITPendingBit(DMA_IT_TC);
        unsigned short smpcnt = m_ADC_CTRL.smpcnt;
        if( smpcnt < ADC_CNT )
        {
            unsigned char i;
            for( i = 0 ; i < ADC_CHNUM; i++ )
                m_ADC_CTRL.smv[i][smpcnt] = m_ADC_RegConvValTab[i];
            m_ADC_CTRL.smpcnt++;
        }
    }
}*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
