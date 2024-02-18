/*
 * cdc_serial.c
 *
 *  Created on: Feb 3, 2024
 *      Author: Steve
 */

#include "usb_type.h"
#include "cdc_serial.h"
#include "cdc_fifo.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_prop.h"
#include "string.h"

extern uint8_t USBD_Endp3_Busy;
uint8_t volatile Com_Cfg[ 8 ];
uint8_t volatile USB_Down_StopFlag;                 /* Serial xUSB packet stop down flag */
uint8_t volatile USB_Up_IngFlag;                    /* Serial xUSB packet being uploaded flag */

uint16_t volatile cdc_read_timer_ms;				// timer for user configurable timeout for cdc_read funcs
uint16_t volatile cdc_up_force_finish_timer_ms;		// force USB upload complete if we don't get success callback when timeout
uint16_t volatile cdc_touch_timer_ms;				// when this reaches its limit, any call to cdc library flushes the tx buffer
uint16_t volatile cdc_flush_timer_ms;				// when this reaches its limit, the tx buffer is immediately flushed

uint16_t read_timeout_ms = 3; // 3ms
volatile uint8_t USB_Up_Pack0_Flag;                          /* Serial xUSB data needs to upload 0-length packet flag */


/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   1000us Timer
 *
 * @return  none
 */
void TIM2_Init( void )	//still used
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};

    TIM_DeInit( TIM2 );

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;	//event every 1000 us
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;	//get 1MHz clock for a 1 us time scale
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//use internal clock source
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//count up to ATRLR, reload with 0
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );

    /* Clear TIM2 update pending flag */
    TIM_ClearFlag( TIM2, TIM_FLAG_Update );

    /* TIM IT enable */
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );

    /* Enable Interrupt */
    NVIC_EnableIRQ( TIM2_IRQn );

    /* TIM2 enable counter */
    TIM_Cmd( TIM2, ENABLE );
}

void cdc_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE );

	TIM2_Init( );
	UART2_ParaInit( 1 );

	Set_USBConfig();
	USB_Init();
	USB_Interrupts_Config();
}

// #############################
// ######## SETUP/INIT #########
// #############################
/*********************************************************************
 * @fn      UART2_ParaInit
 *
 * @brief   Uart2 parameters initialization
 *          mode = 0 : Used in usb modify initialization
 *          mode = 1 : Used in default initializations
 * @return  none
 */
void UART2_ParaInit( uint8_t mode )	//still used
{
	cdc_read_timer_ms = 0;
	cdc_up_force_finish_timer_ms = 0;
	cdc_touch_timer_ms = 0;
	cdc_flush_timer_ms = 0;

    USB_Up_IngFlag = 0x00;
    USB_Up_Pack0_Flag = 0x00;
    USB_Down_StopFlag = 0x00;

    if( mode )
    {
        Com_Cfg[ 0 ] = (uint8_t)( DEF_UARTx_BAUDRATE );
        Com_Cfg[ 1 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 8 );
        Com_Cfg[ 2 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 16 );
        Com_Cfg[ 3 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 24 );
        Com_Cfg[ 4 ] = DEF_UARTx_STOPBIT;
        Com_Cfg[ 5 ] = DEF_UARTx_PARITY;
        Com_Cfg[ 6 ] = DEF_UARTx_DATABIT;
        Com_Cfg[ 7 ] = DEF_UARTx_RX_TIMEOUT;
    }
}


// #############################
// ########## RECEIVE ##########
// #############################
void cdc_set_read_timeout(uint16_t time_ms)
{
	read_timeout_ms = time_ms;
}

inline uint16_t cdc_bytes_available()
{
	return fifo_rc_num_used();
}

inline int16_t cdc_peek()
{
	if(fifo_rc_empty())
		return -1;

	return fifo_rc_peek();
}

int16_t cdc_read_byte()
{
	int16_t popped = -1;

	cdc_read_timer_ms = 0;
	while(NOT_TIMEDOUT(cdc_read_timer_ms, read_timeout_ms))
	{
		if(!fifo_rc_empty())
		{
            NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
            NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
			popped = fifo_rc_pop();
            NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
            NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );

			cdc_check_down_start();
			return popped;
		}
	}

	return popped;
}

uint16_t cdc_read_bytes(uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_remaining = num_bytes;
	uint16_t num_to_read = 0;

	cdc_read_timer_ms = 0;
	while(num_remaining && NOT_TIMEDOUT(cdc_read_timer_ms, read_timeout_ms))
	{
		num_to_read = fifo_rc_num_used();
		if (num_to_read > num_remaining)
		{
			num_to_read = num_remaining;
		}

        NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
        NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
		fifo_rc_read(dest, num_to_read);
        NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
        NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );

		num_remaining -= num_to_read;
		dest += num_to_read;
	}

	cdc_check_down_start();
	return (num_bytes - num_remaining);
}

uint16_t cdc_read_bytes_until(uint8_t terminator, uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_remaining = num_bytes;
	uint8_t popped;

	cdc_read_timer_ms = 0;
	while(num_remaining && NOT_TIMEDOUT(cdc_read_timer_ms, read_timeout_ms))
	{
		if(fifo_rc_empty())
			continue;

        NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
        NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
		popped = fifo_rc_pop();
        NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
        NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );

		if(popped == terminator)
			break;

		--num_remaining;
		*dest++ = popped;
	}

	cdc_check_down_start();
	return (num_bytes - num_remaining);
}

inline void cdc_check_down_start()
{
	if ((USB_Down_StopFlag == 0x01) && (fifo_rc_num_used() < (DEF_USB_FS_PACK_LEN * 2)))
	{
		NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
		NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );

		SetEPRxValid(ENDP2);
		USB_Down_StopFlag = 0x00;

		NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
		NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
	}
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
	//bool touch_timeout = IS_TIMEDOUT(cdc_touch_timer_ms, CDC_TOUCH_TIMEOUT_MS);

	if(fifo_tm_full())
	{
		cdc_flush();
		//touch_timeout = FALSE; // Don't want to flush twice
	}

	cdc_touch_timer_ms = 0;
	cdc_flush_timer_ms = 0;
	fifo_tm_push(val);

	//if(touch_timeout)
	//{
	//	cdc_flush();
	//}
}

uint16_t cdc_write_string(char* str)
{
	uint16_t len = strlen(str);
	cdc_write_bytes(str, len);
	return len;
}

void cdc_write_bytes(uint8_t* src, uint16_t num_bytes)
{
	//bool touch_timeout = IS_TIMEDOUT(cdc_touch_timer_ms, CDC_TOUCH_TIMEOUT_MS);

	uint16_t num_remaining = num_bytes;
	uint16_t num_to_write = 0;

	while(num_remaining)
	{
		num_to_write = fifo_tm_num_free();
		if (num_to_write > num_remaining)
		{
			num_to_write = num_remaining;
		}

		cdc_touch_timer_ms = 0;
		cdc_flush_timer_ms = 0;
		fifo_tm_write(src, num_to_write);
		num_remaining -= num_to_write;
		src += num_to_write;

		// Flush if full
		if(fifo_tm_full())
		{
			cdc_flush();
			//touch_timeout = FALSE; // Don't want to flush twice
		}
	}

	//if(touch_timeout)
	//{
	//	cdc_flush();
	//}
}

void cdc_task()
{
	if(IS_TIMEDOUT(cdc_touch_timer_ms, CDC_TOUCH_TIMEOUT_MS))
	{
		cdc_flush();
		cdc_touch_timer_ms = 0;
	}
	return;
}

void cdc_flush()
{
	uint16_t num_to_write = 0;
	cdc_flush_timer_ms = 0;	//prevent re-entry due to timer interrupt

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
                cdc_up_force_finish_timer_ms = 0;

                USBD_ENDPx_DataUp(ENDP3, num_to_write);

                /* Start 0-length packet timeout timer */
                /*if( num_to_write == DEF_UARTx_RX_BUF_LEN )
                {
                	// TODO: ^ Change to == DEF_USBD_MAX_PACK_SIZE?
                	// http://tinyurl.com/2c436fkh
                    USB_Up_Pack0_Flag = 0x01;
                }*/

                NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
            }
        }
        else
        {
            /* Set the upload success flag directly if the upload is not successful after the timeout */
            if(cdc_up_force_finish_timer_ms >= DEF_UARTx_USB_UP_TIMEOUT)
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
            if( cdc_up_force_finish_timer_ms >= ( DEF_UARTx_RX_TIMEOUT * 20 ) )	//TODO: fix this (3 * 20)
            {
                NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
                USB_Up_IngFlag = 0x01;
                cdc_up_force_finish_timer_ms = 0;

                USBD_ENDPx_DataUp( ENDP3, 0);

                USB_Up_IngFlag = 0;
                USB_Up_Pack0_Flag = 0x00;
                NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
            }
        }
    }
}
