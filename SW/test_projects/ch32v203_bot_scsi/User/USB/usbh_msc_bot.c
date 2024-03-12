/*
 * usb_msc_bot.c
 *
 *  Created on: Mar 9, 2024
 *      Author: Steve
 */

#include "usbh_msc_bot.h"
#include "ch32v20x_usbfs_host.h"
#include "usbh_msc.h"
#include "usb_host_config.h"
#include "debug.h"
#include "Udisk_Operation.h"

/** @defgroup USBH_MSC_BOT_Private_Functions
  * @{
  */

/**
  * @brief  USBH_MSC_BOT_REQ_Reset
  *         The function the MSC BOT Reset request.
  * @param  phost: Host handle
  * @retval USBH Status
  */
//USBH_StatusTypeDef USBH_MSC_BOT_REQ_Reset(USBH_HandleTypeDef *phost)
uint8_t USBH_MSC_BOT_REQ_Reset()
{
//  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS
//                                         | USB_REQ_RECIPIENT_INTERFACE;
//
//  phost->Control.setup.b.bRequest = USB_REQ_BOT_RESET;
//  phost->Control.setup.b.wValue.w = 0U;
//  phost->Control.setup.b.wIndex.w = 0U;
//  phost->Control.setup.b.wLength.w = 0U;
//
//  return USBH_CtlReq(phost, NULL, 0U);

	pUSBFS_SetupRequest->bRequest = USB_REQ_BOT_RESET;
	pUSBFS_SetupRequest->wValue = 0U;
	pUSBFS_SetupRequest->wIndex = 0U;
	pUSBFS_SetupRequest->wLength = 0U;

	return USBFSH_CtrlTransfer(0, NULL, NULL);
}

/**
  * @brief  USBH_MSC_BOT_REQ_GetMaxLUN
  *         The function the MSC BOT GetMaxLUN request.
  * @param  phost: Host handle
  * @param  Maxlun: pointer to Maxlun variable
  * @retval USBH Status
  */
uint8_t USBH_MSC_BOT_REQ_GetMaxLUN(uint8_t* Maxlun)
{
//	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS
//										 | USB_REQ_RECIPIENT_INTERFACE;
//
//	phost->Control.setup.b.bRequest = USB_REQ_GET_MAX_LUN;
//	phost->Control.setup.b.wValue.w = 0U;
//	phost->Control.setup.b.wIndex.w = 0U;
//	phost->Control.setup.b.wLength.w = 1U;

	pUSBFS_SetupRequest->bRequest = USB_REQ_GET_MAX_LUN;
	pUSBFS_SetupRequest->wValue = 0U;
	pUSBFS_SetupRequest->wIndex = 0U;
	pUSBFS_SetupRequest->wLength = 1U;

	*Maxlun = 0;

	uint16_t transfer_len = 0;
	uint8_t ctrl_transfer_result = USBFSH_CtrlTransfer(RootHubDev[ 0 ].bEp0MaxPks, Com_Buffer, &transfer_len);
	if(ctrl_transfer_result != ERR_SUCCESS)
	{
		PRINT("Non-Success ctrl_transfer_result: 0x%02X\n", ctrl_transfer_result);
	}
	else if(transfer_len != pUSBFS_SetupRequest->wLength)
	{
		PRINT("Unexpected plen from USBFSH_CtrlTransfer... expected:[%d] actual:[%d]\n", pUSBFS_SetupRequest->wLength, transfer_len);
	}
	else
	{
		*Maxlun = Com_Buffer[0];
	}

	return ctrl_transfer_result;
}

//TODO: implement USBH_MSC_BOT_Process

