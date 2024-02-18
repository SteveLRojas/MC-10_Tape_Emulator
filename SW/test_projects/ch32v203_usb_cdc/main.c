/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/08/08
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/ 

/*
 *@Note
 *Example routine to emulate a simulate USB-CDC Device, USE USART2(PA2/PA3);
 *Please note: This code uses the default serial port 1 for debugging,
 *if you need to modify the debugging serial port, please do not use USART2
*/


#include "debug.h"
#include "cdc_serial.h"

void platform_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE );

	GPIO_InitTypeDef GPIO_InitStructure = {0};

	// Init PORTB (LCD_CTRL)
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	platform_init();
	Delay_Init(); 
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf("USBD SimulateCDC Demo\r\n");

	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
	Delay_Ms(100);
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
	Delay_Ms(100);

	cdc_init();
 	
	while(1)
	{
		// UART2_DataRx_Deal( );
		// UART2_DataTx_Deal( );

		// If there's stuff to read, read it
		// Copy into TM FIFO
		while(cdc_bytes_available())
		{
			cdc_write_byte(cdc_read_byte());
		}

		cdc_task();
	}
}







