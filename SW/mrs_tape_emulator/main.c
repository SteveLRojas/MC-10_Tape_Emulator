/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "display.h"

/* Global define */

/* Global Variable */

void gpio_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	PWR_WakeUpPinCmd(DISABLE);

	GPIO_InitTypeDef GPIO_InitStructure = {0};

	// Init PORTA (LCD_BUS)
	GPIO_InitStructure.GPIO_Pin = (0x00FF);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_LCD_BUS, &GPIO_InitStructure);

	// Init PORTB (LCD_CTRL)
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1 | PIN_LCD_CTRL_RW | PIN_LCD_CTRL_E | PIN_LCD_CTRL_RS);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_LCD_CTRL, &GPIO_InitStructure);

	// Init control signals to 0
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RW, Bit_RESET);
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_E, Bit_RESET);
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_RESET);
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
    u8 i = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    gpio_init();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("GPIO Toggle TEST\r\n");
    Delay_Ms(10);

    // LCD test code?
    lcd_init();
    lcd_print_string("Hello World!");
    lcd_cursor_line(1);
    lcd_print_string("I am RISC-V");
    lcd_cursor_line(2);
    lcd_print_string("Line 2");
    lcd_cursor_line(3);
    lcd_print_string("Line 3");

    lcd_cursor_pos(0, 13);
    lcd_print_string("Test0");
    lcd_cursor_pos(1, 12);
    lcd_print_string("Test1");
    lcd_cursor_pos(2, 10);
    lcd_print_string("Test2");
    lcd_cursor_pos(3, 10);
    lcd_print_string("Test3");

    lcd_cursor_pos(3,5);

    while(1)
    {
        Delay_Ms(250);
        GPIO_WriteBit(GPIOB, GPIO_Pin_1, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, i);
    }
}











