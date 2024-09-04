/*
 * display.h
 *
 *  Created on: Nov 25, 2023
 *      Author: Steve
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define PORT_LCD_BUS GPIOA  		// Data & Commands
#define PORT_LCD_CTRL GPIOB 		// LCD Control Signals

#define PIN_LCD_CTRL_RW GPIO_Pin_0	// Read(1), Write(0)
#define PIN_LCD_CTRL_E GPIO_Pin_10	// Enable(1), Disable(0)
#define PIN_LCD_CTRL_RS GPIO_Pin_11	// Bus Reg Select: Data(1), Command(0)

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

#endif /* DISPLAY_H_ */
