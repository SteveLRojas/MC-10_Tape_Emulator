/*
 * cdc_fifo.h
 *
 *  Created on: Jan 13, 2024
 *      Author: Steve
 */

#ifndef UART_CDC_FIFO_H_
#define UART_CDC_FIFO_H_

#define CDC_BUF_SIZE 1024 // Must be power of 2

// Receive
extern __attribute__ ((aligned(4))) uint8_t fifo_receive_buf[CDC_BUF_SIZE];
extern uint16_t fifo_rc_count;
extern uint16_t fifo_rc_front;
extern uint16_t fifo_rc_back;

// Transmit
extern __attribute__ ((aligned(4))) uint8_t fifo_transmit_buf[CDC_BUF_SIZE];
extern uint16_t fifo_tm_count;
extern uint16_t fifo_tm_front;
extern uint16_t fifo_tm_back;

// Receive
uint16_t fifo_rc_num_used();
uint16_t fifo_rc_num_free();
bool fifo_rc_empty();
bool fifo_rc_full();
bool fifo_rc_push(uint8_t data);
uint8_t fifo_rc_pop();
uint8_t fifo_rc_peek();
bool fifo_rc_read(uint8_t* dest, uint16_t num_bytes);
//bool fifo_rc_read_as_words(uint16_t* dest, uint16_t num_bytes);
bool fifo_rc_write(uint8_t* src, uint16_t num_bytes);
void fifo_pma_to_rc(uint16_t* src, uint16_t num_bytes);

// Transmit
uint16_t fifo_tm_num_used();
uint16_t fifo_tm_num_free();
bool fifo_tm_empty();
bool fifo_tm_full();
bool fifo_tm_push(uint8_t data);
uint8_t fifo_tm_pop();
uint8_t fifo_tm_peek();
bool fifo_tm_read(uint8_t* dest, uint16_t num_bytes);
bool fifo_tm_write(uint8_t* src, uint16_t num_bytes);
bool fifo_tm_to_pma(uint16_t* dest, uint16_t num_bytes);

#endif /* UART_CDC_FIFO_H_ */
