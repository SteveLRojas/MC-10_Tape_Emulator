/*
 * lcd.c
 *
 *  Created on: Sep 4, 2024
 *      Author: Steve
 */

#include "ch32v20x.h"
#include "ch32v203_gpio.h"
#include "debug.h"
#include "lcd.h"

void lcd_pulse_enable()
{
	gpio_set_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_E);
	Delay_Us(8);
	gpio_clear_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_E);
	return;
}

void lcd_send(uint8_t data)
{
	uint16_t gpio_a_val = gpio_read_port(PORT_LCD_BUS);
	gpio_a_val = ((gpio_a_val & 0xFF00) | data);
	gpio_write_port(PORT_LCD_BUS, gpio_a_val);

	lcd_pulse_enable();
	return;
}

void lcd_clear()
{
	gpio_clear_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS); // Bus Command
	// 0 0 0 0 0 0 0 1
	// "Display Clear" Command
	lcd_send(0x01);
	Delay_Ms(10);
	return;
}

void lcd_init()
{
	gpio_clear_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_RW);	// Write mode
	gpio_clear_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS); // Bus Command
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
	gpio_set_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS); // Bus Data
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
	gpio_clear_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS); // Bus Command
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

	gpio_clear_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS); // Bus Command
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
	static const uint8_t addr_tab[4] = {0x00, 0x40, 0x14, 0x54};
	uint8_t addr;

	row &= 0x03;
	addr = addr_tab[row];

	addr += col;
	addr |= 0x80; // "Set Display Data RAM Address"

	gpio_clear_pin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS); // Bus Command
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
