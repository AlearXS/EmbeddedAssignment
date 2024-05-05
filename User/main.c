/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05����ģ����Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_blt_usart.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./hc05/bsp_hc05.h"
#include "./key/bsp_key.h" 
#include <string.h>
#include <stdlib.h>
#include "./dwt_delay/core_delay.h"   
#include "./i2c/i2c.h"
#include "./mpu6050/mpu6050.h"
#include "./as608/bsp_as608.h"
#include "./as608/as608_test.h"
#include "./beep/bsp_beep.h"
#include "./lcd/bsp_ili9806g_lcd.h"



extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
extern int hc05_inquery_connect;

//��������
void Configure_all(void);
void show_tip(void);

// ��ʱ��������
int blt_unlocking(int *id){*id = 1; return 0;};


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  Configure_all();

  //���������ʾ��
  int x, id, user_id, res;
  do{
    // int x, id, res;
    show_tip();
    // while(~scanf("%d", &x))printf("test %d\r\n", x);
    scanf("%d", &x);
    // while(1){
    // }
    // printf("test\r\n");
    switch (x)
    {
    case 1:
      res = FR_unlocking(&id);
      break;
    case 2:
      res = blt_unlocking(&id);
      break;
    case 3:
      // TODO��ע��
      res = 3;
      printf("ע�Ṧ�ܻ��ڿ�����");
      break;
    case 4:
      // TODO: ����
      // break;
    default:
      printf("δ֪��ħ�����㵽����ʲô�ˣ�");
      res = 2;
      break;
    }
    printf("���λ��۽𾦵Ľ���ǣ�%d\r\n\r\n", res);
    Delay_ms(500);
  }while(res);
  printf("��½�ɹ� ID: %d\r\n", id);
  //ѡ�������ʽ

  //��ӭҳ
  //TODO: �ĳ�lcd��ʾ
  printf("ID: %d\r\n", user_id);
  // TODO: ����û���
  // TODO: �����ʪ��
  printf("��Key1�������˹���飬��key2���뻭ͼ����");
  // TODO
  while(1);
  
}


/**
  * @brief  ȫ����������
  * @param  ��
  * @retval ��
  */
void Configure_all(void){

	/* ��ʱ������ʼ�� */
  CPU_TS_TmrInit();

  /* LCD��ʼ�� */
  #ifdef ENABLE_LCD_DISPLAY
	NT35510_Init();	
	NT35510_GramScan( 6 );
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);
  NT35510_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  #endif
	
  /* ���Դ��ڳ�ʼ�� USART1 ����ģʽΪ 115200 8-N-1 �����ж� */
	Debug_USART_Config();

	LED_GPIO_Config();
	Key_GPIO_Config();

	/*�������˿ڳ�ʼ�� */
	BEEP_GPIO_Config(); 
	
  ILI9806G_Init ();         //LCD ��ʼ��
  
	//����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
  //���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
  //���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
  ILI9806G_GramScan ( 6 );
	

  
  /* �������ֳ�ʼ�� */
  char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
  
  unsigned char mpu_reg = 0; //�����ȡMPU6050�Ĵ�����ֵ
  /* HC05����ģ���ʼ����GPIO �� USART3 ����ģʽΪ 38400 8-N-1 �����ж� */
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05ģ����������");
		#ifdef ENABLE_LCD_DISPLAY
		NT35510_DispString_EN ( 40, 60, "HC05 module detected!" );
    	#endif
	}
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
		#ifdef ENABLE_LCD_DISPLAY
		NT35510_DispString_EN ( 20, 60, "No HC05 module detected!"  );
		NT35510_DispString_EN ( 5, 100, "Please check the hardware connection and reset the system." );
    #endif
    
		while(1);
	}
  
	/*��λ���ָ�Ĭ��״̬*/
	HC05_Send_CMD("AT+RESET\r\n",1);	//��λָ������֮����Ҫһ��ʱ��HC05�Ż������һ��ָ��
	HC05_Send_CMD("AT+ORGL\r\n",1);

	
	/*�������������ʾ��Ĭ�ϲ���ʾ��
	 *��bsp_hc05.h�ļ���HC05_DEBUG_ON ������Ϊ1��
	 *����ͨ�����ڵ������ֽ��յ�����Ϣ*/	
	
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	/*��ʼ��SPP�淶*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*����ģ������*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("��ģ������Ϊ:%s ,ģ����׼��������",hc05_name);
	#ifdef ENABLE_LCD_DISPLAY
	NT35510_DispStringLine_EN ( (LINE(4)), "ReceiveData USART1" );	
  NT35510_DispStringLine_EN ( (LINE(12)), "ReceiveData HC-05" );
  #endif
  
  
  
  /* �ⲿ���Ƕ���Ĵ���
     Ŀ����Ϊ�˷�ֹ���� MPU6050 ��INT���Ÿ���STM32 PF10���ŵĵ�ƽ�仯
     ʹ�ð��컢�������ⲿ�ֲ���ȥ��������Ӱ��HC05������״̬�ļ��
  */
	I2cMaster_Init(); //��ʼ�� I2C
	if( ! MPU6050ReadID() ) {printf("�޷���ȡMPU6050\r\n"); while(1);}
  MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG, (mpu_reg |((1<<MPU6050_INTCFG_INT_LEVEL_BIT) | 
                                                      (1<<MPU6050_INTCFG_INT_OPEN_BIT) | 
                                                      (1<<MPU6050_INTCFG_LATCH_INT_EN_BIT)) ) );
  
  
  /* SysTick 10ms�жϳ�ʼ�� */
  SysTick_Init();
  

  /* ָ��ʶ�𲿷ֳ�ʼ�� */
  
  /*��ʼ�����λ�����*/
  rx_queue_init();
   
  /*��ʼ��ָ��ģ������*/
	AS608_Config();
  
	/*����STM32��ָ��ģ���ͨ��*/
  Connect_Test();
}

/**
  * @brief  �����ʼ����ʱ����ʾ
  * @param  ��
  * @retval ��
  */
void show_tip(void){
  printf("��ӭ�μ��ⳡʢ��Ŀ���\r\n");
  printf("��ѡ����Ľ��뷽ʽ��\r\n");
  printf("1: �ʺ����ָ��֤������ҳ�\r\n");
  printf("2: ��ɫ�����������޵�Զ���й���\r\n");
  printf("3: ��ħ����Լ����������\r\n");
  printf("3: Сè�俴�������\r\n");
  printf("�ǵ�����س�\r\n");
 }


/*********************************************END OF FILE**********************/

