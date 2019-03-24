/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "task.h"

TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;
#define USE_SEMIHOSTING 0
char usr_msg[250];
void vTask1_handler (void *params);
void vTask2_handler (void *params);

#if(USE_SEMIHOSTING)
extern void initialise_monitor_handles();
#endif
static void prvSetupHardware(void);
static void prvSetupUART(void);
uint8_t UART_ACCESS_KEY = 1;
#define TRUE 1
#define FALSE 0
#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE
void printmsg(char *msg);
int main(void)
{

#if(USE_SEMIHOSTING)
	initialise_monitor_handles();
	printf("Hello world from main \n");
#endif
	DWT->CTRL |= (1<<0); //Enable CYCNT in DWT_CTRL

	RCC_DeInit();
	SystemCoreClockUpdate();

	prvSetupHardware();
	sprintf(usr_msg,"This is hello world of my application\r\n");
	printmsg(usr_msg);

	// Start recording SEGGER
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	xTaskCreate(vTask1_handler,"Task-1",configMINIMAL_STACK_SIZE, NULL,2,&xTaskHandle1);
	xTaskCreate(vTask2_handler,"Task-2",configMINIMAL_STACK_SIZE, NULL,2,&xTaskHandle2);

	//start the schedular
	vTaskStartScheduler();

	for(;;);
}

void vTask1_handler (void *params)
{
	while(1)
	{
		if(UART_ACCESS_KEY == AVAILABLE)
		{
			UART_ACCESS_KEY = NOT_AVAILABLE;
			printmsg("Hello from Task 1\r\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}

	}
}

void vTask2_handler (void *params)
{
	while(1)
	{
		if(UART_ACCESS_KEY == AVAILABLE)
		{
			UART_ACCESS_KEY = NOT_AVAILABLE;
			printmsg("Hello from Task 2\r\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}
	}
}
static void prvSetupUART(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;
	//1. Enable USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	// PA2
	memset(&gpio_uart_pins,0,sizeof(gpio_uart_pins));
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	// UART pheripheral configuration
	memset(&uart2_init,0,sizeof(gpio_uart_pins));
	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&uart2_init);

	USART_Cmd(USART2,ENABLE);
}
static void prvSetupHardware(void)
{
	prvSetupUART();
}

void printmsg(char *msg)

{
	for(uint32_t i=0;i<strlen(msg);i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
		USART_SendData(USART2,msg[i]);
	}

}
