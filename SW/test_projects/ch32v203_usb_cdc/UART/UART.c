/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.01
* Date               : 2022/12/13
* Description        : uart serial port related initialization and processing
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "UART.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_prop.h"
#include "support.h"
#include "cdc_fifo.h"

/*******************************************************************************/
/* Variable Definition */
/* Global */
uint8_t volatile Rx_TimeOut;                                                /* Serial x data receive timeout */
uint8_t Rx_TimeOutMax;                                                      /* Serial x data receive timeout maximum */
uint8_t Tx_Flag;                                                            /* Serial x data send status */
uint8_t volatile USB_Up_IngFlag;                                            /* Serial xUSB packet being uploaded flag */
uint16_t volatile USB_Up_TimeOut;                                           /* Serial xUSB packet upload timeout timer */
uint8_t USB_Up_Pack0_Flag;                                                  /* Serial xUSB data needs to upload 0-length packet flag */
uint8_t volatile USB_Down_StopFlag;                                         /* Serial xUSB packet stop down flag */
uint8_t volatile Com_Cfg[ 8 ];

/* The following are serial port transmit and receive related variables and buffers */
__attribute__ ((aligned(4))) uint8_t  UART2_Tx_Buf[ DEF_UARTx_TX_BUF_LEN ];  /* Serial port 2 transmit data buffer */
__attribute__ ((aligned(4))) uint8_t  UART2_Rx_Buf[ DEF_UARTx_RX_BUF_LEN ];  /* Serial port 2 receive data buffer */

extern uint8_t USBD_Endp3_Busy;

/*********************************************************************
 * @fn      RCC_Configuration
 *
 * @brief   Configures the different system clocks.
 *
 * @return  none
 */
uint8_t RCC_Configuration( void )	//still used
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );
    return 0;
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   100us Timer
 *          144 * 100 * 13.8888 -----> 100uS
 *
 * @return  none
 */
void TIM2_Init( void )	//still used
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};

    TIM_DeInit( TIM2 );

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;	//event every 100 us
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
    Rx_TimeOut = 0x00;
    Rx_TimeOutMax = 30;

    Tx_Flag = 0x00;

    USB_Up_IngFlag = 0x00;
    USB_Up_TimeOut = 0x00;
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


/*********************************************************************
 * @fn      UART2_USB_Init
 *
 * @brief   Uart2 initialization in usb interrupt
 *
 * @return  none
 */
void UART2_USB_Init( void )
{
    UART2_ParaInit( 0 );
}

/*********************************************************************
 * @fn      UART2_DataTx_Deal
 *
 * @brief   Uart2 data transmission processing
 *
 * @return  none
 */
void UART2_DataTx_Deal( void )	//USB -> UART
{
    /* uart1 transmission processing */
    //if( Tx_Flag )
    //{
		Tx_Flag = 0x00;

		NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
		NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );

		if ((USB_Down_StopFlag == 0x01) && (fifo_rc_num_used() < (DEF_USB_FS_PACK_LEN * 2)))
		{
			SetEPRxValid( ENDP2);
			USB_Down_StopFlag = 0x00;
		}

		NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
		NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
    //}
    //else
    //{
		while(!fifo_rc_empty() && !fifo_tm_full())
		{
			uint8_t data = fifo_rc_pop();
			print_hex_byte(data);
			fifo_tm_push(data);
			Rx_TimeOut = 0x00;
		}

		Tx_Flag = 0x01;
    //}
}

/*********************************************************************
 * @fn      UART2_DataRx_Deal
 *
 * @brief   Uart2 data receiving processing
 *
 * @return  none
 */
void UART2_DataRx_Deal( void )	//UART -> USB
{
    uint32_t remain_len;
    uint16_t packlen;

    if (!fifo_tm_empty())
    {
        if( USB_Up_IngFlag == 0 )
        {
            /* Calculate the length of this upload */
            remain_len = fifo_tm_num_used();
            packlen = 0x00;
            if( remain_len >= DEF_USBD_MAX_PACK_SIZE )
            {
                packlen = DEF_USBD_MAX_PACK_SIZE;
            }
            else
            {
                if( Rx_TimeOut >= Rx_TimeOutMax )
                {
                    packlen = remain_len;
                }
            }

            /* Upload serial data via usb */
            if( packlen )
            {
                NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
                USB_Up_IngFlag = 0x01;
                USB_Up_TimeOut = 0x00;

                USBD_ENDPx_DataUp(ENDP3, packlen);

                /* Start 0-length packet timeout timer */
                if( packlen == DEF_UARTx_RX_BUF_LEN )
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
