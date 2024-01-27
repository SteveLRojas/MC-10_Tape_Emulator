/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_endp.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Endpoint routines
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/ 
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_prop.h"
#include "UART.h"
#include "cdc_fifo.h"

uint8_t USBD_Endp3_Busy;
uint16_t USB_Rx_Cnt=0; 

uint8_t led_state = Bit_RESET;

/*********************************************************************
 * @fn      EP2_IN_Callback
 *
 * @brief  Endpoint 1 IN.
 *
 * @return  none
 */
void EP1_IN_Callback (void)
{ 
	
}



/*********************************************************************
 * @fn      EP2_OUT_Callback
 *
 * @brief  Endpoint 2 OUT.
 *
 * @return  none
 */
void EP2_OUT_Callback (void)
{ 
	uint16_t num_bytes;
    num_bytes = GetEPRxCount( EP2_OUT & 0x7F );
//    PMAToUserBufferCopy( &UART2_Tx_Buf[ ( Uart.Tx_LoadNum * DEF_USB_FS_PACK_LEN ) ], GetEPRxAddr( EP2_OUT & 0x7F ), num_bytes );

    uint16_t wPMABufAddr = GetEPRxAddr( EP2_OUT & 0x7F );
//    uint32_t*  pdwVal = (uint32_t *)(wPMABufAddr * 2 + PMAAddr);
//
//	uint32_t i;
//	for (i = num_bytes; i > 1; i = i - 2)
//	{
//		fifo_rc_push(*pdwVal & 0xFF);
//		fifo_rc_push((*pdwVal >> 8) & 0xFF);
//		++pdwVal;
//	}
//	if(i)
//	{
//		fifo_rc_push(*pdwVal & 0xFF);
//	}

    uint16_t*  pdwVal = (uint16_t*)(wPMABufAddr * 2 + PMAAddr);

    fifo_pma_to_rc(pdwVal, num_bytes);

    Uart.Tx_PackLen[ Uart.Tx_LoadNum ] = num_bytes;
    Uart.Tx_LoadNum++;
    if( Uart.Tx_LoadNum >= DEF_UARTx_TX_BUF_NUM_MAX )
    {
        Uart.Tx_LoadNum = 0x00;
    }
    Uart.Tx_RemainNum++;

	if( Uart.Tx_RemainNum >= ( DEF_UARTx_TX_BUF_NUM_MAX - 2 ) )	//TODO: fix this
    {
        Uart.USB_Down_StopFlag = 0x01;
    }
    else
    {
        SetEPRxValid( ENDP2 );
    }
}
/*********************************************************************
 * @fn      EP3_IN_Callback
 *
 * @brief  Endpoint 3 IN.
 *
 * @return  none
 */
void EP3_IN_Callback (void)
{ 
	USBD_Endp3_Busy = 0;
	Uart.USB_Up_IngFlag = 0x00;
}

/*********************************************************************
 * @fn      USBD_ENDPx_DataUp
 *
 * @brief  USBD ENDPx DataUp Function
 * 
 * @param   endp - endpoint num.
 *          len - data length to transmit.
 * 
 * @return  data up status.
 */
uint8_t USBD_ENDPx_DataUp( uint8_t endp, uint16_t len )
{
	if( endp == ENDP3 )
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_1, led_state);
		led_state = ~led_state;

		if (USBD_Endp3_Busy)
		{
			return USB_ERROR;
		}
		USB_SIL_Write( EP3_IN, len );
		USBD_Endp3_Busy = 1;
		SetEPTxStatus( ENDP3, EP_TX_VALID );
	}
	else
	{
		return USB_ERROR;
	}
	return USB_SUCCESS;
}
