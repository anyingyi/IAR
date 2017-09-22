/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ****************
* �ļ�����: lcd.h
* ��    ��: Һ����ʾ.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/7

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/7: �ļ�����        ������
*******************************************************************************/
#ifndef _LCD_H_
#define _LCD_H_

//��ַ����
#define MAX_REG             56
#define DIS_NC              0xff
#define BLINK_TIME          50  //50*10ms=500ms

#define MAX_NIXIE           26  //�������
#define MAX_SEGMENT         38  //��������

//���벻��ʾbitλ
#define NC_BIT0             0
#define NC_BIT1             1
#define NC_BIT2             2
#define NC_BIT3             3
#define NC_BIT4             4
#define NC_BIT5             5
#define NC_BIT6             6
#define NC_BIT7             7
#define NC_NONE             0xff

//���붨��
#define T1                  0
#define T2                  1
#define T3                  2
#define T4                  3
#define T5                  4
#define T6                  5
#define T7                  6
#define T8                  7
#define T9                  8
#define T10                 9
#define T11                 10
#define T12                 11
#define T13                 12
#define T14                 13
#define T15                 14
#define T16                 15
#define T17                 16
#define T18                 17
#define T19                 18
#define T20                 19
#define T21                 20
#define T22                 21
#define T23                 22
#define T24                 23
#define X1                  24
#define X2                  25
#define X3                  26
#define X4                  27
#define X5                  28
#define X6                  29
#define X7                  30
#define X8                  31
#define X9                  32
#define X10                 33
#define X11                 34
#define X12                 35
#define X13                 36
#define X14                 37

//��ʾ����
#define REGION_TIME         0   //ʱ����ʾ����
#define REGION_PM25         1   //PM2.5��ʾ����
#define REGION_TH           2   //��ʪ����ʾ����
#define REGION_MODE         3   //����ģʽ��ʾ����
#define REGION_PM01_TVOC    4   //PM1��TVOC��ʾ����
#define REGION_PM10_CO2     5   //PM10��CO2��ʾ����
#define REGION_FILTER       6   //��о������ʾ����
#define MAX_REGION          7   //��ʾ��������

//��ʾ״̬
#define STATUS_ON           0   //��ʾ״̬
#define STATUS_BLINK        1   //��˸״̬

#define BLINK_NIXIE         0   //������˸
#define BLINK_SEGMENT       1   //������˸
#define MAX_BLINK           2   //��˸����

//������ʾģʽ
typedef enum
{
    MODE_T1 =  0,   //��ʾģʽ1
    MODE_T2 =  1,   //��ʾģʽ2
    MODE_T3 =  2,   //��ʾģʽ3
    MODE_T4 =  3,   //��ʾģʽ4
    MODE_T5 =  4,   //��ʾģʽ5
    MODE_T6 =  5,   //��ʾģʽ6
    MODE_T7 =  6,   //��ʾģʽ7
}DIS_MODE;

//������ʾ��Ϣ�ṹ��
typedef struct CNIXIE_t
{
    unsigned char   addr0;      //��λλ��0
    unsigned char   addr1;      //��λλ��1
    DIS_MODE        dismode;    //������ʾģʽ
    unsigned char   nixienc;    //������ʾbitλ
}CNIXIE;

//������ʾ��Ϣ�ṹ��
typedef struct CSEGMENT_t
{
    unsigned char   addr;       //��ַ
    unsigned char   bit;        //bitλ 
}CSEGMENT;

//��Ļˢ�½ṹ�� 
typedef struct CDISPLAY_t
{
    unsigned int    nixie;      //������˸��־
    unsigned int    segment;    //������˸��־
    unsigned short  time;       //��˸��ʱ
    unsigned char   blink;      //��˸��־
    unsigned char   oldblink;   //��һ����˸״̬
    unsigned short  oldstatus;  //��һ�ι���״̬
    unsigned char   connect;    //��һ������״̬
    unsigned short  pm01;       //��һ��PM1����
    unsigned short  pm25;       //��һ��PM2.5����
    unsigned short  pm10;       //��һ��PM10����
    signed short    temp;       //��һ���¶�����
    unsigned short  humi;       //��һ��ʪ������
    unsigned short  co2;        //��һ��CO2����
    unsigned short  tvoc;       //��һ��tvoc����
    unsigned short  runmode;    //��һ������ģʽ����
    unsigned short  filter;     //��һ����о��������
    unsigned char   trans;      //��һ�����ߴ����־
    unsigned char   tm_min;     //��һ�η���
    unsigned char   status;     //��һ��wifi״̬  
}CDISPLAY;

typedef void (*displayfunc)( void );
typedef void (*blinkfunc)( unsigned char, unsigned char );
/* Exported macro ------------------------------------------------------------*/
void display_time( void );
void display_pm25( void );
void display_th( void );
void display_mode( void );
void display_pm01tvoc( void );
void display_pm10co2( void );
void display_filter( void );
void blink_nixie( unsigned char id, unsigned char status );
void blink_segment( unsigned char id, unsigned char status );

extern void lcd_init( void );                       //lcd��ʼ������
extern void lcd_diplay( void );                     //lcdȫ����ʾ����
extern void lcd_clrwdt( void );                     //lcd WDT
extern displayfunc  m_LcdDisplay[MAX_REGION];       //lcd������ʾ����
extern blinkfunc    m_LcdBlink[MAX_BLINK];          //lcd��˸��ʾ����
extern void display_check( unsigned char* str, unsigned char light );    //�Լ���ʾ

#endif //_LCD_H_
/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ **************/
