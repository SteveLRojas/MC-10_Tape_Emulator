/*
 * cdc_serial.h
 *
 *  Created on: Feb 3, 2024
 *      Author: Steve
 */

#ifndef UART_CDC_SERIAL_H_
#define UART_CDC_SERIAL_H_

#define DEF_USB_FS_PACK_LEN			64			/* USB full speed mode packet size for serial x data */
#define DEF_UARTx_BAUDRATE			115200		/* Default baud rate for serial port */
#define DEF_UARTx_STOPBIT			0			/* Default stop bit for serial port */
#define DEF_UARTx_PARITY			0			/* Default parity bit for serial port */
#define DEF_UARTx_DATABIT			8			/* Default data bit for serial port */
#define DEF_UARTx_RX_TIMEOUT		30			/* Serial port receive timeout, in 100uS TODO: fix this*/
#define DEF_UARTx_USB_UP_TIMEOUT	6000		/* Serial port receive upload timeout, in 1000uS */

#define CDC_TOUCH_TIMEOUT_MS 100
#define CDC_FLUSH_TIMEOUT_MS 1000

extern uint16_t read_timeout_ms;
extern uint8_t  USB_Up_Pack0_Flag;				/* Serial xUSB data needs to upload 0-length packet flag */

extern volatile uint8_t  USB_Up_IngFlag;		/* Serial xUSB packet being uploaded flag */
extern volatile uint8_t  USB_Down_StopFlag;		/* Serial xUSB packet stop down flag */
extern volatile uint8_t  Com_Cfg[ 8 ];			/* Serial x parameter configuration (default baud rate is 115200, 1 stop bit, no parity, 8 data bits) */

uint16_t volatile cdc_read_timer_ms;				// timer for user configurable timeout for cdc_read funcs
uint16_t volatile cdc_up_force_finish_timer_ms;		// force USB upload complete if we don't get success callback when timeout
uint16_t volatile cdc_touch_timer_ms;				// when this reaches its limit, any call to cdc library flushes the tx buffer
uint16_t volatile cdc_flush_timer_ms;				// when this reaches its limit, the tx buffer is immediately flushed

// Setup/Init
extern void UART2_ParaInit( uint8_t mode );		/* Serial port parameter initialization */


// Receive
void cdc_set_read_timeout(uint16_t time_ms);
uint16_t cdc_bytes_available();
int16_t cdc_peek();
int16_t cdc_read_byte();
uint16_t cdc_read_bytes(uint8_t* dest, uint16_t num_bytes);
uint16_t cdc_read_bytes_until(uint8_t terminator, uint8_t* dest, uint16_t num_bytes);

// Rx helpers
void cdc_check_down_start();

// Send
uint16_t cdc_bytes_available_for_write();
void cdc_write_byte(uint8_t val);
uint16_t cdc_write_string(char* str);
void cdc_write_bytes(uint8_t* src, uint16_t num_bytes);
uint8_t cdc_task();
void cdc_flush();

#endif /* UART_CDC_SERIAL_H_ */
