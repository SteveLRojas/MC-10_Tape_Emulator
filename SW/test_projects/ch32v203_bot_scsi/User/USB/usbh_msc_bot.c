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
#include "UDisk_HW.h"

#define MSC_USB_ENDPOINT_1 0x01

//TODO: initialize this
BOT_HandleTypeDef hbot;
uint8_t endp_tog_out = 0x00;	//set to 1 for magic
uint8_t endp_tog_in = 0x00;


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

	pUSBFS_SetupRequest->bRequestType = USB_REQ_TYP_OUT | USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF;
	pUSBFS_SetupRequest->bRequest = USB_REQ_BOT_RESET;
	pUSBFS_SetupRequest->wValue = 0U;
	pUSBFS_SetupRequest->wIndex = 0U;
	pUSBFS_SetupRequest->wLength = 0U;

	return USBFSH_CtrlTransfer(0, NULL, NULL);
}

//TODO: handle STALL correctly
uint8_t USBH_MSC_BOT_REQ_GetMaxLUN(uint8_t* Maxlun)
{
	pUSBFS_SetupRequest->bRequestType = USB_REQ_TYP_IN | USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF;
	pUSBFS_SetupRequest->bRequest = USB_REQ_GET_MAX_LUN;
	pUSBFS_SetupRequest->wValue = 0U;
	pUSBFS_SetupRequest->wIndex = 0U;
	pUSBFS_SetupRequest->wLength = 1U;

	*Maxlun = 0;

	uint16_t transfer_len = 0;
	uint8_t ctrl_transfer_result = USBFSH_CtrlTransfer(RootHubDev[ 0 ].bEp0MaxPks, Com_Buffer, &transfer_len);	//TODO: no need to check length or pass in a large buffer, function checks wLength
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


USBH_StatusTypeDef USBH_MSC_BOT_Init()
{
	hbot.cbw.field.Signature = BOT_CBW_SIGNATURE;
	hbot.cbw.field.Tag = BOT_CBW_TAG;
	hbot.state = BOT_SEND_CBW;
	hbot.cmd_state = BOT_CMD_SEND;

	return USBH_OK;
}


uint8_t USBH_MSC_BOT_Command(uint8_t lun)
{
	// ----- SEND_CBW
	hbot.cbw.field.LUN = lun;
	uint8_t send_cbw_status = USBFSH_SendEndpData(MSC_USB_ENDPOINT_1, &endp_tog_out, hbot.cbw.data, BOT_CBW_LENGTH);

	if(send_cbw_status != ERR_SUCCESS)
	{
		PRINT("Non-Success send_cbw_status: 0x%02X\n", send_cbw_status);
		//return send_cbw_status;
	}
	Delay_Ms(100);

	// ----- BOT_DATA_IN OR BOT_DATA_OUT OR NONE
	// TODO: Write
	if (hbot.cbw.field.DataTransferLength != 0U)
	{
		/* If there is Data Transfer Stage */
		if (((hbot.cbw.field.Flags) & USB_EP_DIR_MSK) == USB_EP_DIR_IN)
		{
			/* Data Direction is IN */
			uint16_t data_in_rx_len = 0;
			uint8_t data_in_status = USBFSH_GetEndpData(MSC_USB_ENDPOINT_1, &endp_tog_in, hbot.pbuf, &data_in_rx_len, hbot.cbw.field.DataTransferLength);

			if(data_in_status != ERR_SUCCESS)
			{
				PRINT("Non-Success data_in_status: 0x%02X\n", data_in_status);
				//return data_in_status;
			}
			if(data_in_rx_len != hbot.cbw.field.DataTransferLength)
			{
				PRINT("Unexpected data_in_rx_len: %d\n", data_in_rx_len);
			}
		}
		else
		{
			/* Data Direction is OUT */
			// hbot.state = BOT_DATA_OUT;
		}
	}
	Delay_Ms(100);


	// ----- RECEIVE_CSW
	uint16_t get_csw_rx_len = 0;
	uint8_t get_csw_status = USBFSH_GetEndpData(MSC_USB_ENDPOINT_1, &endp_tog_in, hbot.csw.data, &get_csw_rx_len, BOT_CSW_LENGTH);

	if(get_csw_status != ERR_SUCCESS)
	{
		PRINT("Non-Success get_csw_status: 0x%02X\n", get_csw_status);
		//return get_csw_status;
	}
	if(get_csw_rx_len != BOT_CSW_LENGTH)
	{
		PRINT("Unexpected get_csw_rx_len: %d\n", get_csw_rx_len);
	}

	return ERR_SUCCESS;
}

