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
#include "FATFS/ff.h"
#include "./flash/bsp_spi_flash.h"
#include "./user/user.h"


#define PADDING_LEFT (2 * LCD_GetFont()->Width)
/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_flash;                /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
"��ӭʹ��Ұ��STM32 F429������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";  

//�ⲿ����
extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
extern int hc05_inquery_connect;

//��������
void Configure_all(void);
void show_tip(void);
void ask(int pre, int x);

// ��ʱ��������

// ȫ�ֱ���
#define BUFF_SIZE 256
char buf[BUFF_SIZE];

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  Configure_all();
  // Register(0, "Alear", "123456");
  //���������ʾ��
  int x = 1, id, user_id, res, pre = 0;
  CLS;
  StringLine(PADDING_LEFT, 0, "Hello World!");
  StringLine(PADDING_LEFT, (1), "1: Fingerprint Recognition");
  StringLine(PADDING_LEFT, (2), "2: Bluetooth");
  StringLine(PADDING_LEFT, (3), "3: Register");
  StringLine(PADDING_LEFT, (4), "4: Delete user");
  StringLine(PADDING_LEFT, (5), "5: Show user table");
  StringLine(PADDING_LEFT, (6), "Press key1 to select the next item.");
  StringLine(PADDING_LEFT, (8), "press key2 to confirm.");

  do{
    // int x, id, res;
    show_tip();
    // while(~scanf("%d", &x))printf("test %d\r\n", x);
    // x = 1;
    while(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_OFF){
      if(x != pre)
        ask(pre, x), pre = x;
      if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON){
        x++;
        if(x > 5)
          x = 1;
      }
    }
    
    // scanf("%d", &x);
    switch (x)
    {
    case 1:
      res = FR_unlocking(&id);
      break;
    case 2:
      res = blt_unlocking(&id);
      break;
    case 3:
      res = FR_Register();
      res = 1;
      break;
    case 4:
      res = FR_Delete();
      res = 1;
      break;
    case 5:
      puts("����û���");
      load_user_table();
      for(int i=0;i<MAX_USERS;i++){
        if(users[i].status == USER_STATUS_REGISTERED)
          printf("%d %s %s\r\n", users[i].id, users[i].username, users[i].password);
      }
			res = 1;
      break;
    default:
      printf("δ֪��ħ�����㵽����ʲô�ˣ�");
      res = 2;
      break;
    }
    printf("���λ��۽𾦵Ľ���ǣ�%d\r\n\r\n", res);
    Delay_ms(500);
  }while(res);
  printf("��½�ɹ� ID: %d\r\n", id);

  while(1){
    //��ӭҳ
    
    sprintf(buf, "ID: %d", user_id);
    puts(buf);
    StringLine(PADDING_LEFT, 1, buf);

    sprintf(buf, "User_name: %s", users[id].username);
    puts(buf);
    StringLine(PADDING_LEFT, 2, buf);

    sprintf(buf, "wet: ");
    puts(buf);
    StringLine(PADDING_LEFT, 3, buf);

    sprintf(buf, "press key1 to enter TETRIS");
    puts(buf);
    StringLine(PADDING_LEFT, 4, buf);

    sprintf(buf, "press key2 to enter PAINTING");
    puts(buf);
    StringLine(PADDING_LEFT, 5, buf);
    puts("��Key1�������˹���飬��key2���뻭ͼ����");

    while(1){
      if(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN)){
        // TETRIS(id);
      }
      if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN)){

      }
    }
  }
  
}


/**
  * @brief  ȫ����������
  * @param  ��
  * @retval ��
  */
void Configure_all(void){

	/* ��ʱ������ʼ�� */
  CPU_TS_TmrInit();
	
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

  /*��ʼ���ļ�ϵͳ*/
  
  printf("****** ����һ��SPI FLASH �ļ�ϵͳʵ�� ******\r\n");
  
	//���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	res_flash = f_mount(&fs,"1:",1);
	
/*----------------------- ��ʽ������ ---------------------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("��FLASH��û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_flash=f_mkfs("1:",0,0);							
		
		if(res_flash == FR_OK)
		{
			printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_flash = f_mount(NULL,"1:",1);			
      /* ���¹���	*/			
			res_flash = f_mount(&fs,"1:",1);
		}
		else
		{
			LED_RED;
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res_flash!=FR_OK)
  {
    printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
    printf("��������ԭ��SPI Flash��ʼ�����ɹ���\r\n");
	printf("������ SPI����д����FLASH ���̲��ԣ�����������ڸ�����f_mount�����if���ǰ��ʱ�����һ�� res_flash = FR_NO_FILESYSTEM; ������ֱ��ִ�и�ʽ������\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }
  
/*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
	/* ���ļ�������ļ��������򴴽��� */
	printf("\r\n****** ���������ļ�д�����... ******\r\n");	
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_flash == FR_OK )
	{
		printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		res_flash=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_flash==FR_OK)
    {
      printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
	}
	else
	{	
		LED_RED;
		printf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
/*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
	printf("****** ���������ļ���ȡ����... ******\r\n");
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(res_flash == FR_OK)
	{
		LED_GREEN;
		printf("�����ļ��ɹ���\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_flash==FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_flash);
    }		
	}
	else
	{
		LED_RED;
		printf("�������ļ�ʧ�ܡ�\r\n");
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);	
  /*end of ��ʼ���ļ�ϵͳ*/
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

void ask(int pre, int x){
  ILI9806G_DispChar_EN(0, LINE(pre), ' ');
  ILI9806G_DispChar_EN(0, LINE(x), '>');
}

/*********************************************END OF FILE**********************/

