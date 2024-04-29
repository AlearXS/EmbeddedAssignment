/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//ָ��ģ��
#include "stm32f4xx_it.h"
#include "./as608/bsp_as608.h"
#include "./usart/rx_data_queue.h"
#include "./led/bsp_led.h"
 //����
#include "./systick/bsp_SysTick.h" 
#include "./usart/bsp_blt_usart.h"
#include "./usart/bsp_debug_usart.h"


 //����SysTick��ʱ 
#define TASK_DELAY_NUM  2       //����������������Լ�����ʵ������޸�
#define TASK_DELAY_0    500    //����0��ʱ 500*10 �����ִ�У���������Ƿ�������
#define TASK_DELAY_1    50     //����1��ʱ 50*10 �����ִ�У�

uint32_t Task_Delay_Group[TASK_DELAY_NUM];  //�������飬������ʱ�����ж��Ƿ�ִ�ж�Ӧ����

/* ÿ5s��������Ƿ����� */
int hc05_inquery_connect;




/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles AS608 Monitor exception.
  * @param  None
  * @retval None
  */
void AS608Mon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}
	
	
/**
  * @brief  ���� handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  int i;
  
  for(i=0; i<TASK_DELAY_NUM; i++)
  {
    Task_Delay_Group[i] ++;                   //�����ʱ��ʱ�䵽��ִ��
  }
  
  
  /* ��������0 */
  if(Task_Delay_Group[0] >= TASK_DELAY_0)     //�ж��Ƿ�ִ������0
  {
    Task_Delay_Group[0] = 0;                  //��0���¼�ʱ
    
    /* ����0����������Ƿ������� */
    
    hc05_inquery_connect = 1; //�������û�����ӣ���־λ��1
    
  }
  
  /* ��������1 */
  if(Task_Delay_Group[1] >= TASK_DELAY_1)     //�ж��Ƿ�ִ������1
  {
    Task_Delay_Group[1] = 0;                  //��0���¼�ʱ
    
    
    /* ����1��xxxxx */
    //printf("Test\r\n");
  }
}
        
  
  /**
  * @brief  ָ��ģ��:TouchOut����EXTI�ж�
  * @param  None
  * @retval None
  */
	
void AS608_TouchOut_IRQHandler(void)
{
  /*ȷ���Ƿ������EXTI Line�ж�*/
	
	if(EXTI_GetITStatus(AS608_TouchOut_INT_EXTI_LINE) != RESET) 
	{
		/*LED��ת*/	
		LED1_TOGGLE;

    EXTI_ClearITPendingBit(AS608_TouchOut_INT_EXTI_LINE);   
  }
}


  /**
* @brief  ָ��ģ��:�����жϷ�����,�ѽ��յ�������д�뻺������
            ��main��������ѯ�������������
  * @param  None
  * @retval None
  */
  void AS608_USART_IRQHandler(void)
{
 	uint8_t ucCh;
	QUEUE_DATA_TYPE *data_p; 
	
	if(USART_GetITStatus(AS608_USART,USART_IT_RXNE)!=RESET)
	{	
		ucCh  = USART_ReceiveData( AS608_USART );
		
						/*��ȡд������ָ�룬׼��д��������*/
		data_p = cbWrite(&rx_queue); 
		
		if (data_p != NULL)	//���������δ������ʼ����
		{		

			//��������д�����ݣ���ʹ�ô��ڽ��ա�dmaд��ȷ�ʽ
			*(data_p->head + data_p->len) = ucCh;
				
			if( ++data_p->len >= QUEUE_NODE_DATA_LEN)
			{
				cbWriteFinish(&rx_queue);
			}
		}else return;	
	}
	
	if ( USART_GetITStatus( AS608_USART, USART_IT_IDLE ) == SET )                                         //����֡�������
	{
			/*д�뻺�������*/
			cbWriteFinish(&rx_queue);
		ucCh = USART_ReceiveData( AS608_USART );                                                              //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)

	}
}	



//����ģ��:���Դ����жϻ��洮������
ReceiveData DEBUG_USART_ReceiveData;

// �����жϷ�����
void DEBUG_USART_IRQHandler(void)
{
		uint8_t ucCh; 
    if(USART_GetITStatus(DEBUG_USART, USART_IT_RXNE) != RESET)
    {
      ucCh = USART_ReceiveData(DEBUG_USART);
      if(DEBUG_USART_ReceiveData.datanum < UART_BUFF_SIZE)
        {
          if((ucCh != 0x0a) && (ucCh != 0x0d))
          {
            DEBUG_USART_ReceiveData.uart_buff[DEBUG_USART_ReceiveData.datanum] = ucCh;                 //�����ջ��лس�
            DEBUG_USART_ReceiveData.datanum++;
          }

        }         
     }
		if(USART_GetITStatus( DEBUG_USART, USART_IT_IDLE ) == SET )                                         //����֡�������
    {
        DEBUG_USART_ReceiveData.receive_data_flag = 1;
        USART_ReceiveData( DEBUG_USART );                                                              //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)	
    }	
} 


//����ģ��:���������жϻ��洮������
ReceiveData BLT_USART_ReceiveData;

void BLT_USART_IRQHandler(void)
{
    uint8_t ucCh; 
    if(USART_GetITStatus(BLT_USARTx, USART_IT_RXNE) != RESET)
    {
      ucCh = USART_ReceiveData(BLT_USARTx);
      if(BLT_USART_ReceiveData.datanum < UART_BUFF_SIZE)
      {
        if((ucCh != 0x0a) && (ucCh != 0x0d))
        {
          BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = ucCh;                 //�����ջ��лس�
          BLT_USART_ReceiveData.datanum++;
        }
      }
    }
		if(USART_GetITStatus( BLT_USARTx, USART_IT_IDLE ) == SET )                                         //����֡�������
    {
        BLT_USART_ReceiveData.receive_data_flag = 1;
        USART_ReceiveData( BLT_USARTx );                                                              //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)	
    }	

}

extern void TimingDelay_Decrement(void);
void SysTick_DHT11_Handler(void)
{
  TimingDelay_Decrement();
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
