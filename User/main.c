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



extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
extern int hc05_inquery_connect;

//函数声明
void Configure_all(void);
void show_tip(void);

// 临时函数声明
int blt_unlocking(int *id){*id = 1; return 0;};


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  Configure_all();

  //串口输出提示语
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
      // TODO：注册
      res = 3;
      printf("注册功能还在开发中");
      break;
    case 4:
      // TODO: 管理
      // break;
    default:
      printf("未知的魔法，你到底是什么人？");
      res = 2;
      break;
    }
    printf("本次火眼金睛的结果是：%d\r\n\r\n", res);
    Delay_ms(500);
  }while(res);
  printf("登陆成功 ID: %d\r\n", id);
  //选择解锁方式

  //欢迎页
  //TODO: 改成lcd显示
  printf("ID: %d\r\n", user_id);
  // TODO: 输出用户名
  // TODO: 输出温湿度
  printf("按Key1进入俄罗斯方块，按key2进入画图程序");
  // TODO
  while(1);
  
}


/**
  * @brief  全局启动函数
  * @param  无
  * @retval 无
  */
void Configure_all(void){

	/* 延时函数初始化 */
  CPU_TS_TmrInit();

  /* LCD初始化 */
  #ifdef ENABLE_LCD_DISPLAY
	NT35510_Init();	
	NT35510_GramScan( 6 );
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);
  NT35510_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
  #endif
	
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
		#ifdef ENABLE_LCD_DISPLAY
		NT35510_DispString_EN ( 20, 60, "No HC05 module detected!"  );
		NT35510_DispString_EN ( 5, 100, "Please check the hardware connection and reset the system." );
    #endif
    
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
	#ifdef ENABLE_LCD_DISPLAY
	NT35510_DispStringLine_EN ( (LINE(4)), "ReceiveData USART1" );	
  NT35510_DispStringLine_EN ( (LINE(12)), "ReceiveData HC-05" );
  #endif
  
  
  
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


/*********************************************END OF FILE**********************/

