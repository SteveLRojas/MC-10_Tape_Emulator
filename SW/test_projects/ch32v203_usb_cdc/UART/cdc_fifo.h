/*
 * cdc_fifo.h
 *
 *  Created on: Jan 13, 2024
 *      Author: Steve
 */

#ifndef UART_CDC_FIFO_H_
#define UART_CDC_FIFO_H_

#define CDC_BUF_SIZE 1024 // Must be power of 2

extern uint8_t fifo_receive_buf[CDC_BUF_SIZE];
extern uint16_t fifo_rc_count;
extern uint16_t fifo_rc_front;
extern uint16_t fifo_rc_back;

extern uint8_t fifo_transmit_buf[CDC_BUF_SIZE];
extern uint16_t fifo_tm_count;
extern uint16_t fifo_tm_front;
extern uint16_t fifo_tm_back;

inline bool fifo_rc_empty();
inline bool fifo_rc_full();
bool fifo_rc_push(uint8_t data);
uint8_t fifo_rc_pop();
uint8_t fifo_rc_peek();
bool fifo_rc_read(uint8_t* dest, uint16_t amt);
bool fifo_rc_write(uint8_t* src, uint16_t amt);

inline bool fifo_tm_empty();
inline bool fifo_tm_full();
bool fifo_tm_push(uint8_t data);
uint8_t fifo_tm_pop();
uint8_t fifo_tm_peek();
bool fifo_tm_read(uint8_t* dest, uint16_t amt);
bool fifo_tm_write(uint8_t* src, uint16_t amt);

#endif /* UART_CDC_FIFO_H_ */
