/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_sil.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Simplified Interface Layer for Global Initialization and 
 *			           Endpoint  Rea/Write operations.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/ 
#include "usb_lib.h"
#include "support.h"
#include "cdc_fifo.h"

/*******************************************************************************
 * @fn         USB_SIL_Init
 *
 * @brief      Initialize the USB Device IP and the Endpoint 0.
 *
 * @return   Status.
 */
uint32_t USB_SIL_Init(void)
{
  _SetISTR(0);
  wInterrupt_Mask = IMR_MSK;
  _SetCNTR(wInterrupt_Mask);
	
  return 0;
}

/*******************************************************************************
 * @fn			USB_SIL_Write
 *
 * @brief		Write a buffer of data to a selected endpoint.
 *
 * @param		bEpAddr: The address of the non control endpoint.
 *				num_bytes: Number of data to be written (in bytes).
 */
void USB_SIL_Write(uint8_t bEpAddr, uint16_t num_bytes)
{
	//original stuff:
	//	UserToPMABufferCopy(pBufferPointer, GetEPTxAddr(bEpAddr & 0x7F), wBufferSize);
	uint16_t wPMABufAddr = GetEPTxAddr(bEpAddr & 0x7F);
	uint16_t *pdwVal = (uint16_t *)(wPMABufAddr * 2 + PMAAddr);

	fifo_tm_to_pma(pdwVal, num_bytes);

	SetEPTxCount((bEpAddr & 0x7F), num_bytes);
}

/*******************************************************************************
 * @fn       USB_SIL_Read
 *
 * @brief     Write a buffer of data to a selected endpoint.
 *
 * @param    bEpAddr: The address of the non control endpoint.
 *                  pBufferPointer: The pointer to which will be saved the 
 *             received data buffer.
 *
 * @return     Number of received data (in Bytes).
 */
//uint32_t USB_SIL_Read(uint8_t bEpAddr, uint8_t* pBufferPointer)
//{
//	uint32_t DataLength = 0;
//
//	DataLength = GetEPRxCount(bEpAddr & 0x7F);
//	PMAToUserBufferCopy(pBufferPointer, GetEPRxAddr(bEpAddr & 0x7F), DataLength);
//
//	return DataLength;
//}






