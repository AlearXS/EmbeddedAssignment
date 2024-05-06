/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05蓝牙模块测试程序
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
  *                              定义变量
  ******************************************************************************
  */
FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res_flash;                /* 文件操作结果 */
UINT fnum;            					  /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区*/
"欢迎使用野火STM32 F429开发板 今天是个好日子，新建文件系统测试文件\r\n";  

//外部变量
extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
extern int hc05_inquery_connect;

//函数声明
void Configure_all(void);
void show_tip(void);
void ask(int pre, int x);

// 临时函数声明

// 全局变量
#define BUFF_SIZE 256
char buf[BUFF_SIZE];

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  Configure_all();
  // Register(0, "Alear", "123456");
  //串口输出提示语
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
      puts("输出用户表");
      load_user_table();
      for(int i=0;i<MAX_USERS;i++){
        if(users[i].status == USER_STATUS_REGISTERED)
          printf("%d %s %s\r\n", users[i].id, users[i].username, users[i].password);
      }
			res = 1;
      break;
    default:
      printf("未知的魔法，你到底是什么人？");
      res = 2;
      break;
    }
    printf("本次火眼金睛的结果是：%d\r\n\r\n", res);
    Delay_ms(500);
  }while(res);
  printf("登陆成功 ID: %d\r\n", id);

  while(1){
    //欢迎页
    
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
    puts("按Key1进入俄罗斯方块，按key2进入画图程序");

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
  * @brief  全局启动函数
  * @param  无
  * @retval 无
  */
void Configure_all(void){

	/* 延时函数初始化 */
  CPU_TS_TmrInit();
	
  /* 调试串口初始化 USART1 配置模式为 115200 8-N-1 接收中断 */
	Debug_USART_Config();

	LED_GPIO_Config();
	Key_GPIO_Config();

	/*蜂鸣器端口初始化 */
	BEEP_GPIO_Config(); 
	
  ILI9806G_Init ();         //LCD 初始化
  
	//其中0、3、5、6 模式适合从左至右显示文字，
  //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
  //其中 6 模式为大部分液晶例程的默认显示方向  
  ILI9806G_GramScan ( 6 );
	

  
  /* 蓝牙部分初始化 */
  char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
  
  unsigned char mpu_reg = 0; //保存读取MPU6050寄存器的值
  /* HC05蓝牙模块初始化：GPIO 和 USART3 配置模式为 38400 8-N-1 接收中断 */
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05模块检测正常。");
		#ifdef ENABLE_LCD_DISPLAY
		NT35510_DispString_EN ( 40, 60, "HC05 module detected!" );
    	#endif
	}
	else
	{
		HC05_ERROR("HC05模块检测不正常，请检查模块与开发板的连接，然后复位开发板重新测试。");
    
		while(1);
	}
  
	/*复位、恢复默认状态*/
	HC05_Send_CMD("AT+RESET\r\n",1);	//复位指令发送完成之后，需要一定时间HC05才会接受下一条指令
	HC05_Send_CMD("AT+ORGL\r\n",1);

	
	/*各种命令测试演示，默认不显示。
	 *在bsp_hc05.h文件把HC05_DEBUG_ON 宏设置为1，
	 *即可通过串口调试助手接收调试信息*/	
	
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	/*初始化SPP规范*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*设置模块名字*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("本模块名字为:%s ,模块已准备就绪。",hc05_name);
  
  
  
  /* 这部分是额外的代码
     目的是为了防止板载 MPU6050 的INT引脚干扰STM32 PF10引脚的电平变化
     使用霸天虎开发板这部分不可去掉，否则影响HC05的连接状态的检测
  */
	I2cMaster_Init(); //初始化 I2C
	if( ! MPU6050ReadID() ) {printf("无法读取MPU6050\r\n"); while(1);}
  MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG, (mpu_reg |((1<<MPU6050_INTCFG_INT_LEVEL_BIT) | 
                                                      (1<<MPU6050_INTCFG_INT_OPEN_BIT) | 
                                                      (1<<MPU6050_INTCFG_LATCH_INT_EN_BIT)) ) );
  
  
  /* SysTick 10ms中断初始化 */
  SysTick_Init();
  

  /* 指纹识别部分初始化 */
  
  /*初始化环形缓冲区*/
  rx_queue_init();
   
  /*初始化指纹模块配置*/
	AS608_Config();
  
	/*测试STM32与指纹模块的通信*/
  Connect_Test();

  /*初始化文件系统*/
  
  printf("****** 这是一个SPI FLASH 文件系统实验 ******\r\n");
  
	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_flash = f_mount(&fs,"1:",1);
	
/*----------------------- 格式化测试 ---------------------------*/  
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("》FLASH还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_flash=f_mkfs("1:",0,0);							
		
		if(res_flash == FR_OK)
		{
			printf("》FLASH已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_flash = f_mount(NULL,"1:",1);			
      /* 重新挂载	*/			
			res_flash = f_mount(&fs,"1:",1);
		}
		else
		{
			LED_RED;
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(res_flash!=FR_OK)
  {
    printf("！！外部Flash挂载文件系统失败。(%d)\r\n",res_flash);
    printf("！！可能原因：SPI Flash初始化不成功。\r\n");
	printf("请下载 SPI—读写串行FLASH 例程测试，如果正常，在该例程f_mount语句下if语句前临时多添加一句 res_flash = FR_NO_FILESYSTEM; 让重新直接执行格式化流程\r\n");
		while(1);
  }
  else
  {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }
  
/*----------------------- 文件系统测试：写测试 -----------------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");	
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_flash == FR_OK )
	{
		printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
		res_flash=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_flash==FR_OK)
    {
      printf("》文件写入成功，写入字节数据：%d\n",fnum);
      printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("！！文件写入失败：(%d)\n",res_flash);
    }    
		/* 不再读写，关闭文件 */
    f_close(&fnew);
	}
	else
	{	
		LED_RED;
		printf("！！打开/创建文件失败。\r\n");
	}
	
/*------------------- 文件系统测试：读测试 ------------------------------------*/
	printf("****** 即将进行文件读取测试... ******\r\n");
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(res_flash == FR_OK)
	{
		LED_GREEN;
		printf("》打开文件成功。\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_flash==FR_OK)
    {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("！！文件读取失败：(%d)\n",res_flash);
    }		
	}
	else
	{
		LED_RED;
		printf("！！打开文件失败。\r\n");
	}
	/* 不再读写，关闭文件 */
	f_close(&fnew);	
  /*end of 初始化文件系统*/
}

/**
  * @brief  输出初始进入时的提示
  * @param  无
  * @retval 无
  */
void show_tip(void){
  printf("欢迎参加这场盛大的开演\r\n");
  printf("请选择你的进入方式：\r\n");
  printf("1: 鲜红的手指见证永恒的忠诚\r\n");
  printf("2: 蓝色的心灵在无限的远方中共鸣\r\n");
  printf("3: 恶魔的契约贱卖你的灵魂\r\n");
  printf("3: 小猫咪看不得这个\r\n");
  printf("记得输出回车\r\n");
 }

void ask(int pre, int x){
  ILI9806G_DispChar_EN(0, LINE(pre), ' ');
  ILI9806G_DispChar_EN(0, LINE(x), '>');
}

/*********************************************END OF FILE**********************/

