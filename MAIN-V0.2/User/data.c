/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: data.c
* ��    ��: xxxx���ݽṹ��������ݶ���.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/1

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/1 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "data.h"
#include "flash.h"
#include "rtc.h"

__IO unsigned short m_timer_1ms;
__IO unsigned short m_timer_10ms = 50;
__IO unsigned short m_timer_1000ms = 5;
__IO unsigned short m_buzzer_time = 0;

struct tm		m_time_now; //���ڵ�����ʱ��
struct tm       m_time_set; //���õ�����ʱ��
struct SetData 	m_SetData; 
struct RunData 	m_RunData;
struct NetData 	m_NetData; 
struct DevData 	m_DevData;

unsigned short m_test_pwm = 94;


//----local functions-----------------------------------------------------------
void Data_Init_Para(unsigned char allereset);
unsigned char Str_Cmp( unsigned char* Str, unsigned char* Src );


void Data_Init( unsigned char init )
{
    int date;
    unsigned short data;
    float rate;
	
	if( init == 0 )
	{
		flash_r( (unsigned char*)&m_SetData,ADDR_SETTING,sizeof( struct SetData ));
		data = m_SetData.productdate;
		date = ((data>>9)&0xff )*10000 + ((data>>5)&0x0F)*100 + (data&0x1F);
		if( date != PRODUCT_DATE )
		{
			Data_Init_Para(1);
			flash_e( ADDR_SETTING, sizeof( struct SetData ) );
			flash_w( (unsigned short*)&m_SetData,ADDR_SETTING,sizeof( struct SetData ));
		}
	}
	else if( init == 1 )
	{
		Data_Init_Para(0);
		flash_e( ADDR_SETTING, sizeof( struct SetData ) );
		flash_w( (unsigned short*)&m_SetData,ADDR_SETTING,sizeof( struct SetData ));
	}
    
    //����͹��Ĭ�Ͽ���
	m_DevData.lcdonoff = 0;
	m_DevData.lightonoff = 1;
	m_DevData.actived = ACTTIME;
    //m_DevData.blink = BLINK_TIME;
    
    //����ʹ��ʱ��
    if( m_RunData.totaltime == 0 )
        m_RunData.totaltime = m_SetData.usedfilter;
    else
    {
        //��1�������
        if( (m_RunData.totaltime - m_SetData.usedfilter) > 1440 )
            m_RunData.totaltime = m_SetData.usedfilter;
    }
    if( m_SetData.filtertime > m_RunData.totaltime )
    {
        rate = m_SetData.filtertime - m_RunData.totaltime;
        rate = rate*100/m_SetData.filtertime+0.5;
    }
    else
    {
        rate = 0;
    }
    m_RunData.filter = (unsigned short)rate;
}

//������ʼ��
void Data_Init_Para(unsigned char allereset)
{
	unsigned char i;
	unsigned char* id = (unsigned char*)0x1FFFF7EA;
	
	if( allereset )
	{
		for( i = 0; i < 8; i++ )
		{
			if(IS_CHAR(*id))
				m_SetData.serialnumber[i] = *id;
			else
				m_SetData.serialnumber[i] = *id%10+0x30;
			id++;
		}
		m_SetData.hardversion = 100; //V1.00
		m_SetData.softversion = 100; //V1.00
		m_SetData.productdate = (PRODUCT_DATE/10000<<9)+((PRODUCT_DATE/100%100)<<5)+(PRODUCT_DATE%100);
		m_SetData.pm25[0] = 35; //35ug/m3
		m_SetData.pm25[1] = 75; //75ug/m3
		m_SetData.pm25[2] = 115; //115ug/m3
		m_SetData.pm25[3] = 150; //150ug/m3
		m_SetData.pm25[4] = 250; //250ug/m3
		m_SetData.temp[0] = 1000; //10��
		m_SetData.temp[1] = 3500; //35��
		m_SetData.humi[0] = 3000; //30%RH
		m_SetData.humi[1] = 6000; //60%RH
		m_SetData.co2[0] = 1000; //1000PPM
		m_SetData.co2[1] = 2000; //2000PPM
		m_SetData.co2[2] = 3000; //3000PPM
		m_SetData.tvoc[0] = 400; //400ug/m3
		m_SetData.tvoc[1] = 500; //500ug/m3
		m_SetData.tvoc[2] = 600; //600ug/m3
		m_SetData.hearttime = 180; //180s=3min
		m_SetData.realtime = 900; //900s=15min
		m_SetData.filtertime = 172800;  //172800min=4months
		//Ĭ�ϣ�����ػ�9:00-16:00,���Ͽ���16:00,һ��ģʽ
		for( i = 0; i < 7; i++ )
		{
			m_SetData.onofftime[i][0] = 900;
			m_SetData.onofftime[i][1] = 1600;
			m_SetData.setmode[i] = 0x0001;
		}
        m_SetData.usedfilter = 0;
        m_SetData.advancetime = 0;
        //���������ݳ�ʼ��
        for( i = 0; i < 6; i++ )
            m_SetData.keycode[i] = 0xffffffff;
	}
}

/*------------------------------------------------------------------------------
    �ַ�������
------------------------------------------------------------------------------*/
unsigned short strslen(unsigned char* str)
{
    unsigned short len = 0;
    while( (str[len] != '\0') && (len < 0xffff) )len++;
    return len;
}

/*------------------------------------------------------------------------------
    �ַ����Ƚ�
------------------------------------------------------------------------------*/
unsigned char Str_Cmp( unsigned char* Str, unsigned char* Src )
{
    unsigned char rt = 1;
    unsigned short Idx = 0;
    unsigned short len1,len2;
    
    len1 = strslen(Str);
    len2 = strslen(Src);
    if( len1 == len2 )
    {
        while( Idx < len1 )
        {
            if( Str[Idx] == Src[Idx] )
                Idx++;
            else
            {
                Idx = len1;
                rt = 0;
            }
        }
    }
    else
    {
        rt = 0;
    }
    
    return rt; 
}

/*******************************************************************************
* ��������  : num2str
* ��    ��  : ��������
* ��    ��  : str���ַ����洢��ַ��numpro���������ԣ�val����ת����ֵ
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void num2str(unsigned char* str,unsigned char numpro,int val )
{
    unsigned int v;
    unsigned char DEC = ( ( numpro>>1 ) & 0x07 );//С������
    unsigned char INT = ( numpro >> 4 );//��������
    unsigned char ZS  = ( numpro & 0x01 );//��֧��
    unsigned char LEN = DEC + INT;
    if( DEC > 0 )LEN++;
    if( val < 0 ) v = 0 - val;
    else v = val;

    unsigned char i,j;
    //С������:123456--> 65
    unsigned char decstr[7];
    for( i = 0; i < DEC; i++ )
    {
        decstr[i] = v%10 + 0x30;
        v /= 10;
    }
    //��������:123456--> 432100
    unsigned char intstr[10];
    for( i = 0; i < INT; i++ )
    {
        intstr[i] = v%10 + 0x30;
        v /= 10;
    }
    //////////////////////////////////////////////////////
    //�������ֵߵ�����
    unsigned char over0 = 0;
    unsigned char ascii = 0;
    unsigned char ok = 0;
    for( i = 0; i < INT; i++ )
    {
        ascii = intstr[INT - i - 1];
        if( ascii == '0' )
        {
          if( over0 == 1 || ZS == 1 )
              str[i] = ascii;
          else
          {
              str[i] = ' ';
              ok |= 1;
          }
        }
        else
        {
            over0 |= 1;
            str[i] = ascii;
        }
    }
    if(  ascii == '0' )str[i-1] = ascii;
    //////////////////////////////////////////////////////
    //С�����ֵߵ�����
    if( DEC > 0 )str[i++] = '.';
    for( j = 1; j <= DEC; j++ )
    {
        str[i++] = decstr[DEC - j];
    }
    str[i] = '\0';
    //////////////////////////////////////////////////////
    //�ж�����__0012.23
    if( val < 0 )
    {
        if( ok == 1 )
        {
          for( i = LEN; i > 0; i-- )
          {
            if( str[i-1] == ' ' )
            {
                str[i-1] = '-';
                break;
            }
          }
        }
        else
        {
            if( str[0] == '0' )
            {
                str[0] = '-';
            }
            else
            {
                for( i = LEN; i > 0; i-- )
                {
                  str[i] = str[i-1];
                }
                str[i] = '-';
            }
        }
    }
}

/*------------------------------------------------------------------------------
 ��ʱ����
------------------------------------------------------------------------------*/
void DelayMicroSeconds(unsigned int nbrOfUs)
{   
  	unsigned int i,j;
  	for(i = 0; i < nbrOfUs; i++)
  	{  
		for( j = 0; j < 36; j++);
  	}
}

/*------------------------------------------------------------------------------
    �ַ�������
------------------------------------------------------------------------------*/
unsigned short Str_Cpy( unsigned char* Str,unsigned char* Src )
{
    unsigned short Idx = 0;
    while( Src[Idx] != '\0' )
    {
        Str[Idx] = Src[Idx];
        Idx++;
    }
    Str[Idx] = '\0';
	
    return Idx;
}

/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
