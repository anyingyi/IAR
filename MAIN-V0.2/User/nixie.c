/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: nixie.c
* 描    述: 数码显示.c文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/8/8

* 修改历史: 时间       内容           修改者
            2017/8/8 :文件创建       王晶东
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "nixie.h"
#include "data.h"

unsigned short m_nixie_blink;
unsigned short m_nixie_onoff;
unsigned short m_nixie_blinktime;

//LED控制
typedef struct CNIXIE_t
{
    unsigned char   onoff;          //显示开关:0x00-关,0x01-开
    unsigned char   preonoff;       //上一次开关状态
    unsigned short  inittime;       //初始化时间
    unsigned char   blink;          //闪烁状态
    unsigned char   yblink;         //闪烁标志
    unsigned short  blinktime;      //闪烁时间
    unsigned char   brightness;     //数码显示亮度:0-7:表示1级-8级
    unsigned char   prebrightness;  //上一次亮度
    unsigned char   disbuf[3];      //数码显示缓存
    unsigned char   olddisbuf[3];   //数码显示缓存备份
}CNIXIE;

CNIXIE m_nixie;     //数码显示

const unsigned char m_Display_addr[4] = { ADDR_DIG1, ADDR_DIG2, ADDR_DIG3, ADDR_DIG4};

const unsigned char m_Brightness[8] = {  CMD_BRIGHT_1,       //1级亮度
                                         CMD_BRIGHT_2,       //2级亮度 
                                         CMD_BRIGHT_3,       //3级亮度 
                                         CMD_BRIGHT_4,       //4级亮度
                                         CMD_BRIGHT_5,       //5级亮度
                                         CMD_BRIGHT_6,       //6级亮度 
                                         CMD_BRIGHT_7,       //7级亮度 
                                         CMD_BRIGHT_8 };     //8级亮度


//标准数码段码表
/*     A
     _ _ _
    |     |
  F |  G  | B
    |_ _ _|
    |     |
  E |     | C
    |_ _ _| 
       D      */
const unsigned char m_NixieCode[38] = 
{
    0x3F,  //'0':00111111
    0x06,  //'1':00000110
    0x5B,  //'2':01011011
    0x4F,  //'3':01001111
    0x66,  //'4':01100110
    0x6D,  //'5':01101101
    0x7D,  //'6':01111101
    0x07,  //'7':00000111
    0x7F,  //'8':01111111
    0x6F,  //'9':01101111
    0x77,  //'A':01110111
    0x7C,  //'b':01111100
    0x39,  //'C':00111001
    0x5E,  //'d':01011110
    0x79,  //'E':01111001
    0x71,  //'F':01110001
    0x3D,  //'G':00111101
    0x76,  //'H':01110110
    0x19,  //'I':00011001
    0x0D,  //'J':00001101
    0x72,  //'K':01110010
    0x38,  //'L':00111000
    0x55,  //'M':01010101
    0x54,  //'n':01010100
    0x5D,  //'O':01011101
    0x73,  //'P':01110011
    0x67,  //'q':01100111
    0x50,  //'r':01010000
    0x6D,  //'S':01101101 
    0x78,  //'t':01111000
    0x3E,  //'U':00111110
    0x1C,  //'v':00011100
    0x6A,  //'W':01101010
    0x1D,  //'X':00011101
    0x6E,  //'y':01101110
    0x49,  //'Z':01001001
    0x40,  //'-':01000000
    0x00,  //' ':00000000
};


/*******************************************************************************
* 函数名称  : display_cmp
* 描    述  : 显示缓存比较
* 输    入  : str1:缓存1，str2:缓存2，len:比较长度
* 输    出  : None
* 返    回  : 0:表示不一样，1:表示一样
*******************************************************************************/
unsigned char display_cmp( unsigned char* str1, unsigned char* str2, unsigned char len )
{
    unsigned char i, rtn = 1;
    
    for( i = 0; i < len; i++ )
    {
        if( str1[i] != str2[i] )
        {
            rtn = 0;
            break;
        }
    }
    return rtn;
}

/*******************************************************************************
* 函数名称  : display_cpy
* 描    述  : 显示缓存拷贝
* 输    入  : str1:拷入的目标缓存，str2:拷出的原缓存，len:拷贝长度
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void display_cpy( unsigned char* str1, unsigned char* str2, unsigned char len )
{
    unsigned char i;
    
    for( i = 0; i < len; i++ )
        str1[i] = str2[i];
}

/*******************************************************************************
* 函数名称  : nixie_init
* 描    述  : 数码显示初始化
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void nixie_init( void )
{
    unsigned char i;
    
    //1.驱动初始化
    tm1650_Init( ID_NIXIE );
    //2.显存数据清零
    for( i = 0; i < 3; i++ )
    {
        m_nixie.disbuf[i] = 0xff;
        m_nixie.olddisbuf[i] = 0;
    }
    m_nixie.blinktime = 0;
    m_nixie.blink = 0;
    m_nixie.onoff = 1;
    m_nixie.preonoff = 1;
    m_nixie.brightness = 7;
    m_nixie.prebrightness = 7;
    m_nixie.inittime = 200;
}

/*******************************************************************************
* 函数名称  : nixie_getcode
* 描    述  : 获取字符对应的显示段码
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
unsigned char nixie_getcode(unsigned char ascii )
{
    unsigned char code = 0x00;
    if( ascii >= '0' && ascii <= '9' )
        code = m_NixieCode[ascii-'0'];
    else if( ascii >= 'A' && ascii <= 'Z' )
        code = m_NixieCode[ascii-55];
    else if( ascii >= 'a' && ascii <= 'z' )
        code = m_NixieCode[ascii-87];
    else if( ascii == ' ' )
        code = m_NixieCode[37];
    else if( ascii == '-' )
        code = m_NixieCode[36];
    return code;
}

/*******************************************************************************
* 函数名称  : nixie_display
* 描    述  : 
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void niexie_write( unsigned char id , unsigned char* buf, unsigned char onoff )
{
    unsigned char i, config;
    
    config = m_Brightness[m_nixie.brightness]|CMD_8_REG|onoff;
    //1.1.8级亮度、8段显示和开显示
    tm1650_WriteConfig( id, config );
    //1.2.写入显存数据
    if( onoff == CMD_ON_DIS )
    {
        for( i = 0; i < 3; i++ )
            tm1650_WriteDisplay( id, m_Display_addr[3-i], buf[i] );
    }
}

/*******************************************************************************
* 函数名称  : nixie_set
* 描    述  : 将字符转换为字符显示
* 输    入  : pbuf:字符串，blink:闪烁标志位
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void nixie_set( unsigned char* pbuf, unsigned char onoff, unsigned short bright, unsigned short blink )
{
    unsigned char ascii;
    unsigned char i,n,chg;
    unsigned char nixie[3] = {0,};
    n = 0;i = 0;
    ascii = pbuf[i];
    while( ascii != 0 && i < 6 )
    {
        chg = 0;
        //判断字符有效
        nixie[n] |= nixie_getcode( ascii );
        //小数点
        if( ascii == '.' )
        {
            nixie[n] |= 0x80;
            chg = 1;
        }
        else
        {
            if( pbuf[i+1] != '.' )//下一个是小数点的话不显示
                chg = 1;
        }
        //需要显示
        if( chg == 1 )
        {
            //字符显示
            if( nixie[n] != m_nixie.disbuf[n] )
            {
                //数据复制
                m_nixie.disbuf[n] = nixie[n];
            }
            n++;
            if( n >= 3 )
                break;
        }
        i++;
        ascii = pbuf[i];
    }
    m_nixie.blink = blink;
    m_nixie.onoff = onoff;
    m_nixie.brightness = bright;
    //niexie_write( ID_NIXIE, m_nixie.disbuf, m_nixie.onoff );
}

/*******************************************************************************
* 函数名称  : nixie_display
* 描    述  : 
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void nixie_display( void )
{
    unsigned char chg;
    
    //0.显示状态
    if( m_nixie.onoff == 1 )
    {
        //0.1.不闪烁状态
        if( m_nixie.blink == 0 )
        {
            //0.1.1.显存数据有更新、状态有变化
            chg = display_cmp( m_nixie.disbuf, m_nixie.olddisbuf, 3 );
            if( ( !chg ) || m_nixie.yblink || ( !m_nixie.preonoff ))
            {
                //0.1.1.1.写显存数据
                niexie_write( ID_NIXIE, m_nixie.disbuf, CMD_ON_DIS );
                //0.1.1.2.显存拷贝
                display_cpy( m_nixie.olddisbuf, m_nixie.disbuf, 3 );
                //0.1.1.3.标志清零
                m_nixie.yblink = 0;   
            }
        }
        else//0.2.闪烁状态
        {
            //0.2.1.显示
            if( m_nixie.blinktime <= 100 )
            {
                m_nixie.blinktime++;
                if( m_nixie.blinktime == 100 )
                {
                    niexie_write( ID_NIXIE, "", CMD_ON_DIS );
                    m_nixie.blinktime = 0;
                }
            }
            //0.2.2.不显示
            if( m_nixie.blinktime == 50 )
                niexie_write( ID_NIXIE, m_nixie.disbuf, CMD_ON_DIS ); 
            //0.2.3.闪烁标志
            m_nixie.yblink = 1;
        }
        //0.3.亮度有变化
        if( m_nixie.prebrightness != m_nixie.brightness )
        {
            niexie_write( ID_NIXIE, m_nixie.disbuf, m_nixie.preonoff );
            m_nixie.prebrightness = m_nixie.brightness;
        }
        //0.4.开关状态备份
        m_nixie.preonoff = m_nixie.onoff;     
    }
    else //1.不显示状态
    {
        if( m_nixie.preonoff == 1 )
        {
            niexie_write( ID_NIXIE, "", CMD_ON_DIS );
            //niexie_write( ID_NIXIE, m_nixie.disbuf, CMD_OFF_DIS );
            m_nixie.preonoff = 0;
        }
    }
    //3.数码管初始化时间
    if( m_nixie.inittime > 1 ) 
    {
        m_nixie.inittime--;
        if( m_nixie.inittime == 1 )
        {
            m_nixie.inittime = 0;
            m_nixie.onoff = 0;
            niexie_write( ID_NIXIE, "", CMD_ON_DIS );
        }
    }
}

/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
