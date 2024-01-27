/*
 * cdc_fifo.c
 *
 *  Created on: Jan 13, 2024
 *      Author: Steve
 */

#include "debug.h"
#include "usb_type.h"
#include "cdc_fifo.h"
#include "support.h"

__attribute__ ((aligned(4))) uint8_t fifo_receive_buf[CDC_BUF_SIZE] = {0};
uint16_t fifo_rc_count = 0;
uint16_t fifo_rc_front = 0;
uint16_t fifo_rc_back = 0;

__attribute__ ((aligned(4))) uint8_t fifo_transmit_buf[CDC_BUF_SIZE] = {0};
uint16_t fifo_tm_count = 0;
uint16_t fifo_tm_front = 0;
uint16_t fifo_tm_back = 0;

// #############################
// ########## RECEIVE ##########
// #############################
inline uint16_t fifo_rc_n_used()
{
	return fifo_rc_count;
}

inline uint16_t fifo_rc_n_free()
{
	return (CDC_BUF_SIZE - fifo_rc_count);
}

inline bool fifo_rc_empty()
{
	return fifo_rc_count == 0;
}

inline bool fifo_rc_full()
{
	return fifo_rc_count >= CDC_BUF_SIZE;
}

bool fifo_rc_push(uint8_t data)
{
	if(fifo_rc_full())
	{
		return FALSE; // Fifo full
	}

	fifo_receive_buf[fifo_rc_back] = data;
	++fifo_rc_back;
	fifo_rc_back &= (CDC_BUF_SIZE - 1); // Handle wrap-around
	++fifo_rc_count;

	return TRUE;
}

uint8_t fifo_rc_pop()
{
	if(fifo_rc_empty())
	{
		return 0; // Nothing in fifo
	}

	uint8_t data = fifo_receive_buf[fifo_rc_front];
	++fifo_rc_front;
	fifo_rc_front &= (CDC_BUF_SIZE - 1); // Handle wrap-around
	--fifo_rc_count;

	return data;
}

uint8_t fifo_rc_peek()
{
	if(fifo_rc_empty())
	{
		return 0; // Nothing in fifo
	}

	return fifo_receive_buf[fifo_rc_front];
}

bool fifo_rc_read(uint8_t* dest, uint16_t amt)
{
	if(amt > fifo_rc_count)
	{
		return FALSE; // Not enough data to read
	}

	// Read from fifo to dest
	fifo_rc_count -= amt;
	while(amt)
	{
		*dest = fifo_receive_buf[fifo_rc_front];
		++fifo_rc_front;
		fifo_rc_front &= (CDC_BUF_SIZE - 1); // Handle wrap-around
		++dest;
		--amt;
	}

	return TRUE;
}

// Copies 2 bytes at a time to dest
//bool fifo_rc_read_as_words(uint16_t* dest, uint16_t num_bytes)
//{
//	if(num_bytes > fifo_rc_count)
//	{
//		return FALSE; // Not enough data to read
//	}
//
//	if((fifo_rc_front & 0x01) || (num_bytes & 0x01))
//	{
//		// Use byte-copy for odd-numbered fifo index (not word aligned)
//		// or if odd-number of bytes requested
//		return fifo_rc_read((uint8_t*)dest, num_bytes);
//	}
//
//	// Byte bookkeeping
//	fifo_rc_count -= num_bytes;
//	fifo_rc_front += num_bytes;
//	fifo_rc_front &= (CDC_BUF_SIZE - 1); // Handle wrap-around
//
//	// Read from fifo to dest in terms of words
//	uint16_t* word_buf = (uint16_t*)((void*)fifo_receive_buf);
//	uint16_t word_front = fifo_rc_front >> 1;
//	uint16_t word_front_mask = (CDC_BUF_SIZE - 1) >> 1;
//
//	while(num_bytes)
//	{
//		*dest = word_buf[word_front];
//		++word_front;
//		word_front &= word_front_mask;	//handle wrap-around
//		++dest;
//		num_bytes -= 2;
//	}
//
//	return TRUE;
//}

bool fifo_rc_write(uint8_t* src, uint16_t amt)
{
	if(amt > (CDC_BUF_SIZE - fifo_rc_count))
	{
		return FALSE; // Not enough space to write
	}

	// Write to fifo from src
	fifo_rc_count += amt;
	while(amt)
	{
		fifo_receive_buf[fifo_rc_back] = *src;
		++fifo_rc_back;
		fifo_rc_back &= (CDC_BUF_SIZE - 1); // Handle wrap-around
		++src;
		--amt;
	}

	return TRUE;
}

// The packet memory area (PMA) is accessed through a 16-bit APB bus.
// Bits 31:16 are not used, and bits 15:0 must be written in a single request (writing single bytes does not work).
void fifo_pma_to_rc(uint16_t* src, uint16_t num_bytes)
{
	//too late to check for space, overrun the buffer if its full
	if(num_bytes > (CDC_BUF_SIZE - fifo_rc_count))
		uart_print_string("RC buffer overrun!\n");

	uint16_t num_words = num_bytes >> 1;	//round down
	fifo_rc_count += num_bytes;

	register uint16_t temp;
	while(num_words)
	{
		temp = *src;
		src = src + 2;

		fifo_receive_buf[fifo_rc_back] = (uint8_t)temp;
		++fifo_rc_back;
		fifo_rc_back &= (CDC_BUF_SIZE - 1);	// Handle wrap-around

		fifo_receive_buf[fifo_rc_back] = (uint8_t)(temp >> 8);
		++fifo_rc_back;
		fifo_rc_back &= (CDC_BUF_SIZE - 1);	// Handle wrap-around

		--num_words;
	}

	//handle odd byte
	if(num_bytes & 0x01)
	{
		fifo_receive_buf[fifo_rc_back] = (uint8_t)*src;
		++fifo_rc_back;
		fifo_rc_back &= (CDC_BUF_SIZE - 1);	// Handle wrap-around
	}

	return;
}

// ##############################
// ########## TRANSMIT ##########
// ##############################
inline uint16_t fifo_tm_n_used()
{
	return fifo_tm_count;
}

inline uint16_t fifo_tm_n_free()
{
	return (CDC_BUF_SIZE - fifo_tm_count);
}

inline bool fifo_tm_empty()
{
	return fifo_tm_count == 0;
}

inline bool fifo_tm_full()
{
	return fifo_tm_count >= CDC_BUF_SIZE;
}

bool fifo_tm_push(uint8_t data)
{
	if(fifo_tm_full())
	{
		return FALSE; // Fifo full
	}

	fifo_transmit_buf[fifo_tm_back] = data;
	++fifo_tm_back;
	fifo_tm_back &= (CDC_BUF_SIZE - 1); // Handle wrap-around
	++fifo_tm_count;

	return TRUE;
}

uint8_t fifo_tm_pop()
{
	if(fifo_tm_empty())
	{
		return 0; // Nothing in fifo
	}

	uint8_t data = fifo_transmit_buf[fifo_tm_front];
	++fifo_tm_front;
	fifo_tm_front &= (CDC_BUF_SIZE - 1); // Handle wrap-around
	--fifo_tm_count;

	return data;
}

uint8_t fifo_tm_peek()
{
	if(fifo_tm_empty())
	{
		return 0; // Nothing in fifo
	}

	return fifo_transmit_buf[fifo_tm_front];
}

bool fifo_tm_read(uint8_t* dest, uint16_t amt)
{
	if(amt > fifo_tm_count)
	{
		return FALSE; // Not enough data to read
	}

	// Read from fifo to dest
	fifo_tm_count -= amt;
	while(amt)
	{
		*dest = fifo_transmit_buf[fifo_tm_front];
		++fifo_tm_front;
		fifo_tm_front &= (CDC_BUF_SIZE - 1); // Handle wrap-around
		++dest;
		--amt;
	}

	return TRUE;
}

bool fifo_tm_write(uint8_t* src, uint16_t amt)
{
	if(amt > (CDC_BUF_SIZE - fifo_tm_count))
	{
		return FALSE; // Not enough space to write
	}

	// Write to fifo from src
	fifo_tm_count += amt;
	while(amt)
	{
		fifo_transmit_buf[fifo_tm_back] = *src;
		++fifo_tm_back;
		fifo_tm_back &= (CDC_BUF_SIZE - 1); // Handle wrap-around
		++src;
		--amt;
	}

	return TRUE;
}

// The packet memory area (PMA) is accessed through a 16-bit APB bus.
// Bits 31:16 are not used, and bits 15:0 must be written in a single request (writing single bytes does not work).
bool fifo_tm_to_pma(uint16_t* dest, uint16_t num_bytes)
{
	if(num_bytes > fifo_tm_count)
	{
		return FALSE; // Not enough data to read
	}

	uint16_t num_words = (num_bytes + 1) >> 1;
	uint16_t tm_front_copy = fifo_tm_front;	//need copy so we don't mess up the FIFO if we copy an extra byte

	//FIFO bookkeeping
	fifo_tm_count -= num_bytes;
	fifo_tm_front += num_bytes;
	fifo_tm_front &= (CDC_BUF_SIZE - 1); // Handle wrap-around

	register uint16_t temp;
	while(num_words)
	{
		temp = (uint16_t)(fifo_transmit_buf[tm_front_copy]);	//low byte
		++tm_front_copy;
		tm_front_copy &= (CDC_BUF_SIZE - 1); // Handle wrap-around

		temp = temp | (((uint16_t)(fifo_transmit_buf[tm_front_copy])) << 8);	//high byte
		++tm_front_copy;
		tm_front_copy &= (CDC_BUF_SIZE - 1); // Handle wrap-around

		*dest = temp;
		dest = dest + 2;
		--num_words;
	}

	return TRUE;
}
