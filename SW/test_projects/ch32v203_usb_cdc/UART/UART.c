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

/* The following are serial port transmit and receive related variables and buffers */
volatile UART_CTL Uart;

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
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
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
    //uint8_t i;

    //Uart.Rx_LoadPtr = 0x00;
    //Uart.Rx_DealPtr = 0x00;
    //Uart.Rx_RemainLen = 0x00;
    Uart.Rx_TimeOut = 0x00;
    Uart.Rx_TimeOutMax = 30;

    //Uart.Tx_LoadNum = 0x00;
    //Uart.Tx_DealNum = 0x00;
    //Uart.Tx_RemainNum = 0x00;
    //for( i = 0; i < DEF_UARTx_TX_BUF_NUM_MAX; i++ )
    //{
    //    Uart.Tx_PackLen[ i ] = 0x00;
    //}
    Uart.Tx_Flag = 0x00;
    //Uart.Tx_CurPackLen = 0x00;
    //Uart.Tx_CurPackPtr = 0x00;

    Uart.USB_Up_IngFlag = 0x00;
    Uart.USB_Up_TimeOut = 0x00;
    Uart.USB_Up_Pack0_Flag = 0x00;
    Uart.USB_Down_StopFlag = 0x00;

    if( mode )
    {
        Uart.Com_Cfg[ 0 ] = (uint8_t)( DEF_UARTx_BAUDRATE );
        Uart.Com_Cfg[ 1 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 8 );
        Uart.Com_Cfg[ 2 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 16 );
        Uart.Com_Cfg[ 3 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 24 );
        Uart.Com_Cfg[ 4 ] = DEF_UARTx_STOPBIT;
        Uart.Com_Cfg[ 5 ] = DEF_UARTx_PARITY;
        Uart.Com_Cfg[ 6 ] = DEF_UARTx_DATABIT;
        Uart.Com_Cfg[ 7 ] = DEF_UARTx_RX_TIMEOUT;
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
    if( Uart.Tx_Flag )
    {
            Uart.Tx_Flag = 0x00;

            NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
            NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );

            //Uart.Tx_CurPackPtr += Uart.Tx_CurPackLen;
            //Uart.Tx_CurPackLen = 0;

                //Uart.Tx_PackLen[ Uart.Tx_DealNum ] = 0x0000;
                //Uart.Tx_DealNum++;
                //if( Uart.Tx_DealNum >= DEF_UARTx_TX_BUF_NUM_MAX )
                //{
                //    Uart.Tx_DealNum = 0x00;
                //}
                //Uart.Tx_RemainNum--;

            /* If the current serial port has suspended the downlink, restart the driver downlink */
//            if( ( Uart.USB_Down_StopFlag == 0x01 ) && ( Uart.Tx_RemainNum < 2 ) )
//            {
//                SetEPRxValid( ENDP2 );
//                Uart.USB_Down_StopFlag = 0x00;
//            }
			if ((Uart.USB_Down_StopFlag == 0x01) && (fifo_rc_n_used() < (DEF_USB_FS_PACK_LEN * 2)))
			{
				SetEPRxValid( ENDP2);
				Uart.USB_Down_StopFlag = 0x00;
			}

            NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
            NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
        //}
    }
    else
    {
        /* Load data from the serial port send buffer to send  */
        //if( Uart.Tx_RemainNum )
        //{
            /* Determine whether to load from the last unsent buffer or from a new buffer */
            //if( Uart.Tx_CurPackLen == 0x00 )
            //{
            //    Uart.Tx_CurPackLen = Uart.Tx_PackLen[ Uart.Tx_DealNum ];
            //    Uart.Tx_CurPackPtr = ( Uart.Tx_DealNum * DEF_USB_FS_PACK_LEN );
            //}

            while(!fifo_rc_empty() && !fifo_tm_full())
            {
            	uint8_t data = fifo_rc_pop();
            	print_hex_byte(data);
            	fifo_tm_push(data);
            }
            //Uart.Rx_RemainLen = Uart.Tx_CurPackLen;

            Uart.Tx_Flag = 0x01;
        //}
    }
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

    /*****************************************************************/
    /* Serial port 1 data processing via USB upload and reception */
//    if( Uart.Rx_RemainLen )
    if (!fifo_tm_empty())
    {
        if( Uart.USB_Up_IngFlag == 0 )
        {
            /* Calculate the length of this upload */
            remain_len = fifo_tm_n_used();
            packlen = 0x00;
            if( remain_len >= DEF_USBD_MAX_PACK_SIZE )
            {
                packlen = DEF_USBD_MAX_PACK_SIZE;
            }
            else
            {
                if( Uart.Rx_TimeOut >= Uart.Rx_TimeOutMax )
                {
                    packlen = remain_len;
                }
            }
            //if( packlen > ( DEF_UARTx_RX_BUF_LEN - Uart.Rx_DealPtr ) )	//Prevent wrapping? TODO: get rid of this
            //{
            //    packlen = ( DEF_UARTx_RX_BUF_LEN - Uart.Rx_DealPtr );
            //}

            /* Upload serial data via usb */
            if( packlen )
            {
                NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
                Uart.USB_Up_IngFlag = 0x01;
                Uart.USB_Up_TimeOut = 0x00;

                USBD_ENDPx_DataUp(ENDP3, packlen);

                /* Calculate the variables of interest */
                //Uart.Rx_RemainLen -= packlen;
                //Uart.Rx_DealPtr += packlen;
                //if( Uart.Rx_DealPtr >= DEF_UARTx_RX_BUF_LEN )
                //{
                //    Uart.Rx_DealPtr = 0x00;
                //

                /* Start 0-length packet timeout timer */
                if( packlen == DEF_UARTx_RX_BUF_LEN )
                {
                	// TODO: ^ Change to == DEF_USBD_MAX_PACK_SIZE?
                	// http://tinyurl.com/2c436fkh
                    Uart.USB_Up_Pack0_Flag = 0x01;
                }

                NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
            }
        }
        else
        {
            /* Set the upload success flag directly if the upload is not successful after the timeout */
            if( Uart.USB_Up_TimeOut >= DEF_UARTx_USB_UP_TIMEOUT )
            {
                Uart.USB_Up_IngFlag = 0x00;
                USBD_Endp3_Busy = 0;
            }
        }
    }

    /*****************************************************************/
    /* Determine if a 0-length packet needs to be uploaded (required for CDC mode) */
    if( Uart.USB_Up_Pack0_Flag )
    {
        if( Uart.USB_Up_IngFlag == 0 )
        {
            if( Uart.USB_Up_TimeOut >= ( DEF_UARTx_RX_TIMEOUT * 20 ) )
            {
                NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_DisableIRQ( USB_HP_CAN1_TX_IRQn );
                Uart.USB_Up_IngFlag = 0x01;
                Uart.USB_Up_TimeOut = 0x00;

                USBD_ENDPx_DataUp( ENDP3, 0);

                Uart.USB_Up_IngFlag = 0;
                Uart.USB_Up_Pack0_Flag = 0x00;
                NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
                NVIC_EnableIRQ( USB_HP_CAN1_TX_IRQn );
            }
        }
    }
}
