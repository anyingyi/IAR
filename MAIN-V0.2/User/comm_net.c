/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: comm_net.c
* 描    述: xxxx.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/1

* 修改历史: 时间       内容           修改者
            2017/6/1  :文件创建       王晶东
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "comm_net.h"
#include "comm.h"
#include "data.h"
#include "stdlib.h"
#include "rtc.h"
#include "flash.h"
#include "warn.h"
#include "sysctrl.h"
#include "ledctrl.h"
#include "fanctrl.h"
#include "sysctrl.h"


/************************************协议包结构********************************
|起始标志   |   命令    |  命令返回状态    |   数据长度   |  数据   |   校验    |
--------------------------------------------------------------------------------
| 0x5446    | 16bit数据 |    16bit数据     |   16bit数据  |  数据   | 16bit数据 |
| 'F''T'    | 小端模式  |    小端模式      |    小端模式  |         | 小端模式  |
*******************************************************************************/

#define TIME_NET_PERIOD				100
#define TIME_STATUS_PERIOD			2000
#define TIME_PARA_PERIOD			10  //10s上报一次
#define TIME_STATUS_PERIOD2			30  //30s读取一次状态

#define	MAX_CURRENT_STATUS			84
//#define	LEN_AP_LIST_INFOEM			332 

//协议命令
#define CMD_READ_VERSION			0x0001
#define CMD_READ_CONFIG				0x0002
#define	CMD_WRITE_CONFIG			0x0003
#define	CMD_SCAN					0x0004
#define	CMD_OTA						0x0005
#define	CMD_NET2COM					0x0006
#define	CMD_COM2NET					0x0007
#define	CMD_GET_STATUS				0x0008
#define	CMD_CONTROL					0x0009
#define CMD_GET_APLIST				0x000A
#define CMD_UNBUNDLING				0x000E
#define	CMD_CHECK				    0x000F
#define CMD_SET_NET                 0x0010
#define	CMD_UP_STATUS				0x0066

//数据命令
#define CMD_NONE					0x00
#define CMD_UP_HEART				0x01
#define CMD_UP_REAL					0x02
#define CMD_UP_WARN					0x03
#define CMD_UP_PARA					0x04
#define CMD_DOWN_PARA				0x05
#define CMD_OK_PARA					0x06
#define CMD_READ_REAL				0x07
#define CMD_RTN_REAL				0x08
#define CMD_READ_PARA				0x09
#define CMD_RTN_PARA				0x0A
#define CMD_DOWN_RECALL				0x0B
#define CMD_DOWN_ONOFF				0x0C
#define CMD_DOWN_RESET				0x0D
#define CMD_DOWN_UPDATE				0x0E
#define CMD_ADJUST					0x0F
#define CMD_STATUS_RTN				0x10
#define CMD_ALL_RTN					0xF0

#define MAX_SETTIME_START           54        //定时设置起始字节
#define MAX_SETTIME_END             96        //定时设置终止位置
#define MAX_SET_END                 96        //设置终止位置


unsigned char m_net_Sendbuff[512];
unsigned short m_set_srart, m_set_bytes;

unsigned char const Code[256] = 
{   
    //加密随机串
    0x00,0x5E,0xBC,0xE2,0x61,0x3F,0xDD,0x83,0xC2,0x9C,0x7E,0x20,0xA3,0xFD,0x1F,0x41,
    0x9D,0xC3,0x21,0x7F,0xFC,0xA2,0x40,0x1E,0x5F,0x01,0xE3,0xBD,0x3E,0x60,0x82,0xDC,
    0x23,0x07,0x9F,0xC1,0x42,0x1C,0xFE,0xA0,0xE1,0xBF,0x5D,0x03,0x80,0xDE,0x3C,0x62,
    0xBE,0xE0,0x02,0x5C,0xDF,0x81,0x63,0x3D,0x7C,0x22,0xC0,0x9E,0x1D,0x43,0xA1,0xFF,
    0x46,0x18,0xFA,0xA4,0x27,0x79,0x9B,0xC5,0x84,0xDA,0x38,0x66,0xE5,0xBB,0x59,0x07,
    0xDB,0x85,0x67,0x39,0xBA,0xE4,0x06,0x58,0x19,0x47,0xA5,0xFB,0x78,0x26,0xC4,0x9A,
    0x65,0x3B,0xD9,0x87,0x04,0x5A,0xB8,0xE6,0xA7,0xF9,0x1B,0x45,0xC6,0x98,0x7A,0x24,
    0xF8,0xA6,0x44,0x1A,0x99,0xC7,0x25,0x7B,0x3A,0x64,0x86,0xD8,0x5B,0x05,0xE7,0xB9,
    0x8C,0xD2,0x30,0x6E,0xED,0xB3,0x51,0x0F,0x4E,0x10,0xF2,0xAC,0x2F,0x71,0x93,0xCD,
    0x11,0x4F,0xAD,0xF3,0x70,0x2E,0xCC,0x92,0xD3,0x8D,0x6F,0x31,0xB2,0xEC,0x0E,0x50,
    0xAF,0xF1,0x13,0x4D,0xCE,0x90,0x72,0x2C,0x6D,0x33,0xD1,0x8F,0x0C,0x52,0xB0,0xEE,
    0x32,0x6C,0x8E,0xD0,0x53,0x0D,0xEF,0xB1,0xF0,0xAE,0x4C,0x12,0x91,0xCF,0x2D,0x73,
    0xCA,0x94,0x76,0x28,0xAB,0xF5,0x17,0x49,0x08,0x56,0xB4,0xEA,0x69,0x37,0xD5,0x8B,
    0x57,0x09,0xEB,0xB5,0x36,0x68,0x8A,0xD4,0x95,0xCB,0x29,0x77,0xF4,0xAA,0x48,0x16,
    0xE9,0xB7,0x55,0x0B,0x88,0xD6,0x34,0x6A,0x2B,0x75,0x97,0xC9,0x4A,0x14,0xF6,0xA8,
    0x74,0x2A,0xC8,0x96,0x15,0x4B,0xA9,0xF7,0xB6,0xE8,0x0A,0x54,0xD7,0x89,0x6B,0x35,
};



//加密函数
void WriteCode(unsigned char TCode,unsigned char *Data,unsigned int Seq)
{
#ifdef _ENCRYPT
	unsigned int i;
	for(i = 0; i < Seq; i ++)
	{
		Data[i] ^= TCode;
		Data[i] ^= Code[i % 256];
	}
#endif
}

//数据部分帧打包,返回数据长度
unsigned short pachage_make( unsigned char* buf, unsigned char cmd, unsigned char data )
{
  	unsigned short i,len;
	unsigned char* p;
	unsigned short length = 0;
    unsigned short tran;
	
	if( cmd == CMD_NONE )
		return 0;
	//1.命令
	buf[0] = cmd;
	//2.序列号
	for( i = 0; i < 8; i++ )
		buf[i+1] = m_SetData.serialnumber[i]; 
	//3.包序号跳过
	//4.产生加密字随机数0-255
	srand((unsigned) time(NULL));
	buf[10] = rand()%256;
	switch( cmd )
	{
	case CMD_UP_HEART:  //心跳包
		buf[9] = 0; 
		buf[11] = m_RunData.status & 0x00ff; 
		buf[12] = (m_RunData.status>>8) & 0x00ff; 
		WriteCode(buf[10], &buf[11], 2);
		length = 13;
		break;
	case CMD_UP_REAL:  //实时数据
	case CMD_RTN_REAL: //实时数据返回
	  	if( cmd == CMD_UP_REAL)  
	  		buf[9] = 0; 
		else
			buf[9] = m_NetData.packagenum;
		p = (unsigned char*)&m_RunData.status;
		len = 60;
		for( i = 0; i < 8; i++ )
		{
			buf[11+i] = *p;
			p++;
		}
        //温度数据去掉小数
        tran = m_RunData.temp/100;
        buf[19] = tran&0xff;
        buf[20] = (tran>>8)&0xff;
        tran = m_RunData.humi/100;
        buf[21] = tran&0xff;
        buf[22] = (tran>>8)&0xff;
        //其他数据
        p = (unsigned char*)&m_RunData.co2;
        for( i = 0; i < 10; i++ )
        {
			buf[23+i] = *p;
			p++;
        }
        p = (unsigned char*)&m_RunData.totaltime;
        for( i = 0; i < 38; i++ )
        {
			buf[33+i] = *p;
			p++;
        }
		WriteCode(buf[10], &buf[11], len);
		length = 11+len;
		break;
	case CMD_UP_PARA:  //参数上报
	  	buf[9] = 0; 
        for( i = 0; i < 7; i++ )
        {
            tran = m_SetData.onofftime[i][1];
            buf[11+4*i] = tran&0x00ff;
            buf[12+4*i] = (tran>>8)&0x00ff;
            tran = m_SetData.onofftime[i][0];
            buf[13+4*i] = tran&0x00ff;
            buf[14+4*i] = (tran>>8)&0x00ff;
        }
        for( i = 0; i < 7; i++ )
        {
            tran = m_SetData.setmode[i];
            buf[39+2*i] = tran&0x00ff;
            buf[40+2*i] = (tran>>8)&0x00ff;
        }
        len = 42;
		WriteCode(buf[10], &buf[11], len);
		length = 11+len;
		break;
	case CMD_RTN_PARA:  //参数数据返回
		buf[9] = m_NetData.packagenum;  
		p = (unsigned char*)&m_SetData.serialnumber[0];
		//偏移量
		p += m_set_srart;
        buf[11] = m_set_srart&0x00ff;
        buf[12] = (m_set_srart>>8)&0x00ff;
        buf[13] = m_set_bytes&0x00ff;
        buf[14] = (m_set_bytes>>8)&0x00ff;
		for( i = 0; i < m_set_bytes; i++ )
		{
			buf[15+i] = *p;
			p++;
		}
		WriteCode(buf[10], &buf[11], m_set_bytes+4);
		length = 15+m_set_bytes;
		break;
	case CMD_DOWN_ONOFF:  //开关机数据返回	
	case CMD_DOWN_RESET:  //重启数据返回	
	  	buf[9] = 0; 
		buf[11] = data; //成功
		WriteCode(buf[10], &buf[11], 1);
		length = 12;
	  	break;
	case CMD_OK_PARA:  //参数下发返回数据
	case CMD_ALL_RTN:  //其他回复
	  	buf[9] = m_NetData.packagenum; 
		buf[11] = data;
		WriteCode(buf[10], &buf[11], 1);
		length = 12;
		break;
	default:
		break;
	}
	
	return length;
}

//帧数据格式
unsigned short Frame_make( unsigned char* buf, unsigned short cmd_frame, unsigned char cmd_package, unsigned char data )
{
  	unsigned short len;
	unsigned int ck;
	
	/*buf[0] = 0x46;
	buf[1] = 0x54;*/
 	
	buf[0] = 0x54;
	buf[1] = 0x46;
	buf[2] = cmd_frame&0x00ff;
	buf[3] = (cmd_frame>>8)&0x00ff;
	buf[4] = 0x00;
	buf[5] = 0x00;
	//1.设置wifi参数命令
	if( cmd_frame == CMD_WRITE_CONFIG )
	{
		;
	}
	else //数据信息
 		len = pachage_make( &buf[8], cmd_package, data );
	//数据长度
	buf[6] = len&0x00ff;
	buf[7] = (len>>8)&0x00ff;
	ck = calc_sum( buf, (len+8) );
	buf[8+len] = ck&0x00ff;
	buf[9+len] = (ck>>8)&0x00ff;
	
	return (10+len);
}

/******************************************************************************
//  对上发送数据信息
*******************************************************************************/
int Com_Net_Req( unsigned char* buf,unsigned char id )
{
  	unsigned short sendlen = 0;
	unsigned char ok = 0,cmd = 0;
	

  	//1.有及时数据需要回复或上报
  	//参数回复、实时数据返回、参数数据返回、告警上报、参数上报
	if( (m_NetData.other_time == 0) && ( m_NetData.status < 4 ) )
	{
	  	//3.远程服务器未连接成功
	  	if( (!m_NetData.connect) || m_NetData.status_rt || m_NetData.setnet )
		{
            if( m_NetData.setnet )
            {
		  	    sendlen = Frame_make( m_net_Sendbuff, CMD_SET_NET, 0, 0 );
                m_NetData.setnet = 0;
                m_NetData.other_time = sendlen+TIME_NET_PERIOD;
            }
            else if( m_NetData.status_rt )
            {
                sendlen = Frame_make( m_net_Sendbuff, CMD_UP_STATUS, 0, 0 );
                m_NetData.status_rt = 0;
                m_NetData.other_time = sendlen+TIME_NET_PERIOD;
            }
            else
            {
		  	    sendlen = Frame_make( m_net_Sendbuff, CMD_GET_STATUS, 0, 0 );
			    m_NetData.other_time  = TIME_STATUS_PERIOD;
                m_NetData.err_num++;
                m_NetData.bit_rsp |= 0x0008;
                m_NetData.connect = 0;
            }

		}
		//4.正常数据发送
		else
		{
			if( m_NetData.bit_rsp > 0 )
			{
				//1.参数下发正确回复
				if( (m_NetData.bit_rsp & 0x0001) == 0x0001 )
				{
					cmd = CMD_OK_PARA;
					ok = 1;
					m_NetData.bit_rsp &= ~0x0001;
				}
				//2.实时数据返回
				else if( (m_NetData.bit_rsp & 0x0002) == 0x0002 )
				{
					cmd = CMD_RTN_REAL;
					m_NetData.bit_rsp &= ~0x0002;
				} 
				//3.参数数据返回
				else if( (m_NetData.bit_rsp & 0x0004) == 0x0004 )
				{
					cmd = CMD_RTN_PARA;
					m_NetData.bit_rsp &= ~0x0004;
				}
				//4.参数上报
				else if( (m_NetData.bit_rsp & 0x0008) == 0x0008 )
				{
					cmd = CMD_UP_PARA;
					m_NetData.bit_rsp &= ~0x0008;
				}
				//5.参数下发错误回复
				else if( (m_NetData.bit_rsp & 0x0010) == 0x0010 )
				{
					cmd = CMD_OK_PARA;
					ok = 0;
					m_NetData.bit_rsp &= ~0x0010;
				}
				//6.校准数据下发回复
				else if( (m_NetData.bit_rsp & 0x0020) == 0x0020 )
				{
					cmd = CMD_ADJUST;
					m_NetData.bit_rsp &= ~0x0020;
					ok = 1;
				}
				//7.开关机成功回复
				else if( (m_NetData.bit_rsp & 0x0040) == 0x0040 )
				{
					cmd = CMD_DOWN_ONOFF;
					m_NetData.bit_rsp &= ~0x0040;
					ok = 1;
				}
				//8.开关机失败回复
				else if( (m_NetData.bit_rsp & 0x0080) == 0x0080 )
				{
					cmd = CMD_DOWN_ONOFF;
					m_NetData.bit_rsp &= ~0x0080;
					ok = 0;
				}
				//9.重启回复
				else if( (m_NetData.bit_rsp & 0x0100) == 0x0100 )
				{
					cmd = CMD_DOWN_RESET;
					m_NetData.bit_rsp &= ~0x0100;
					ok = 1;
				}
				//其他回复数据
				else if( (m_NetData.bit_rsp & 0x4000) == 0x4000 )
				{
					cmd = CMD_ALL_RTN;
					m_NetData.bit_rsp &= ~0x4000;
					ok = 1;
				}
				sendlen = Frame_make( m_net_Sendbuff, CMD_COM2NET, cmd, ok );
			}
			//2.实时数据发送时间
			else if( (m_NetData.real_time == 0) || (m_NetData.real_send == 1) )
			{
				sendlen = Frame_make( m_net_Sendbuff, CMD_COM2NET, CMD_UP_REAL, 0 );
			  	if( m_NetData.real_send == 1 )
				  	m_NetData.real_send = 0;
				else
					m_NetData.real_time = m_SetData.realtime;	
			}
            //3.参数数据发送时间
            else if( m_NetData.para_time == 0 )
            {
                m_NetData.bit_rsp |= 0x0008;
                m_NetData.para_time = TIME_PARA_PERIOD;
            }
			//4.心跳包发送时间
			else if( m_NetData.heart_time == 0 )
			{
			   sendlen = Frame_make( m_net_Sendbuff, CMD_COM2NET, CMD_UP_HEART, 0 );
			   m_NetData.heart_time = m_SetData.hearttime;	 
			   m_NetData.heart_c++;
			}
            //5.读取模块状态数据发送时间
            else if( m_NetData.status_time == 0 )
            {
                sendlen = Frame_make( m_net_Sendbuff, CMD_GET_STATUS, 0, 0 );
                m_NetData.status_time = TIME_STATUS_PERIOD2;
            }
            m_NetData.other_time  = sendlen+TIME_NET_PERIOD;
		}
		//有数据发送
		if( sendlen > 0 )
		{
			Com_Send(0, m_net_Sendbuff, sendlen, 0, 0 ); 
		}
 	}
	
    return 1;
}

/******************************************************************************
//  对上响应数据信息
*******************************************************************************/
int Com_Net_Rsp( unsigned char* buf, unsigned char id )
{
  	unsigned char cmd;
	unsigned short i,len;
    unsigned char* p;
	unsigned short* net_cmd;
    signed short mode = 0;
	
	/*if( (buf[0] != 0x46) || (buf[1] != 0x54))
	  return 0;*/
	if( (buf[0] != 0x54) || (buf[1] != 0x46))
	  return 0;
	net_cmd = (unsigned short*)&buf[2];
	if( *net_cmd == CMD_NET2COM )
	{
	  	//加密数据长度
		len = (buf[7]<<8)+buf[6]-11;
		cmd = buf[8];
		//设备序列号错误返回
		/*for( i = 0; i < 8; i++ )
		{
			if( buf[9+i] != m_SetData.serialnumber[i] )
			  return 0;
		}*/
		//解密
		WriteCode(buf[18], &buf[19], len);
		switch( cmd )
		{
		case CMD_UP_HEART: //心跳包回复
			//小时和分钟不一致重新设置时间
            if( (buf[19] != 70) && (buf[20] != 1) && (buf[21] != 1) )
            {
                if( (buf[22] != m_time_now.tm_hour) || (buf[23] != m_time_now.tm_min) )
                {
                    rtc_time_set( buf[19]+2000,buf[20],buf[21],buf[22],buf[23],buf[24] );
                }
            }
			m_NetData.heart_c = 0;
		  	break;
		case CMD_DOWN_PARA: //参数配置下发
            len = (buf[7]<<8) | buf[6];
            if( len == 53 )
            {
                //在参数寄存器区域
                for( i = 0; i < 7; i++ )
                {
                    m_SetData.onofftime[i][1] = (buf[20+4*i] << 8) | buf[19+4*i];
                    m_SetData.onofftime[i][0] = (buf[22+4*i] << 8) | buf[21+4*i];
                }
                for( i = 0; i < 7; i++ )
                {
                    m_SetData.setmode[i] =  buf[48+2*i] | buf[47+2*i];
                }
                //参数保存
                flash_e( ADDR_SETTING, sizeof( struct SetData ) );
                flash_w( (unsigned short*)&m_SetData, ADDR_SETTING, sizeof( struct SetData ));
                
                //如果当前处于定时模式，需要查询新的定时信息
                if( (m_RunData.runmode & MODE_TIME) == MODE_TIME )
                    mode_set( m_RunData.runmode );
                
                m_NetData.bit_rsp |= 0x0001;
                m_NetData.bit_rsp |= 0x0008; //参数上报更新
            }
			break;
		case CMD_READ_REAL:  //实时数据读取
			m_NetData.bit_rsp |= 0x0002;
			break;
		case CMD_READ_PARA:  //参数数据读取
			m_set_srart = buf[19] + (buf[20]<<8);
			m_set_bytes = buf[21] + (buf[22]<<8);  
			//正常读取
			if( (m_set_srart + m_set_bytes) < MAX_SET_END )
				m_NetData.bit_rsp |= 0x0004;  
			break;
		case CMD_DOWN_ONOFF:  //开关机
            if( buf[19] < 0x80 )
            {
                //关机模式
                if( buf[19] == 0 )
                {
                    //开机状态
                    if( m_RunData.runmode != 0 )
                    {
                        display_enable(0, 1);
                        mode = MODE_LEVEL0;
                    }
                    else //关机状态
                    {
                        if( m_DevData.oldrunmode != 0 )
                        {
                            mode = m_DevData.oldrunmode;
                            //定时智能模式
                            if( (m_DevData.oldrunmode & 0x0030) == 0x0030 )
                                mode = 0x0030;
                        }
                        else
                        {
                            mode = MODE_LEVEL1;
                        }
                    }
                }
                else
                {
                    mode = buf[19];
                    if( buf[19] >= MODE_TIME )
                    {
                        if( (buf[19] & MODE_TIME)== MODE_TIME )  //定时模式
                        {
                            mode = m_RunData.runmode & 0x002f;
                            mode |=  MODE_TIME;
                            m_DevData.timemode = 1;
                        }
                        else if( (buf[19] & MODE_INTEL)== MODE_INTEL )  //自动模式
                        {
                            mode = m_RunData.runmode & 0x000f;
                            mode |= MODE_INTEL;
                            m_DevData.net_automode = 1; 
                        }
                    }
                }
                if( mode > -1 )
                    mode_set( mode );
				m_NetData.bit_rsp |= 0x0040;
            }
            else
            {
                m_NetData.bit_rsp |= 0x0080;
            }
			break;
		default:
			break;
		}
	}
	else if( (*net_cmd == CMD_GET_STATUS) || (*net_cmd == CMD_UP_STATUS)  )
	{
		//加密数据长度
		len = (buf[7]<<8)+buf[6];
		if( len == (MAX_CURRENT_STATUS+11) )
		{
            WriteCode(buf[18], &buf[19], MAX_CURRENT_STATUS);
			p = (unsigned char*)&m_NetData.status;
			for( i = 0; i < MAX_CURRENT_STATUS; i++ )
			{
				*p = buf[19+i];
				p++;
			}
			m_RunData.signal = m_NetData.signal;
            if( m_NetData.status == 3 )
                m_NetData.connect = 1;
            else
                m_NetData.connect = 0;
		}
        if( *net_cmd == CMD_UP_STATUS )
            m_NetData.status_rt = 1;
        
		m_NetData.err_num = 0;
	}
	else if( *net_cmd == CMD_CHECK )
    {
		//加密数据长度
		len = (buf[7]<<8)+buf[6];
		if( len == 12 )
            m_DevData.check = buf[19];
    }
	
	return 1;   
}

/*-------------------------------------------------------------
//2.对上网络通信--超时函数
**------------------------------------------------------------*/
int Com_Net_Dly( unsigned char* buf,unsigned char id )
{
  	m_NetData.connect = 0;
	
    return 0;
}

/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ***************/

