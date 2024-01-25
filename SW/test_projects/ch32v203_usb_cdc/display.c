/*
 * display.c
 *
 *  Created on: Nov 25, 2023
 *      Author: Steve
 */

#include "debug.h"
#include "display.h"

void lcd_pulse_enable()
{
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_E, Bit_SET);
	Delay_Us(8);
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_E, Bit_RESET);
	return;
}

void lcd_send(uint8_t data)
{
	uint16_t gpio_a_val = GPIO_ReadInputData(PORT_LCD_BUS);
	gpio_a_val = ((gpio_a_val & 0xFF00) | data);
	GPIO_Write(PORT_LCD_BUS, gpio_a_val);

	lcd_pulse_enable(); //TODO:?
	return;
}

void lcd_clear()
{
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_RESET); // Bus Command
	// 0 0 0 0 0 0 0 1
	// "Display Clear" Command
	lcd_send(0x01);
	Delay_Ms(10);
	return;
}

void lcd_init()
{
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_RESET); // Bus Command
	// 0 0 1 DL N F X X
	// 001   : "Function Set" Command
	// DL = 1: 8-Bit interface (DL = 0: 4-bit)
	// N  = 1: 2 line disp (N = 0: 1 line)
	// F  = 0: 5x7 dots (F = 1: 5x10)
	lcd_send(0x38);
	Delay_Ms(10);

	// 0 0 0 0 1 D C B
	// 00001: "Display On/Off" Command
	// D = 1: Display On (D = 0: Off)
	// C = 1: Cursor On (C = 0: Off)
	// B = 1: Cursor Blink (B = 0: Off)
	lcd_send(0x0F);
	Delay_Ms(10);

	lcd_clear();
	return;
}

void lcd_putc(char c)
{
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_SET); // Bus Data
	lcd_send(c);
	Delay_Ms(2);
	return;
}

void lcd_print_string(const char* str)
{
    unsigned char i = 0;
    while(str[i])
    {
        lcd_putc(str[i]);
        ++i;
    }
    return;
}

void lcd_cursor_home()
{
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_RESET); // Bus Command
	// 0 0 0 0 0 0 1 X
	// "Return Home" Command
	lcd_send(0x03);
	Delay_Ms(10);
	return;
}

void lcd_cursor_line(uint8_t line)
{
	lcd_cursor_home(); // Start from known position

	uint8_t inc_amt = 0;
	switch(line)
	{
		case 0:
			return;
		case 1:
			inc_amt = 0x28;
			break;
		case 2:
			inc_amt = 0x14;
			break;
		case 3:
			inc_amt = 0x3C;
			break;
		default:
			return;
	}

	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_RESET); // Bus Command
	for(uint8_t i = 0; i < inc_amt; i++)
	{
		// 0 0 0 1 D/C R/L 0 0
		// "Move Cursor" Command
		// D/C = 0: Cursor (1: Disp)
		// R/L = 1: Right (0: Left)
		lcd_send(0x14);
		Delay_Ms(2);
	}
	return;
}

void lcd_cursor_pos(uint8_t row, uint8_t col)
{
	uint8_t addr = 0;
	switch(row)
	{
		case 0:
			break;
		case 1:
			addr = 0x40;
			break;
		case 2:
			addr = 0x14;
			break;
		case 3:
			addr = 0x54;
			break;
		default:
			break;
	}

	addr += col;
	addr |= 0x80; // "Set Display Data RAM Address"

	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_RESET); // Bus Command
	lcd_send(addr);
	Delay_Ms(2);

	return;
}

void lcd_clear_line(uint8_t line)
{
	lcd_cursor_line(line);

	for(uint8_t i = 0; i < 20; i++)
	{
		lcd_putc(' ');
	}

	lcd_cursor_line(line);
	return;
}






