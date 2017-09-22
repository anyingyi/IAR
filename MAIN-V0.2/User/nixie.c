/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: nixie.c
* ��    ��: ������ʾ.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/8/8

* �޸���ʷ: ʱ��       ����           �޸���
            2017/8/8 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "nixie.h"
#include "data.h"

unsigned short m_nixie_blink;
unsigned short m_nixie_onoff;
unsigned short m_nixie_blinktime;

//LED����
typedef struct CNIXIE_t
{
    unsigned char   onoff;          //��ʾ����:0x00-��,0x01-��
    unsigned char   preonoff;       //��һ�ο���״̬
    unsigned short  inittime;       //��ʼ��ʱ��
    unsigned char   blink;          //��˸״̬
    unsigned char   yblink;         //��˸��־
    unsigned short  blinktime;      //��˸ʱ��
    unsigned char   brightness;     //������ʾ����:0-7:��ʾ1��-8��
    unsigned char   prebrightness;  //��һ������
    unsigned char   disbuf[3];      //������ʾ����
    unsigned char   olddisbuf[3];   //������ʾ���汸��
}CNIXIE;

CNIXIE m_nixie;     //������ʾ

const unsigned char m_Display_addr[4] = { ADDR_DIG1, ADDR_DIG2, ADDR_DIG3, ADDR_DIG4};

const unsigned char m_Brightness[8] = {  CMD_BRIGHT_1,       //1������
                                         CMD_BRIGHT_2,       //2������ 
                                         CMD_BRIGHT_3,       //3������ 
                                         CMD_BRIGHT_4,       //4������
                                         CMD_BRIGHT_5,       //5������
                                         CMD_BRIGHT_6,       //6������ 
                                         CMD_BRIGHT_7,       //7������ 
                                         CMD_BRIGHT_8 };     //8������


//��׼��������
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
* ��������  : display_cmp
* ��    ��  : ��ʾ����Ƚ�
* ��    ��  : str1:����1��str2:����2��len:�Ƚϳ���
* ��    ��  : None
* ��    ��  : 0:��ʾ��һ����1:��ʾһ��
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
* ��������  : display_cpy
* ��    ��  : ��ʾ���濽��
* ��    ��  : str1:�����Ŀ�껺�棬str2:������ԭ���棬len:��������
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void display_cpy( unsigned char* str1, unsigned char* str2, unsigned char len )
{
    unsigned char i;
    
    for( i = 0; i < len; i++ )
        str1[i] = str2[i];
}

/*******************************************************************************
* ��������  : nixie_init
* ��    ��  : ������ʾ��ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void nixie_init( void )
{
    unsigned char i;
    
    //1.������ʼ��
    tm1650_Init( ID_NIXIE );
    //2.�Դ���������
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
* ��������  : nixie_getcode
* ��    ��  : ��ȡ�ַ���Ӧ����ʾ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
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
* ��������  : nixie_display
* ��    ��  : 
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void niexie_write( unsigned char id , unsigned char* buf, unsigned char onoff )
{
    unsigned char i, config;
    
    config = m_Brightness[m_nixie.brightness]|CMD_8_REG|onoff;
    //1.1.8�����ȡ�8����ʾ�Ϳ���ʾ
    tm1650_WriteConfig( id, config );
    //1.2.д���Դ�����
    if( onoff == CMD_ON_DIS )
    {
        for( i = 0; i < 3; i++ )
            tm1650_WriteDisplay( id, m_Display_addr[3-i], buf[i] );
    }
}

/*******************************************************************************
* ��������  : nixie_set
* ��    ��  : ���ַ�ת��Ϊ�ַ���ʾ
* ��    ��  : pbuf:�ַ�����blink:��˸��־λ
* ��    ��  : None
* ��    ��  : None
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
        //�ж��ַ���Ч
        nixie[n] |= nixie_getcode( ascii );
        //С����
        if( ascii == '.' )
        {
            nixie[n] |= 0x80;
            chg = 1;
        }
        else
        {
            if( pbuf[i+1] != '.' )//��һ����С����Ļ�����ʾ
                chg = 1;
        }
        //��Ҫ��ʾ
        if( chg == 1 )
        {
            //�ַ���ʾ
            if( nixie[n] != m_nixie.disbuf[n] )
            {
                //���ݸ���
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
* ��������  : nixie_display
* ��    ��  : 
* ��    ��  : None
* ��    ��  : None
* ��    ��  : None
*******************************************************************************/
void nixie_display( void )
{
    unsigned char chg;
    
    //0.��ʾ״̬
    if( m_nixie.onoff == 1 )
    {
        //0.1.����˸״̬
        if( m_nixie.blink == 0 )
        {
            //0.1.1.�Դ������и��¡�״̬�б仯
            chg = display_cmp( m_nixie.disbuf, m_nixie.olddisbuf, 3 );
            if( ( !chg ) || m_nixie.yblink || ( !m_nixie.preonoff ))
            {
                //0.1.1.1.д�Դ�����
                niexie_write( ID_NIXIE, m_nixie.disbuf, CMD_ON_DIS );
                //0.1.1.2.�Դ濽��
                display_cpy( m_nixie.olddisbuf, m_nixie.disbuf, 3 );
                //0.1.1.3.��־����
                m_nixie.yblink = 0;   
            }
        }
        else//0.2.��˸״̬
        {
            //0.2.1.��ʾ
            if( m_nixie.blinktime <= 100 )
            {
                m_nixie.blinktime++;
                if( m_nixie.blinktime == 100 )
                {
                    niexie_write( ID_NIXIE, "", CMD_ON_DIS );
                    m_nixie.blinktime = 0;
                }
            }
            //0.2.2.����ʾ
            if( m_nixie.blinktime == 50 )
                niexie_write( ID_NIXIE, m_nixie.disbuf, CMD_ON_DIS ); 
            //0.2.3.��˸��־
            m_nixie.yblink = 1;
        }
        //0.3.�����б仯
        if( m_nixie.prebrightness != m_nixie.brightness )
        {
            niexie_write( ID_NIXIE, m_nixie.disbuf, m_nixie.preonoff );
            m_nixie.prebrightness = m_nixie.brightness;
        }
        //0.4.����״̬����
        m_nixie.preonoff = m_nixie.onoff;     
    }
    else //1.����ʾ״̬
    {
        if( m_nixie.preonoff == 1 )
        {
            niexie_write( ID_NIXIE, "", CMD_ON_DIS );
            //niexie_write( ID_NIXIE, m_nixie.disbuf, CMD_OFF_DIS );
            m_nixie.preonoff = 0;
        }
    }
    //3.����ܳ�ʼ��ʱ��
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

/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
