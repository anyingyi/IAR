/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: lcd.c
* ��    ��: Һ����ʾ.c�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/7

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/7 :�ļ�����       ������
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "data.h"
#include "ht1622.h"

//������ʾ��Ϣ��
const CNIXIE m_Nixie[MAX_NIXIE] = 
{
    { 1,  3,  MODE_T7, NC_BIT0 },  //nixie1
    { 5,  7,  MODE_T7, NC_BIT0 },  //nixie2
    { 9,  11, MODE_T7, NC_BIT0 },  //nixie3
    { 13, 15, MODE_T7, NC_BIT0 },  //nixie4
    { 17, 19, MODE_T7, NC_BIT0 },  //nixie5
    { 21, 23, MODE_T7, NC_BIT0 },  //nixie6
    { 25, 27, MODE_T7, NC_BIT0 },  //nixie7
    { 29, 31, MODE_T6, NC_NONE },  //nixie8
    { 33, 35, MODE_T6, NC_BIT3 },  //nixie9
    { 37, 39, MODE_T6, NC_NONE },  //nixie10
    { 16, 18, MODE_T2, NC_BIT7 },  //nixie11
    { 20, 22, MODE_T2, NC_BIT7 },  //nixie12
    { 24, 26, MODE_T2, NC_BIT7 },  //nixie13
    { 0,  2,  MODE_T1, NC_BIT4 },  //nixie14
    { 4,  6,  MODE_T1, NC_BIT4 },  //nixie15
    { 8,  10, MODE_T1, NC_BIT4 },  //nixie16
    { 12, 14, MODE_T1, NC_BIT4 },  //nixie17
    { 41, 43, MODE_T5, NC_BIT7 },  //nixie18
    { 45, 47, MODE_T5, NC_BIT7 },  //nixie19
    { 49, 51, MODE_T5, NC_NONE },  //nixie20
    { 40, 42, MODE_T3, NC_BIT4 },  //nixie21
    { 44, 46, MODE_T3, NC_BIT4 },  //nixie22
    { 48, 50, MODE_T3, NC_BIT4 },  //nixie23
    { 52, 54, MODE_T4, NC_NONE },  //nixie24
    { 36, 38, MODE_T3, NC_BIT4 },  //nixie25
    { 53, 55, MODE_T4, NC_BIT0 },  //nixie26
};

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

//������ʾ��Ϣ��
const CSEGMENT m_Segment[MAX_SEGMENT] = 
{
    { 1,   NC_BIT0  },  //T1
    { 5,   NC_BIT0  },  //T2
    { 9,   NC_BIT0  },  //T3
    { 13,  NC_BIT0  },  //T4
    { 17,  NC_BIT0  },  //T5
    { 21,  NC_BIT0  },  //T6
    { 25,  NC_BIT0  },  //T7
    { 22,  NC_BIT3  },  //T8
    { 33,  NC_BIT3  },  //T9
    { 18,  NC_BIT3  },  //T10
    { 26,  NC_BIT3  },  //T11
    { 6,   NC_BIT0  },  //T12
    { 2,   NC_BIT0  },  //T13
    { 10,  NC_BIT0  },  //T14
    { 14,  NC_BIT0  },  //T15
    { 34,  NC_BIT0  },  //T16
    { 43,  NC_BIT3  },  //T17
    { 47,  NC_BIT3  },  //T18
    { 42,  NC_BIT0  },  //T19  
    { 46,  NC_BIT0  },  //T20
    { 50,  NC_BIT0  },  //T21
    { 53,  NC_BIT0  },  //T22
    { 38,  NC_BIT0  },  //T23
    { 34,  NC_BIT3  },  //T24
    { 28,  NC_BIT3  },  //X1
    { 28,  NC_BIT2  },  //X2
    { 28,  NC_BIT1  },  //X3
    { 28,  NC_BIT0  },  //X4
    { 30,  NC_BIT0  },  //X5
    { 30,  NC_BIT1  },  //X6
    { 30,  NC_BIT2  },  //X7
    { 30,  NC_BIT3  },  //X8
    { 34,  NC_BIT1  },  //X9
    { 32,  NC_BIT3  },  //X10
    { 32,  NC_BIT2  },  //X11
    { 32,  NC_BIT1  },  //X12
    { 32,  NC_BIT0  },  //X13
    { 34,  NC_BIT2  },  //X14
};

//���ϴ���
const unsigned char m_SensorEr[3] = "Er";

//��ʼ�������
#define MAX_CMD_INIT    10   //��ʼ���������
unsigned short m_CmdInit[MAX_CMD_INIT] = 
{
    SYS_EN,         //turn on system oscillator
    LCD_ON,         //turn on LCD display
    CLR_TIMER,      //clear the contents of the time base generator
    CLR_WDT,        //clear the contents of WDT stage
    TIMER_DIS,      //disable time base output
    TONE_OFF,       //turn off tone outputs 
    RC_32K,         //system clock source, on-chip RC oscillator
    F16,            //time base clock output:16Hz,the WDT time-out flag after:1/4s
    IRQ_EN,         //enable IRQ output      
    WDT_EN,         //enable WDT time-out flag output
};

//ˢ��������Ϣ
CDISPLAY m_Display;

//������ʾ����
displayfunc m_LcdDisplay[ MAX_REGION ] = 
{ 
    display_time,       //ʱ��������ʾ����
    display_pm25,       //PM2.5������ʾ����
    display_th,         //��ʪ��������ʾ����
    display_mode,       //����ģʽ������ʾ����
    display_pm01tvoc,   //PM1��TVOC������ʾ����
    display_pm10co2,    //PM10��CO2������ʾ����
    display_filter,     //��������������ʾ����
};

//��˸����
blinkfunc m_LcdBlink[MAX_BLINK] = 
{
    blink_nixie,        //������˸����
    blink_segment,      //������˸����
};

/*******************************************************************************
* ��������  : lcd_init
* ��    ��  : lcd��ʼ��
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void lcd_init( void )
{
    unsigned char i, *p, buf = 0x0f;
    
    //1.����оƬIO�ڳ�ʼ��
    ht1622_init();
    //2.ȫ��ʾ
    for( i = 0; i < MAX_REG; i++ )
        ht1622_WriteMode( &buf, i, 1 );
    //3.ʱ��ð����˸
    m_LcdBlink[BLINK_SEGMENT]( T9, STATUS_BLINK );
    //4.�����ʼ�� �������ʼ�����������Ŀ2.0֮��
    ht1622_CmdMode( m_CmdInit, MAX_CMD_INIT );
    //5.��ʷ���ݳ�ʼ��
    p = (unsigned char*)&m_Display.connect;   
    for( i = 0; i < 23; i++ )
    {
        *p = 0xff;
        p++;
    }
}

/*******************************************************************************
* ��������  : nixie_getcode
* ��    ��  : ��ȡ��׼����
* ��    ��  : ascii:�ַ�
* ��    ��  : None
* ��    ��  : ��׼��������
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
* ��������  : nixie_encode
* ��    ��  : �������±���
* ��    ��  : number:��ֵ��ascii:�ַ�,DIS_MODE:�ر��뷽ʽ
* ��    ��  : None
* ��    ��  : �ر�����������
*******************************************************************************/
unsigned char nixie_encode( unsigned char number, unsigned char ascii, DIS_MODE dismode )
{
    unsigned char encode, code = 0;
    
    encode = 0;
    
    //1.��ȡ��׼����
    if( number < 10 )
        code = m_NixieCode[number];
    else
        code = nixie_getcode( ascii );
    //2.���±������
    switch( dismode )
    {
    case MODE_T1:
        encode = ((code&0x01)<<3) | ((code&0x02)<<6) | ((code&0x04)<<3) | 
                 ((code&0x08)>>3) | ((code&0x10)>>3) | ((code&0x20)>>3) | 
                 (code&0x40);
        break;
    case MODE_T2:
        encode = (code&0x01) | ((code&0x02)<<3) | ((code&0x04)<<4) | 
                 (code&0x08) | ((code&0x10)>>2) | ((code&0x20)>>4) | 
                 ((code&0x40)>>1);
        break;
    case MODE_T3:
        encode = (code&0x01) | ((code&0x02)<<4) | ((code&0x04)<<5) | 
                 (code&0x08) | ((code&0x10)>>2) | ((code&0x20)>>4) | 
                 (code&0x40);
        break;
    case MODE_T4:
        encode = ((code&0x01)<<4) | (code&0x02) | ((code&0x04)<<1) | 
                 ((code&0x08)<<4) | ((code&0x10)<<2) | (code&0x20) | 
                 ((code&0x40)>>4);
        break;
    case MODE_T5:
        encode = ((code&0x01)<<3) | ((code&0x02)<<5) | ((code&0x04)<<2) | 
                 ((code&0x08)>>3) | ((code&0x10)>>3) | ((code&0x20)>>3) | 
                 ((code&0x40)>>1);
        break;
    case MODE_T6:
        encode = ((code&0x01)<<7) | ((code&0x02)<<5) | ((code&0x04)<<3) | 
                 ((code&0x08)<<1) | ((code&0x10)>>4) | ((code&0x20)>>3) | 
                 ((code&0x40)>>5);
        break;
    case MODE_T7:
        encode = ((code&0x01)<<7) | ((code&0x02)<<5) | ((code&0x04)<<3) | 
                 ((code&0x08)<<1) | ((code&0x10)>>3) | ((code&0x20)>>2) | 
                 ((code&0x40)>>4);
        break; 
    default:
        break;
    }
    
   return encode;
}

/*******************************************************************************
* ��������  : nixie_display
* ��    ��  : ������ʾ
* ��    ��  : id:�������,data:��ʾ����, blink:�Ƿ���˸
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void nixie_display( unsigned char id, unsigned char data )
{
    unsigned char reg0, reg1;
    unsigned char code, old, blink, bit;
    
    //1.��ȡ����
    ht1622_ReadMode( &reg0, m_Nixie[id].addr0, 1 );
    ht1622_ReadMode( &reg1, m_Nixie[id].addr1, 1 );
    reg0 |= (reg1<<4);
    //2.����ʾbit��ȡ
    bit = 0;
    if( m_Nixie[id].nixienc != NC_NONE )
        bit = 0x01<<m_Nixie[id].nixienc;
    old = reg0&(~bit);
    code = reg0&bit;
    blink = ((m_Display.nixie >> id) & 0x00000001);
    //3.��˸״̬
    if( blink == STATUS_BLINK )
    {
        //3.1.����˸
        if( m_Display.oldblink !=  m_Display.blink )
        {
            if( !m_Display.blink ) 
            {
                reg0 = code&0x0f;
                reg1 = (code>>4)&0x0f;
                ht1622_WriteMode( &reg0, m_Nixie[id].addr0, 1);
                ht1622_WriteMode( &reg1, m_Nixie[id].addr1, 1);
            } 
        }
    }
    //4.��ʾ״̬
    if( (blink == STATUS_ON) || m_Display.blink )
    {
        //3.�µ�����
        if( old != data )
        {
            //3.1.�����ݲ���
            code |= data;
            //3.2.д������
            reg0 = code&0x0f;
            reg1 = (code>>4)&0x0f;
            ht1622_WriteMode( &reg0, m_Nixie[id].addr0, 1);
            ht1622_WriteMode( &reg1, m_Nixie[id].addr1, 1);
        }
    }
}
    
/*******************************************************************************
* ��������  : segment_display
* ��    ��  : ������ʾ
* ��    ��  : id:�������,data:��ʾ����, blink:�Ƿ���˸
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void segment_display( unsigned char id, unsigned char data )
{
    unsigned char reg0, bit, blink;
    
    data &= 0x01; 
    //1.��ȡ����
    ht1622_ReadMode( &reg0, m_Segment[id].addr, 1 );
    //2.��ȡ��Ӧbit
    bit = ( (reg0>>m_Segment[id].bit)&0x01 );
    blink = ((m_Display.segment >> id) & 0x00000001);
    //3.��˸״̬
    if( blink == STATUS_BLINK )
    {
        //3.1.����˸
        if( m_Display.oldblink !=  m_Display.blink )
        {
            if( m_Display.blink ) 
                reg0 |= (0x01<<m_Segment[id].bit);
            else
                reg0 &= ~(0x01<<m_Segment[id].bit);
            ht1622_WriteMode( &reg0, m_Segment[id].addr, 1);
        }
    }
    //4.��ʾ״̬
    else
    {
        //4.1�µ�����
        if( bit != data )
        {
            //4.1.1.����������
            reg0 &= ~(0x01<<m_Segment[id].bit);
            reg0 |= (data<<m_Segment[id].bit);
            //4.1.2.д��������
            ht1622_WriteMode( &reg0, m_Segment[id].addr, 1);
        }
    }
}

/*******************************************************************************
* ��������  : display_time
* ��    ��  : ��ʾʱ������
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void display_time( void )
{
    unsigned char i, t;
    unsigned int *time;
    unsigned char code;
    struct tm	*ptime;
    
    //1.�������ӱ�־
    if( m_Display.status != m_NetData.status )
    {
        //1.1.δ�B��·��
        if( m_NetData.status == 0 )
        {
            m_LcdBlink[BLINK_SEGMENT]( T2, STATUS_ON );
            segment_display( T2, 0 ); 
        }
        //1.2.����·�ɺ�������
        else if( (m_NetData.status == 1) || ( m_NetData.status == 3 ) )
        {
            m_LcdBlink[BLINK_SEGMENT]( T2, STATUS_ON );
            segment_display( T2, 1 );
        }
        //1.3.����ģʽ,��˸
        else
            m_LcdBlink[BLINK_SEGMENT]( T2, STATUS_BLINK );
          
        m_Display.status = m_NetData.status; 
    }
    if( m_Display.connect != m_NetData.connect )
    {
        segment_display( T4, m_NetData.connect ); 
        m_Display.connect = m_NetData.connect;
    }
    //2.���ߴ����־����ʾ
    if( m_Display.trans != m_DevData.trans )
    {
        segment_display( T4, m_DevData.trans ); 
        m_Display.trans = m_DevData.trans;
    }
    //3.�����б仯����������
    if( m_DevData.setmode > 0 )
        ptime = &m_time_set;
    else
        ptime = &m_time_now;
    if( (m_Display.tm_min != ptime->tm_min) || 
        (m_DevData.setmode && (m_Display.oldblink !=  m_Display.blink)) )
    {
        time = (unsigned int*)&ptime->tm_year;
        for( i = 0; i < 5; i++ )
        {
            t = 0;
            if( i == 1 ) t = 1;    
            code = nixie_encode( (*time+t)%100/10, DIS_NC, m_Nixie[2*i].dismode );
            nixie_display( 2*i, code );
            code = nixie_encode( (*time+t)%10, DIS_NC, m_Nixie[2*i+1].dismode );
            nixie_display( 2*i+1, code );
            time--;
        }
        m_Display.tm_min = ptime->tm_min; 
    }
    //4.wifiͼ���ð����˸
    if( m_Display.oldblink !=  m_Display.blink )
    {
        if( ((m_Display.segment >> T2)&0x00000001) == 1)
            segment_display( T2, m_Display.blink ); 
        if( ((m_Display.segment >> T9)&0x00000001) == 1)
            segment_display( T9, m_Display.blink ); 
    }
}

/*******************************************************************************
* ��������  : display_pm25
* ��    ��  : ��ʾPM2.5����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void display_pm25( void )
{
    unsigned short pm;
    unsigned char i, code, k = 0;
    
    //0.����������
    if( (m_Display.oldstatus & 0x0001) != (m_RunData.status & 0x0001) )
    {
        //0.1.ǧλ����ʾ
        segment_display( T10, 0 );
        //0.2.��λ����ʾ
        code = nixie_encode( DIS_NC, DIS_NC, m_Nixie[10].dismode );
        nixie_display( 10, code );
        //0.3.ʮ��λ��ʾEr
        for( i = 0; i < 2; i++ )
        {
            code = nixie_encode( DIS_NC, m_SensorEr[i], m_Nixie[11+i].dismode );
            nixie_display( 11+i, code );
        }
        m_Display.oldstatus &= ~0x0001;
        m_Display.oldstatus |= (m_RunData.status & 0x0001); 
    }
    else  //1.����������
    {
        pm = m_RunData.pm25;
        //1.1.������
        if( m_Display.pm25 != pm )
        {
           
            if( pm >= 1999 ) 
            {
                pm = 1999;
                k = 1;
            }
            //1.1.1.1000��������
            else if( pm >= 1000 )
              k = 1;
            segment_display( T10, k );
            //1.1.2.100��������
            if( (pm >= 100) || k )  
            {
                for( i = 0; i < 3; i++ )
                {
                    code = nixie_encode( pm%10, DIS_NC, m_Nixie[12-i].dismode );
                    nixie_display( 12-i, code );
                    pm /= 10;
                }
            }
            else //1.1.3.100�������ݣ���ʾ�ڰ�λ��ʮλλ��
            {
                k = 0;
                //1.1.3.1.������ȡд��
                do
                {
                    code = nixie_encode( pm%10, DIS_NC, m_Nixie[11-k].dismode );
                    nixie_display( 11-k, code );
                    pm /= 10;
                    k++;
                }while( pm );
                //1.1.3.2.��λ����ʱ������ʾ��λ����
                if( k < 2 ) 
                {
                    code = nixie_encode( DIS_NC, DIS_NC, m_Nixie[10].dismode );
                    nixie_display( 10, code );
                } 
                //1.1.3.3.����ʾ��λ����
                code = nixie_encode( DIS_NC, DIS_NC, m_Nixie[12].dismode );
                nixie_display( 12, code );
            }
            m_Display.pm25 = m_RunData.pm25;
        }
    }
}

/*******************************************************************************
* ��������  : display_th
* ��    ��  : ��ʪ����ʾ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void display_th( void )
{
    unsigned char i, k, code, str[4];
    signed short temp;
    
    //0.��ʪ�ȹ���
    if(  (m_Display.oldstatus & 0x0006) != (m_RunData.status & 0x0006) )
    {
        //0.1.���Ų���ʾ
        segment_display( T13, 0 );
        //0.2.�¶�ʪ����ʾEr
        for( i = 0; i < 2; i++ )
        {
            code = nixie_encode( DIS_NC,  m_SensorEr[i], m_Nixie[13+i].dismode );
            nixie_display( 13+i, code );
            code = nixie_encode( DIS_NC, m_SensorEr[i], m_Nixie[15+i].dismode );
            nixie_display( 15+i, code );
        }
        m_Display.oldstatus &= ~0x0006;
        m_Display.oldstatus |= (m_RunData.status & 0x0006); 
    }
    else //1.�¶���������
    {
        if( m_Display.temp != m_RunData.temp )
        {
            //1.1.����ת�ַ�
            if( m_RunData.temp < 0 )
                temp = m_RunData.temp - 50;
            else
                temp = m_RunData.temp;
            num2str( str, 0x20|ZS1, temp/100 );
            //1.2.��ʾ�¶�
            k = 0;
            if( str[0] == '-' ) k = 1;
            segment_display( T13, k );
            for( i = 0; i < 2; i++ )
            {
                code = nixie_encode( DIS_NC, str[i+k], m_Nixie[13+i].dismode );
                nixie_display( 13+i, code );
            }
            m_Display.temp = m_RunData.temp;
        }
        //2.ʪ����������
        if( m_Display.humi != m_RunData.humi )
        {
            //2.1.����ת�ַ�
            temp = m_RunData.humi;
            if( temp >= 10000 )
                temp = 9999;
            num2str( str, 0x20|ZS1, temp/100 );
            //2.2.��ʾʪ��
            for( i = 0; i < 2; i++ )
            {
                code = nixie_encode( DIS_NC, str[i], m_Nixie[15+i].dismode );
                nixie_display( 15+i, code );
            }
            m_Display.humi = m_RunData.humi;
        }
    }
}

/*******************************************************************************
* ��������  : display_mode
* ��    ��  : ����ģʽʾ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void display_mode( void )
{
    unsigned char i, mode;
    
    //1.�µ�����ģʽ
    if( m_Display.runmode != m_RunData.runmode )
    {
        mode = (m_RunData.runmode & 0x000f);
        //1.1.���з���
        mode *= 2;
        for( i = 0; i < 6; i++ )
        {
            if( i < mode )
                segment_display( X3+i, 1 );
            else
                segment_display( X3+i, 0 );
        }
        //1.2.����
        mode = ((m_RunData.runmode >> 6) & 0x00001);
        segment_display( X9, mode );
        //1.3.����ģʽ
        for( i = 0; i < 5; i++ )
            segment_display( X10+i, 0 );  
        mode = (m_RunData.runmode & 0x0030);
        if( mode )
        {
            //1.3.1.��ʱģʽ
            if( (m_RunData.runmode & 0x0010) == 0x0010 )
                segment_display( X14, 1 );
            //1.3.2.����ģʽ
            if( (m_RunData.runmode & 0x0020) == 0x0020 )
                segment_display( X13, 1 );  
        }
        else
        {
            //1.3.3.��λ
            mode = (m_RunData.runmode & 0x000f);
            if( mode > 0 )
                segment_display( X10+mode-1, 1 );  
        }
        m_Display.runmode = m_RunData.runmode;
    }
}

/*******************************************************************************
* ��������  : display_pm01tvoc
* ��    ��  : pm1��tvoc��ʾ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void display_pm01tvoc( void )
{
    unsigned short tp;
    unsigned char i, dis, code, str[4];
    
    dis = 0;
    //1.������ʾtvoc����
    if( m_RunData.tvoc > 0 )
    {
        //1.1.�µ�tvoc����
        if( m_Display.tvoc != m_RunData.tvoc ) 
        {
            //1.1.1.tvoc��Ϣ
            segment_display( T17, 0 ); 
            segment_display( T18, 1 ); 
            m_Display.tvoc = m_RunData.tvoc;
            tp = m_RunData.tvoc;
            dis = 1;
        }
    }
    //2.��ʾpm1����
    else
    {
        //2.1.�µ�pm1����
        if( m_Display.pm01 != m_RunData.pm01 ) 
        {
            //1.1.1.pm1��Ϣ
            segment_display( T17, 1 ); 
            segment_display( T18, 0 );
            m_Display.pm01 = m_RunData.pm01;
            tp = m_RunData.pm01;
            dis = 1;   
        }
    }
    //3.������������
    if( dis )
    {
        if( tp > 999 )
            tp = 999;
        //num2str( str, 0x30|ZS1, tp );
        num2str( str, 0x30|ZS0, tp );
        for( i = 0; i < 3; i++ )
        {
            code = nixie_encode( DIS_NC, str[i], m_Nixie[17+i].dismode );
            nixie_display( 17+i, code );
        }
    }
}

/*******************************************************************************
* ��������  : display_pm10co2
* ��    ��  : pm1��tvoc��ʾ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void display_pm10co2( void )
{
    unsigned short tp;
    unsigned char i, code, str[5];
    
    //1.������ʾCO2����
    if( m_RunData.co2 > 0 )
    {
        //1.1.�µ�co2����
        if( m_Display.co2 != m_RunData.co2 ) 
        {
            //1.1.1.tvoc��Ϣ
            segment_display( T20, 0 ); 
            segment_display( T21, 1 ); 
            m_Display.co2 = m_RunData.co2;
            if( m_RunData.co2 > 9999 )
                tp = 9999;
            else
                tp = m_RunData.co2;
            //num2str( str, 0x40|ZS1, tp );
            num2str( str, 0x40|ZS0, tp );
            for( i = 0; i < 4; i++ )
            {
                code = nixie_encode( DIS_NC, str[i], m_Nixie[20+i].dismode );
                nixie_display( 20+i, code );
            }
        }
    }
    //2.��ʾpm10����
    else
    {
        //2.1.�µ�pm10����
        if( m_Display.pm10 != m_RunData.pm10 ) 
        {
            //1.1.1.pm1��Ϣ
            segment_display( T20, 1 ); 
            segment_display( T21, 0 );
            m_Display.pm10 = m_RunData.pm10;
            if( m_RunData.pm10 > 999 )
                tp = 999;
            else
                tp = m_RunData.pm10;  
            //3.1.����ʾǧλ����
            code = nixie_encode( DIS_NC, DIS_NC, m_Nixie[20].dismode );
            nixie_display( 20, code );
            //3.2.��ʾ��λ����
            //num2str( str, 0x30|ZS1, tp );
            num2str( str, 0x30|ZS0, tp );
            for( i = 0; i < 3; i++ )
            {
                code = nixie_encode( DIS_NC, str[i], m_Nixie[21+i].dismode );
                nixie_display( 21+i, code );
            }
        }
    }
}

/*******************************************************************************
* ��������  : display_filter
* ��    ��  : ����������ʾ����
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void display_filter( void )
{
    unsigned char i, filter, code, str[3];
    
    //1.�µ�����
    if( m_Display.filter != m_RunData.filter ) 
    {
        filter = m_RunData.filter;
        if( filter <= 100 )
        {
            //1.1.100%
            segment_display( T24, filter/100 );
            num2str( str, 0x20|ZS1, filter%100 );  
        }
        else
        {
            str[0] = 'E';
            str[1] = 'r';
            str[2] = '\0';
        }
        //1.1.��ʾʮλ�͸�λ
        for( i = 0; i < 2; i++ )
        {
            code = nixie_encode( DIS_NC, str[i], m_Nixie[24+i].dismode );
            nixie_display( 24+i, code );
        }
        m_Display.filter = m_RunData.filter; 
    }
}

/*******************************************************************************
* ��������  : lcd_diplay
* ��    ��  : Һ����ʾ
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void lcd_diplay( void )
{
    unsigned char i;
    
    //0.LCD���⿪��
    if( m_DevData.lcdonoff == 1 )
    {
        //1.����ģ����ʾ
        for( i = 0; i < MAX_REGION; i++)
            m_LcdDisplay[i]();
        //2.��˸��ʱ
        if((m_Display.segment > 0) || (m_Display.nixie > 0))
        {
            if( m_Display.oldblink !=  m_Display.blink )
                m_Display.oldblink = m_Display.blink;
            if( m_Display.time <= BLINK_TIME )
                m_Display.blink = 1; 
            else
                m_Display.blink = 0; 
            if( m_Display.time >= 2*BLINK_TIME )
                m_Display.time = 0;
            m_Display.time++;
        }
        else
        {
            if( m_Display.time != 0 )
                m_Display.time = 0;
        }
    }
}


/*******************************************************************************
* ��������  : lcd_clrwdt
* ��    ��  : Һ��WDT
* ��    ��  : None
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void lcd_clrwdt( void )
{
    unsigned short cmd = CLR_WDT;
    
    //1.WDT����
    if( m_DevData.lcdwdt == 0 )
    {
        m_DevData.lcdwdt = 1;
        ht1622_CmdMode( &cmd, 1 );
    }
    //2.Һ�����ϣ����³�ʼ��
    else 
    {
        lcd_init();
    }
}

/*******************************************************************************
* ��������  : blink_nixie
* ��    ��  : ������˸
* ��    ��  : id��������ţ�status���Ƿ���˸
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void blink_nixie( unsigned char id, unsigned char status )
{ 
    unsigned int bit;
    
    bit = (0x0001<<id);
    m_Display.nixie &= ~bit;
    if( status ) 
        m_Display.nixie |= bit;
}

/*******************************************************************************
* ��������  : blink_segment
* ��    ��  : ������˸
* ��    ��  : id��������ţ�status���Ƿ���˸
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void blink_segment( unsigned char id, unsigned char status )
{     
    unsigned int bit;
    
    bit = (0x0001<<id);
    m_Display.segment &= ~bit;
    if( status ) 
        m_Display.segment |= bit;
}

/*******************************************************************************
* ��������  : display_check
* ��    ��  : �Լ���ʾ
* ��    ��  : str���ַ���
* ��    ��  : None
* ��    ��  : NOne
*******************************************************************************/
void display_check( unsigned char* str, unsigned char light )
{
    unsigned char i, code;
    
    //1.ǧλ
     segment_display( T10, light );
    //2.��ʾ����
    for( i = 0; i < 3; i++ )
    {
        code = nixie_encode( DIS_NC, str[i], m_Nixie[10+i].dismode );
        nixie_display( 10+i, code );
    }
}

/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
