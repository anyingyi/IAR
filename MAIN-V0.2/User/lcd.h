/************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 ****************
* 文件名称: lcd.h
* 描    述: 液晶显示.h文件
* 作    者: 王晶东
* 版    本: V1.0
* 日    期: 2017/6/7

* 修改历史: 时间       内容           修改者
            2017/6/7: 文件创建        王晶东
*******************************************************************************/
#ifndef _LCD_H_
#define _LCD_H_

//地址总数
#define MAX_REG             56
#define DIS_NC              0xff
#define BLINK_TIME          50  //50*10ms=500ms

#define MAX_NIXIE           26  //数码个数
#define MAX_SEGMENT         38  //段码条数

//数码不显示bit位
#define NC_BIT0             0
#define NC_BIT1             1
#define NC_BIT2             2
#define NC_BIT3             3
#define NC_BIT4             4
#define NC_BIT5             5
#define NC_BIT6             6
#define NC_BIT7             7
#define NC_NONE             0xff

//段码定义
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

//显示区域
#define REGION_TIME         0   //时间显示区域
#define REGION_PM25         1   //PM2.5显示区域
#define REGION_TH           2   //温湿度显示区域
#define REGION_MODE         3   //运行模式显示区域
#define REGION_PM01_TVOC    4   //PM1和TVOC显示区域
#define REGION_PM10_CO2     5   //PM10和CO2显示区域
#define REGION_FILTER       6   //滤芯寿命显示区域
#define MAX_REGION          7   //显示区块数量

//显示状态
#define STATUS_ON           0   //显示状态
#define STATUS_BLINK        1   //闪烁状态

#define BLINK_NIXIE         0   //数码闪烁
#define BLINK_SEGMENT       1   //段码闪烁
#define MAX_BLINK           2   //闪烁类数

//数码显示模式
typedef enum
{
    MODE_T1 =  0,   //显示模式1
    MODE_T2 =  1,   //显示模式2
    MODE_T3 =  2,   //显示模式3
    MODE_T4 =  3,   //显示模式4
    MODE_T5 =  4,   //显示模式5
    MODE_T6 =  5,   //显示模式6
    MODE_T7 =  6,   //显示模式7
}DIS_MODE;

//数码显示信息结构体
typedef struct CNIXIE_t
{
    unsigned char   addr0;      //段位位置0
    unsigned char   addr1;      //段位位置1
    DIS_MODE        dismode;    //数码显示模式
    unsigned char   nixienc;    //不能显示bit位
}CNIXIE;

//段码显示信息结构体
typedef struct CSEGMENT_t
{
    unsigned char   addr;       //地址
    unsigned char   bit;        //bit位 
}CSEGMENT;

//屏幕刷新结构体 
typedef struct CDISPLAY_t
{
    unsigned int    nixie;      //数码闪烁标志
    unsigned int    segment;    //段码闪烁标志
    unsigned short  time;       //闪烁计时
    unsigned char   blink;      //闪烁标志
    unsigned char   oldblink;   //上一次闪烁状态
    unsigned short  oldstatus;  //上一次故障状态
    unsigned char   connect;    //上一次网络状态
    unsigned short  pm01;       //上一次PM1数据
    unsigned short  pm25;       //上一次PM2.5数据
    unsigned short  pm10;       //上一次PM10数据
    signed short    temp;       //上一次温度数据
    unsigned short  humi;       //上一次湿度数据
    unsigned short  co2;        //上一次CO2数据
    unsigned short  tvoc;       //上一次tvoc数据
    unsigned short  runmode;    //上一次运行模式数据
    unsigned short  filter;     //上一次滤芯寿命数据
    unsigned char   trans;      //上一次有线传输标志
    unsigned char   tm_min;     //上一次分钟
    unsigned char   status;     //上一次wifi状态  
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

extern void lcd_init( void );                       //lcd初始化函数
extern void lcd_diplay( void );                     //lcd全屏显示函数
extern void lcd_clrwdt( void );                     //lcd WDT
extern displayfunc  m_LcdDisplay[MAX_REGION];       //lcd区域显示函数
extern blinkfunc    m_LcdBlink[MAX_BLINK];          //lcd闪烁显示函数
extern void display_check( unsigned char* str, unsigned char light );    //自检显示

#endif //_LCD_H_
/*************** (C) COPYRIGHT 2017 杭州卡丽智能科技股份有限公司 **************/
