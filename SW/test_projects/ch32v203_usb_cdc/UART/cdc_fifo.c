/*
 * cdc_fifo.c
 *
 *  Created on: Jan 13, 2024
 *      Author: Steve
 */

#include "debug.h"
#include "usb_type.h"
#include "cdc_fifo.h"

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

// ##############################
// ########## TRANSMIT ##########
// ##############################
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
