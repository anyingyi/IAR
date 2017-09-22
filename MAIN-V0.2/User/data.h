/*************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************
* �ļ�����: data.h
* ��    ��: ���ݽṹ��������ݶ���.h�ļ�
* ��    ��: ������
* ��    ��: V1.0
* ��    ��: 2017/6/2

* �޸���ʷ: ʱ��       ����           �޸���
            2017/6/2 :�ļ�����       ������
*******************************************************************************/
#ifndef _DATA_H_
#define _DATA_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "time.h"

/*
STM32F103RBT6 FLASH SIZE: 128K,MEM SIZE 20K
*/
//�������ݱ����ַ
#define ADDR_SETTING	 0x0801F800

/*------------------------------------------------------------*/
#define IS_CHAR(x)	(((x >= '0')&&(x <= '9')) || \
					 ((x >= 'A')&&(x <= 'Z')) || \
  					 ((x >= 'a')&&(x <= 'z')))

#define ZS0				0x00 	//��֧��0
#define ZS1   			0x01	//֧��0
#define DEC1  			0x02	//1λС��
#define DEC2  			0x04	//2λС��
#define DEC3  			0x06	//3λС��


#define ACTTIME			180		//����ʱ��180��
#define ESCTIME			60		//�޲����Զ��˳�ʱ��60��
#define BLINK_TIME      60      //����ʱ��60s
#define FILTER_TIME     3       //��оʣ��ٷֱ���ʾʱ��3s

#define PRODUCT_DATE	170720

struct SetData
{
    unsigned char  	serialnumber[8]; 	//���к�
    unsigned short 	hardversion; 		//Ӳ���汾��ͨ���ð汾�������Ƿ��WIFIģ��
    unsigned short 	softversion; 		//����汾
    unsigned short 	productdate; 		//��������
	unsigned short 	pm25[5]; 			//PM2.5�ȼ����֣�0λС��
	  signed short 	temp[2]; 			//�¶������ޣ�2λС��
	unsigned short  humi[2]; 			//ʪ�������ޣ�2λС��
	unsigned short  co2[3]; 			//CO2�ȼ����֣�0λС��
	unsigned short  tvoc[3]; 			//TVOC�ȼ����֣�0λС��
	unsigned short  hearttime; 			//����ʱ��������λ��
	unsigned short  realtime; 			//�ϱ�ʱ��������λ��
	unsigned int	filtertime;			//��������ʱ��
	unsigned short	onofftime[7][2];	//��ʱ���ػ�ʱ��
	unsigned short	setmode[7]; 		//��ʱ��������ģʽ
    unsigned int    usedfilter;         //������ʹ��ʱ��
    unsigned short  advancetime;        //��ʱ���ػ�ʱ�䣬��λ��h
    unsigned int    keycode[6];         //������
	unsigned short	reserved[8]; 		//����
};

struct RunData
{
    unsigned short  status; 			//״̬��Ϣ,�ӵ�λ����λ�ֱ�Ϊ��PM2.5���������¶ȴ�������ʪ�ȴ�������CO2��������TVOC������ 
    unsigned short  pm01;				//PM1��ֵ��0λС��
    unsigned short  pm25;				//PM2.5��ֵ��0λС��
    unsigned short  pm10;				//PM10��ֵ��0λС��	
      signed short  temp;				//�¶���ֵ��2λС��
	unsigned short	humi;				//ʪ����ֵ��2λС��
	unsigned short  co2;				//CO2��ֵ��0λС��	
	unsigned short  tvoc;				//tvoc��ֵ��0λС��	
	unsigned short  signal;				//wifiǿ�ȣ�0λС��	
	unsigned short  runmode;			//��ǰ����ģʽ
	unsigned short  filter;				//��оʣ��ٷֱ�
	unsigned int	totaltime;			//���������ʱ��
	unsigned int	level1time;			//һ��������ʱ��
	unsigned int	level2time;			//����������ʱ��
	unsigned int	level3time;			//����������ʱ��
    unsigned char   warnsensor;         //������״̬����λ����λ PM2.5  temp&humi  CO2  TVOC   
    unsigned char   wpm25;              //PM2.5�澯��Ϣ 
    unsigned char   wtemp;              //�¶ȸ澯��Ϣ 
    unsigned char   whumi;              //ʪ�ȸ澯��Ϣ
    unsigned char   wco2;               //co2�澯��Ϣ 
    unsigned char   wtvoc;              //TVOC�澯��Ϣ 
    unsigned char   wfilter;            //�����澯��Ϣ 
    unsigned char   reserved1[3];       //����
	unsigned short  reserved[6];		//����
};


//��·��������
struct NetData
{
  	unsigned char 	status;			//wifiģ��״̬ 0-δ���ӡ�1-����·�ɡ�2-����״̬��3-����������
    unsigned char   signal;         //wifi�ź�ǿ��
    unsigned char   ip[16];         //ip��Ϣ
    unsigned char   mask[16];       //mask��Ϣ    
    unsigned char   gate[16];       //gate��Ϣ   
    unsigned char   dns[16];        //dns��Ϣ
    unsigned char   mac[18];        //mac��Ϣ
    
  	unsigned char	connect;		//�������ӱ�־
	unsigned short	err_num;		//����������ʧ�ܴ���
	unsigned short	heart_time;		//������ʱ
	unsigned short	real_time;		//ʵʱ��ʱ
	unsigned short	para_time;		//������ʱ
    unsigned short  status_time;    //״̬��ʱ
	unsigned short	other_time;		//������ʱ
	unsigned short	bit_rsp;		//�����ظ����� ���ȼ��ӵ�λ����λ��������ȷ�·��ظ���ʵʱ���ݷ��ء��������ݷ��ء������ϱ� �����������·��ظ�
	unsigned char	packagenum;		//�����
	unsigned char	rt_update;      //Զ���������ر�־
	unsigned short	rt_updatetime;  //Զ���������ؼ�ʱ
	unsigned char 	real_send;		//ʵʱ���ݸ���
	unsigned char	heart_c;		//������ʱwifi��λ
    unsigned char   status_rt;      //״̬�ϱ�����
    unsigned char   setnet;         //��������״̬��־
};

struct DevData
{
	unsigned short 	inputword; 		//������
    unsigned char  	warnbuzzer;     //�澯��־
    unsigned char   trans;          //���ߴ����־ 1-���ӣ�0-�Ͽ�
    unsigned char   check;          //ģ���Լ�״̬
    //��⼤��
    unsigned short  actived;        //���״̬
    unsigned char   lcdonoff;       //����ر�
    unsigned short  blink;          //�����ʱ
    unsigned char   filtertime;     //��оʣ��ٷֱ���ʾʱ��     
    //ʱ������
    unsigned int   	devtim_on;		//
    unsigned int   	devtim_runtime;
	unsigned short 	devtim_connect;
    unsigned int   	devtim_now;
	unsigned char  	devtim_second;
    unsigned short  devtim_5min;    //5���Ӽ�ʱ
    //����������
	unsigned char  	pm25status;     //PM2.5������״̬
	unsigned short 	time_reset;		//��������λʱ��
    //ģʽ����
	unsigned short 	oldrunmode;     //��һ�ιػ�ʱ����ģʽ
    unsigned short 	prerunmode;     //ǰһ������ģʽ
    unsigned short  timecount;      //��ʱ����ʱ
    unsigned char   timeactived;    //��ʱ��ʾʱ�䵹��ʱ
    unsigned char   timemode;       //��ʱģʽ��0-��ʱ��ԤԼ���ֶ���ʱ��  1-APP��ʱ
    //������������
	unsigned short	lightvalue;		//��ǿ�Ȳ���ֵ	
	unsigned char	lightonoff;		//�����ڰ�
    unsigned char	keyonoff;		//�������⿪��
    unsigned char   keyblink;       //��������״̬
    //����������
	unsigned short 	buzzertime;		//������ʱ��
	unsigned short 	buzzertimes;	//����������
    //�����·�����
    unsigned char   net_automode;   //�����·�����ģʽ
    //��������
    unsigned int    usedfilter;     //������ʹ��ʱ��
    unsigned char   setmode;        //����״̬��־
};



extern struct tm		m_time_now; //���ڵ�����ʱ��
extern struct tm        m_time_set; //���õ�����ʱ��
extern struct SetData 	m_SetData;  //�������� 
extern struct RunData 	m_RunData;  //��������
extern struct NetData 	m_NetData;  //��������
extern struct DevData 	m_DevData;  //��������

extern unsigned short m_test_pwm;

extern __IO unsigned short 	m_timer_1ms;
extern __IO unsigned short 	m_timer_10ms;
extern __IO unsigned short 	m_timer_1000ms;
extern __IO unsigned short  m_buzzer_time;
extern void Data_Init( unsigned char init );
extern void num2str(unsigned char *str,unsigned char numpro,int val );
extern void DelayMicroSeconds(unsigned int nbrOfUs);
extern unsigned char Str_Cmp( unsigned char* Str, unsigned char* Src );
extern unsigned short Str_Cpy( unsigned char* Str,unsigned char* Src );
//extern unsigned short Str_Cpy( unsigned char* Str,unsigned char* Src );


#endif //_DATA_H_
/************** (C) COPYRIGHT 2017 ���ݿ������ܿƼ��ɷ����޹�˾ ***************/
