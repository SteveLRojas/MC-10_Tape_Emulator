/*
 * cdc_serial.h
 *
 *  Created on: Feb 3, 2024
 *      Author: Steve
 */

#ifndef UART_CDC_SERIAL_H_
#define UART_CDC_SERIAL_H_

extern uint16_t read_timeout_ms;

void cdc_set_read_timeout(uint16_t time_ms);
uint16_t cdc_bytes_available();
int16_t cdc_peek();
int16_t cdc_read_byte();
uint16_t cdc_read_bytes(uint8_t* dest, uint16_t num_bytes);
uint16_t cdc_read_bytes_until(uint8_t terminator, uint8_t* dest, uint16_t num_bytes);

uint16_t cdc_bytes_available_for_write();
void cdc_write_byte(uint8_t val);
uint16_t cdc_write_string(char* str);
void cdc_write_bytes(uint8_t* src, uint16_t num_bytes);
bool cdc_task();
void cdc_flush();

#endif /* UART_CDC_SERIAL_H_ */
