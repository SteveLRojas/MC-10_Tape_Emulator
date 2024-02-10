/*
 * cdc_serial.c
 *
 *  Created on: Feb 3, 2024
 *      Author: Steve
 */

#include "usb_type.h"
#include "cdc_serial.h"
#include "cdc_fifo.h"
#include "UART.h"	//TODO: make it nice
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_prop.h"

extern uint8_t USBD_Endp3_Busy;

uint16_t read_timeout_ms = 0;

//TODO: Implement timeout stuff, all functions flush when short timeout expires, re-enable down link

// #############################
// ########## RECEIVE ##########
// #############################
void cdc_set_read_timeout(uint16_t time_ms)
{
	read_timeout_ms = time_ms;
}

uint16_t cdc_bytes_available()
{
	return fifo_rc_num_used();
}

int16_t cdc_peek()
{
	return fifo_rc_peek();
}

int16_t cdc_read_byte()
{
	if(!fifo_rc_empty())
	{
		return fifo_rc_pop();
	}

	return -1;
}

uint16_t cdc_read_bytes(uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_remaining = num_bytes;
	uint16_t num_to_read = 0;
	while(num_remaining) // && !timeout
	{
		num_to_read = fifo_rc_num_used();
		if (num_to_read > num_remaining)
		{
			num_to_read = num_remaining;
		}

		fifo_rc_read(dest, num_to_read);
		num_remaining -= num_to_read;
		dest += num_to_read;
	}

	return (num_bytes - num_remaining);
}

uint16_t cdc_read_bytes_until(uint8_t terminator, uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_remaining = num_bytes;
	uint8_t popped;
	while(num_remaining) // && !timeout
	{
		if(fifo_rc_empty())
			continue;

		popped = fifo_rc_pop();
		if(popped == terminator)
			break;

		--num_remaining;
		*dest++ = popped;
	}

	return (num_bytes - num_remaining);
}

// ##############################
// ########## TRANSMIT ##########
// ##############################
uint16_t cdc_bytes_available_for_write()
{
	return fifo_tm_num_free();
}

void cdc_write_byte(uint8_t val)
{
	if(fifo_tm_full())
	{
		cdc_flush();
	}

	fifo_tm_push(val);
}

uint16_t cdc_write_string(char* str)
{
	uint16_t len = strlen(str);
	cdc_write_bytes(str, len);
	return len;
}

void cdc_write_bytes(uint8_t* src, uint16_t num_bytes)
{
	uint16_t num_remaining = num_bytes;
	uint16_t num_to_write = 0;
	while(num_remaining) // && !timeout
	{
		num_to_write = fifo_tm_num_free();
		if (num_to_write > num_remaining)
		{
			num_to_write = num_remaining;
		}

		fifo_tm_write(src, num_to_write);
		num_remaining -= num_to_write;
		src += num_to_write;

		// Flush if full
		if(fifo_tm_full())
		{
			cdc_flush();
		}
	}
}


bool cdc_task()
{
	// if not timed-out
	return FALSE;
}

void cdc_flush()
{
	uint16_t num_to_write = 0;

    while (!fifo_tm_empty())
    {
        if( USB_Up_IngFlag == 0 )
        {
            /* Calculate the length of this upload */
        	num_to_write = fifo_tm_num_used();
            if( num_to_write >= DEF_USBD_MAX_PACK_SIZE )
            {
            	num_to_write = DEF_USBD_MAX_PACK_SIZE;
            }

            /* Upload serial data via usb */
            if( num_to_write )
            {
                NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
                USB_Up_IngFlag = 0x01;
                USB_Up_TimeOut = 0x00;

                USBD_ENDPx_DataUp(ENDP3, num_to_write);

                /* Start 0-length packet timeout timer */
                if( num_to_write == DEF_UARTx_RX_BUF_LEN )
                {
                	// TODO: ^ Change to == DEF_USBD_MAX_PACK_SIZE?
                	// http://tinyurl.com/2c436fkh
                    USB_Up_Pack0_Flag = 0x01;
                }

                NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
            }
        }
        else
        {
            /* Set the upload success flag directly if the upload is not successful after the timeout */
            if( USB_Up_TimeOut >= DEF_UARTx_USB_UP_TIMEOUT )
            {
                USB_Up_IngFlag = 0x00;
                USBD_Endp3_Busy = 0;
            }
        }
    }

    /*****************************************************************/
    /* Determine if a 0-length packet needs to be uploaded (required for CDC mode) */
    if( USB_Up_Pack0_Flag )
    {
        if( USB_Up_IngFlag == 0 )
        {
            if( USB_Up_TimeOut >= ( DEF_UARTx_RX_TIMEOUT * 20 ) )
            {
                NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
                USB_Up_IngFlag = 0x01;
                USB_Up_TimeOut = 0x00;

                USBD_ENDPx_DataUp( ENDP3, 0);

                USB_Up_IngFlag = 0;
                USB_Up_Pack0_Flag = 0x00;
                NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
            }
        }
    }
}
