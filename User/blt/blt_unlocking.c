/*
   @brief   HC05蓝牙模块登录程序
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
#include "./effect/effect.h"
#include "./user/user.h"


extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
extern int hc05_inquery_connect;

void blt_process_received_ID();
void blt_process_received_PASSWORD();

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int blt_unlocking(int *id)
{

  static char id_buffer[20] = {0}; // ID缓冲区，假设ID长度不超过19字符
  static char password_buffer[20] = {0}; // 密码缓冲区，假设密码长度不超过19字符
  static int id_received = 0; // 标记ID是否已接收
  static int password_received = 0; // 标记密码是否已接收

	while(1)
	{
    //每 5s 检查一次蓝牙连接
    if( 1 == hc05_inquery_connect )
    {
      hc05_inquery_connect = 0; //清零标志位

      if( ! IS_HC05_CONNECTED() )
      {
        HC05_Send_CMD("AT+INQ\r\n",1);//模块在查询状态，才能容易被其它设备搜索到
        Usart_SendString( DEBUG_USART, "蓝牙尚未连接。请用手机打开蓝牙调试助手搜索连接蓝牙\r\n" );
      }
      else
      {
        Usart_SendString( DEBUG_USART, "蓝牙已连接,请输入账号ID\r\n" );
      }
    }
    // puts("???fefe");
    
    if(BLT_USART_ReceiveData.receive_data_flag == 1)
    {
      // 清零字符串结束符，以便重新接收数据
      BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
      // 清空接收标志和数据计数器，准备接收新数据
      BLT_USART_ReceiveData.receive_data_flag = 0;
      BLT_USART_ReceiveData.datanum = 0;

      if(strstr(BLT_USART_ReceiveData.uart_buff, "OK") || strstr(BLT_USART_ReceiveData.uart_buff, "FAIL")){
        puts("蓝牙响应");
      }else{
        puts("收到的数据: ");
        puts(BLT_USART_ReceiveData.uart_buff);
        // 如果ID还没接收到，尝试从数据中提取ID
        if (!id_received) {
              strcpy(id_buffer, BLT_USART_ReceiveData.uart_buff);
              id_received = 1;
              Usart_SendString( DEBUG_USART, "\r\n账号ID输入成功：\r\n" );
              *id = atoi(id_buffer);// to int 
          }
        break;

        }
    }
	}

  // 输入密码
  while(1)
	{
    //每 5s 检查一次蓝牙连接
    if( 1 == hc05_inquery_connect )
    {
      hc05_inquery_connect = 0; //清零标志位

      if( ! IS_HC05_CONNECTED() )
      {
        HC05_Send_CMD("AT+INQ\r\n",1);//模块在查询状态，才能容易被其它设备搜索到
        Usart_SendString( DEBUG_USART, "蓝牙尚未连接。请用手机打开蓝牙调试助手搜索连接蓝牙\r\n" );
      }
      else
      {
        Usart_SendString( DEBUG_USART, "\r\n请输入密码：\r\n" );
      }
    }
    
    //   if(BLT_USART_ReceiveData.receive_data_flag == 1)
    // {
    //     // 清零字符串结束符，以便重新接收数据
    //     BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;

    //     // 如果ID还没接收到，尝试从数据中提取ID
    //     if (!password_received) {
    //           strcpy(password_buffer, BLT_USART_ReceiveData.uart_buff);
    //           password_received = 1;
    //           Usart_SendString( DEBUG_USART, "\r\n密码输入成功：\r\n" );
    //         }

    //       // 清空接收标志和数据计数器，准备接收新数据
    //       BLT_USART_ReceiveData.receive_data_flag = 0;
    //       BLT_USART_ReceiveData.datanum = 0;
    //       break;
    // } 
  
    if(BLT_USART_ReceiveData.receive_data_flag == 1)
    {
      // 清零字符串结束符，以便重新接收数据
      BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
      // 清空接收标志和数据计数器，准备接收新数据
      BLT_USART_ReceiveData.receive_data_flag = 0;
      BLT_USART_ReceiveData.datanum = 0;

      if(strstr(BLT_USART_ReceiveData.uart_buff, "OK") || strstr(BLT_USART_ReceiveData.uart_buff, "FAIL")){
        puts("蓝牙响应");
      }else{
        
        // 如果ID还没接收到，尝试从数据中提取ID
        if (!password_received) {
              strcpy(password_buffer, BLT_USART_ReceiveData.uart_buff);
              password_received = 1;
              Usart_SendString( DEBUG_USART, "\r\n密码输入成功：\r\n" );
            }

        }
        break;
    }
	}
  // 如果两者都接收到了，进行后续处理
        if (id_received && password_received) {
            // 这里添加处理ID和密码的逻辑
           
					if(!check(atoi(id_buffer), password_buffer))
          {
						isRight();
						
						 // 重置状态，准备接收下一轮数据
            id_received = 0;
            password_received = 0;
            memset(id_buffer, 0, sizeof(id_buffer));
            memset(password_buffer, 0, sizeof(password_buffer));
						Usart_SendString( DEBUG_USART, "\r\n登入成功：\r\n" );
						return 0;
          }	
					else
					{
          isWrong();
					// 重置状态，准备接收下一轮数据
          id_received = 0;
          password_received = 0;
          memset(id_buffer, 0, sizeof(id_buffer));
          memset(password_buffer, 0, sizeof(password_buffer));
          Usart_SendString( DEBUG_USART, "\r\n登入失败，账户或密码不正确：\r\n" );
					return 1;
					}		
       }
       else
       {
          ( DEBUG_USART, "\r\n账户或密码输入为空：\r\n" ); 
          return 1;
       }
}


/*********************************************END OF FILE**********************/
