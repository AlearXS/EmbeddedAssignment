/*
   @brief   HC05����ģ���¼����
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
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int blt_unlocking(int *id)
{

  static char id_buffer[20] = {0}; // ID������������ID���Ȳ�����19�ַ�
  static char password_buffer[20] = {0}; // ���뻺�������������볤�Ȳ�����19�ַ�
  static int id_received = 0; // ���ID�Ƿ��ѽ���
  static int password_received = 0; // ��������Ƿ��ѽ���

	while(1)
	{
    //ÿ 5s ���һ����������
    if( 1 == hc05_inquery_connect )
    {
      hc05_inquery_connect = 0; //�����־λ

      if( ! IS_HC05_CONNECTED() )
      {
        HC05_Send_CMD("AT+INQ\r\n",1);//ģ���ڲ�ѯ״̬���������ױ������豸������
        Usart_SendString( DEBUG_USART, "������δ���ӡ������ֻ���������������������������\r\n" );
      }
      else
      {
        Usart_SendString( DEBUG_USART, "����������,�������˺�ID\r\n" );
      }
    }
    // puts("???fefe");
    
    if(BLT_USART_ReceiveData.receive_data_flag == 1)
    {
      // �����ַ������������Ա����½�������
      BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
      // ��ս��ձ�־�����ݼ�������׼������������
      BLT_USART_ReceiveData.receive_data_flag = 0;
      BLT_USART_ReceiveData.datanum = 0;

      if(strstr(BLT_USART_ReceiveData.uart_buff, "OK") || strstr(BLT_USART_ReceiveData.uart_buff, "FAIL")){
        puts("������Ӧ");
      }else{
        puts("�յ�������: ");
        puts(BLT_USART_ReceiveData.uart_buff);
        // ���ID��û���յ������Դ���������ȡID
        if (!id_received) {
              strcpy(id_buffer, BLT_USART_ReceiveData.uart_buff);
              id_received = 1;
              Usart_SendString( DEBUG_USART, "\r\n�˺�ID����ɹ���\r\n" );
              *id = atoi(id_buffer);// to int 
          }
        break;

        }
    }
	}

  // ��������
  while(1)
	{
    //ÿ 5s ���һ����������
    if( 1 == hc05_inquery_connect )
    {
      hc05_inquery_connect = 0; //�����־λ

      if( ! IS_HC05_CONNECTED() )
      {
        HC05_Send_CMD("AT+INQ\r\n",1);//ģ���ڲ�ѯ״̬���������ױ������豸������
        Usart_SendString( DEBUG_USART, "������δ���ӡ������ֻ���������������������������\r\n" );
      }
      else
      {
        Usart_SendString( DEBUG_USART, "\r\n���������룺\r\n" );
      }
    }
    
    //   if(BLT_USART_ReceiveData.receive_data_flag == 1)
    // {
    //     // �����ַ������������Ա����½�������
    //     BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;

    //     // ���ID��û���յ������Դ���������ȡID
    //     if (!password_received) {
    //           strcpy(password_buffer, BLT_USART_ReceiveData.uart_buff);
    //           password_received = 1;
    //           Usart_SendString( DEBUG_USART, "\r\n��������ɹ���\r\n" );
    //         }

    //       // ��ս��ձ�־�����ݼ�������׼������������
    //       BLT_USART_ReceiveData.receive_data_flag = 0;
    //       BLT_USART_ReceiveData.datanum = 0;
    //       break;
    // } 
  
    if(BLT_USART_ReceiveData.receive_data_flag == 1)
    {
      // �����ַ������������Ա����½�������
      BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
      // ��ս��ձ�־�����ݼ�������׼������������
      BLT_USART_ReceiveData.receive_data_flag = 0;
      BLT_USART_ReceiveData.datanum = 0;

      if(strstr(BLT_USART_ReceiveData.uart_buff, "OK") || strstr(BLT_USART_ReceiveData.uart_buff, "FAIL")){
        puts("������Ӧ");
      }else{
        
        // ���ID��û���յ������Դ���������ȡID
        if (!password_received) {
              strcpy(password_buffer, BLT_USART_ReceiveData.uart_buff);
              password_received = 1;
              Usart_SendString( DEBUG_USART, "\r\n��������ɹ���\r\n" );
            }

        }
        break;
    }
	}
  // ������߶����յ��ˣ����к�������
        if (id_received && password_received) {
            // ������Ӵ���ID��������߼�
           
					if(!check(atoi(id_buffer), password_buffer))
          {
						isRight();
						
						 // ����״̬��׼��������һ������
            id_received = 0;
            password_received = 0;
            memset(id_buffer, 0, sizeof(id_buffer));
            memset(password_buffer, 0, sizeof(password_buffer));
						Usart_SendString( DEBUG_USART, "\r\n����ɹ���\r\n" );
						return 0;
          }	
					else
					{
          isWrong();
					// ����״̬��׼��������һ������
          id_received = 0;
          password_received = 0;
          memset(id_buffer, 0, sizeof(id_buffer));
          memset(password_buffer, 0, sizeof(password_buffer));
          Usart_SendString( DEBUG_USART, "\r\n����ʧ�ܣ��˻������벻��ȷ��\r\n" );
					return 1;
					}		
       }
       else
       {
          ( DEBUG_USART, "\r\n�˻�����������Ϊ�գ�\r\n" ); 
          return 1;
       }
}


/*********************************************END OF FILE**********************/
