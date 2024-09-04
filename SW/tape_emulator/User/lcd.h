/*
 * lcd.h
 *
 *  Created on: Sep 4, 2024
 *      Author: Steve
 */

#ifndef _LCD_H_
#define _LCD_H_

#define PORT_LCD_BUS GPIOA  		// Data & Commands
#define PORT_LCD_CTRL GPIOB 		// LCD Control Signals

#define PIN_LCD_CTRL_RW GPIO_PIN_0	// Read(1), Write(0)
#define PIN_LCD_CTRL_E GPIO_PIN_10	// Enable(1), Disable(0)
#define PIN_LCD_CTRL_RS GPIO_PIN_11	// Bus Reg Select: Data(1), Command(0)

void lcd_pulse_enable();
void lcd_send(uint8_t data);
void lcd_clear();
void lcd_init();
void lcd_putc(char c);
void lcd_print_string(const char* str);
void lcd_cursor_home();
void lcd_cursor_line(uint8_t line);
void lcd_cursor_pos(uint8_t row, uint8_t col);
void lcd_clear_line(uint8_t line);

#endif /* USER_LCD_H_ */
