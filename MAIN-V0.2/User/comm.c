/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: usart.c
* 描    述: xxxx串口定义.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/1

* 修改历史: 时间       内容           修改者
            2017/6/1 :文件创建       王晶东
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "comm.h"
#include "data.h"
#include "comm_net.h"
#include "comm_pm25.h"


#define LEN(a,b,max)        (a>b?(a-b):(max+a-b))
#define NEXTi(idx,i,max)    ((idx+i)<max?(idx+i):(idx+i-max))

Mod_Chk mMod_Chk[COMM_MAX];

unsigned char mPackBuf[USART1_RECV_MAX];

//本地函数


void Comm_Init( void )
{
    /*------------------对上WIFI通讯----------------------*/
    mMod_Chk[WIFI_UP].puc = &m_USART_CTRL[WIFI_UP];
    mMod_Chk[WIFI_UP].status = 0;//自己定义
    mMod_Chk[WIFI_UP].mreqfunc = Com_Net_Req;//自己定义
    mMod_Chk[WIFI_UP].mrspfunc = Com_Net_Rsp;//自己定义
    mMod_Chk[WIFI_UP].mdelayfunc = 0;//自己定义
    /*-----------------对下PM2.5通讯---------------------*/
    mMod_Chk[PM25_DOWN].puc = &m_USART_CTRL[PM25_DOWN];
    mMod_Chk[PM25_DOWN].status = 0;//自己定义
    mMod_Chk[PM25_DOWN].mreqfunc = 0;//自己定义
    mMod_Chk[PM25_DOWN].mrspfunc = Com_PM25_Rsp;//自己定义
    mMod_Chk[PM25_DOWN].mdelayfunc = Com_PM25_Dly;//自己定义

    unsigned char i;
    for( i = 0; i < COMM_MAX; i++ )
    {
        mMod_Chk[i].addr = 0;
        mMod_Chk[i].index = 0;
        mMod_Chk[i].outtime = 0;
        mMod_Chk[i].timeout = 0;
    }
    mMod_Chk[PM25_DOWN].timeout = 5000;
}

//串口发送函数
void Com_Send(unsigned char id,unsigned char* psendbuff,unsigned short sendlen,
              unsigned short backlen,unsigned short timeout)
{
    if( mMod_Chk[id].puc->sendbusy == 0 )
    {
        mMod_Chk[id].puc->psendbuff = psendbuff;
        mMod_Chk[id].puc->sendlen = sendlen;
        mMod_Chk[id].puc->sendedlen = 0;
        mMod_Chk[id].puc->backlen = backlen;
        mMod_Chk[id].puc->sendbusy = 1;
        mMod_Chk[id].outtime = timeout;
        mMod_Chk[id].addr = psendbuff[0];
        //开启中断开始发送
        USART_ITConfig( mMod_Chk[id].puc->USARTx, USART_IT_TXE, ENABLE );
    }
}

//络数据校验算法
unsigned short calc_sum(unsigned char *inData, unsigned short inLen)
{
    unsigned int cksum = 0;
    unsigned char *p = inData;
	
    while (inLen > 0)
    {
        cksum += *p++;
        inLen--;
    }

    return (cksum & 0x0000ffff);
}

//PM2.5传感器校验算法
unsigned short calc_sum_pt( unsigned char* packet, unsigned char len )
{
  	unsigned char i;
	unsigned short checksum = 0;
	
	for( i = 0; i < len; i++ )
	{
		checksum += packet[i];
	}
	
	return checksum;
}

/*========================================================================
  * @brief  chk_rtu().
  * @param  -id:串口id
  * @param  -status:串口主从机分类
  * @param  -minium:最小接收长度
  * @param  -maxium:缓存最大值
  * @retval 0--无接收，1--接收超时 2-收到数据
  ========================================================================*/
int ComNet_check(unsigned char id, unsigned short min)
{
    Mod_Chk* pMc = &mMod_Chk[id];
    USART_CTRL* pUc = pMc->puc;
	
    //1.接收是否忙
    if(  pUc->recvbusy > 0  )
      return 0;
    //2.正常接收
    unsigned short index,recvlen;
    index = pMc->index;
    recvlen = pUc->recvidx;
    //3.接收空闲
    if( index == recvlen )
        return 0;
    recvlen = LEN(recvlen, index, USART1_RECV_MAX);
    if( recvlen < min )
        return 0;
	unsigned char* recvbuf = pUc->recvbuf;
	unsigned short check;
	unsigned short idx;
	for( idx = 0; idx < recvlen; idx++ )
		mPackBuf[idx] = recvbuf[NEXTi(index,idx,USART1_RECV_MAX)];
	check = (mPackBuf[recvlen-1] << 8) + mPackBuf[recvlen-2];
	if( calc_sum( mPackBuf, recvlen-2 ) == check )
	{
		if( pMc->mrspfunc != 0 )
		{
			pMc->mrspfunc( mPackBuf,id );
		}
	}
	pMc->timeout = 0;
	pMc->index = 0;
	pUc->recvidx = 0;  
	
    return 1;
}


//对PM2.5传感器通讯
int ComPM25_check(unsigned char id, unsigned short packetlen)
{
    unsigned char recvlen;
	Mod_Chk* pMc = &mMod_Chk[id];
    USART_CTRL* puc = pMc->puc;
	//1.接收是否忙
    if( puc->recvbusy > 0 )
      return 0;
    //2.接收超时
    if( pMc->timeout == 1 )
    {
        pMc->timeout = 5000;//5s超时
        puc->recvidx = 0;
        if( pMc->mdelayfunc != 0 )
            pMc->mdelayfunc( 0, id );
        return 0;
    }
    //3.数据接收长度
    recvlen = puc->recvidx;
    //4.接收到数据
    if( recvlen == packetlen )
    {
        //5.接收数据判断,校验成功
	  	unsigned char* recvbuf  = puc->recvbuf;
		unsigned short check = (recvbuf[recvlen-2]<<8)+recvbuf[recvlen-1];
		if( calc_sum_pt( recvbuf, recvlen-2) == check )
		{
			if( pMc->mrspfunc != 0 )
			{
			  	pMc->timeout = 5000;//5s超时
				pMc->mrspfunc( recvbuf,id );
			}
		}
		else
		{
        	pMc->timeout = 0;
		}
    }
	puc->recvidx = 0;
	
    return 1;
}

/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************/
