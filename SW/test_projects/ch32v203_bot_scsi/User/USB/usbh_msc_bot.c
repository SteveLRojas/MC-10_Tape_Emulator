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
uint8_t endp_tog_out = 0x01;
uint8_t endp_tog_in = 0x01;


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


	// ----- BOT_DATA_IN OR BOT_DATA_OUT OR NONE
	// TODO: Read/Write
	Delay_Ms(100);


	// ----- RECEIVE_CSW
	uint16_t rx_len = 0;
	uint8_t get_csw_status = USBFSH_GetEndpData(MSC_USB_ENDPOINT_1, &endp_tog_in, hbot.csw.data, &rx_len, BOT_CSW_LENGTH);

	if(get_csw_status != ERR_SUCCESS)
	{
		PRINT("Non-Success get_csw_status: 0x%02X\n", get_csw_status);
		//return get_csw_status;
	}
	if(rx_len != BOT_CSW_LENGTH)
	{
		PRINT("Unexpected rx_len: %d\n", rx_len);
	}

	return ERR_SUCCESS;
}

////TODO: implement USBH_MSC_BOT_Process
//USBH_StatusTypeDef USBH_MSC_BOT_Process(uint8_t lun)
//{
//	USBH_StatusTypeDef   status = USBH_BUSY;
//	USBH_StatusTypeDef   error  = USBH_BUSY;
//	BOT_CSWStatusTypeDef CSW_Status = BOT_CSW_CMD_FAILED;
//	USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
//	//MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
//	uint8_t toggle = 0U;
//
//	switch (hbot.state)
//	{
//		case BOT_SEND_CBW:
//			hbot.cbw.field.LUN = lun;
//			hbot.state = BOT_SEND_CBW_WAIT;
//			(void)USBH_BulkSendData(phost, hbot.cbw.data, BOT_CBW_LENGTH, MSC_Handle->OutPipe, 1U);	//TODO: adapt this
//			break;
//
//		case BOT_SEND_CBW_WAIT:
//			URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->OutPipe);
//
//			if (URB_Status == USBH_URB_DONE)
//			{
//				if (hbot.cbw.field.DataTransferLength != 0U)
//				{
//					/* If there is Data Transfer Stage */
//					if (((hbot.cbw.field.Flags) & USB_REQ_DIR_MASK) == USB_D2H)
//					{
//						/* Data Direction is IN */
//						hbot.state = BOT_DATA_IN;
//					}
//					else
//					{
//						/* Data Direction is OUT */
//						hbot.state = BOT_DATA_OUT;
//					}
//				}
//				else
//				{
//					/* If there is NO Data Transfer Stage */
//					hbot.state = BOT_RECEIVE_CSW;
//				}
//			}
//			else if (URB_Status == USBH_URB_NOTREADY)
//			{
//				/* Re-send CBW */
//				hbot.state = BOT_SEND_CBW;
//			}
//			else
//			{
//				if (URB_Status == USBH_URB_STALL)
//				{
//					hbot.state  = BOT_ERROR_OUT;
//				}
//			}
//			break;
//
//		case BOT_DATA_IN:
//			/* Send first packet */
//			(void)USBH_BulkReceiveData(phost, hbot.pbuf, MSC_Handle->InEpSize, MSC_Handle->InPipe);	//TODO: adapt this
//
//			hbot.state = BOT_DATA_IN_WAIT;
//			break;
//
//		case BOT_DATA_IN_WAIT:
//			URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->InPipe);
//
//			if (URB_Status == USBH_URB_DONE)
//			{
//				/* Adjust Data pointer and data length */
//				if (hbot.cbw.field.DataTransferLength > MSC_Handle->InEpSize)
//				{
//					hbot.pbuf += MSC_Handle->InEpSize;
//					hbot.cbw.field.DataTransferLength -= MSC_Handle->InEpSize;
//				}
//				else
//				{
//					hbot.cbw.field.DataTransferLength = 0U;
//				}
//
//				/* More Data To be Received */
//				if (hbot.cbw.field.DataTransferLength > 0U)
//				{
//					/* Send next packet */
//					(void)USBH_BulkReceiveData(phost, hbot.pbuf, MSC_Handle->InEpSize, MSC_Handle->InPipe);	//TODO: adapt this
//				}
//				else
//				{
//					/* If value was 0, and successful transfer, then change the state */
//					hbot.state  = BOT_RECEIVE_CSW;
//				}
//			}
//			else if (URB_Status == USBH_URB_STALL)
//			{
//				/* This is Data IN Stage STALL Condition */
//				hbot.state  = BOT_ERROR_IN;
//
//				/* Refer to USB Mass-Storage Class : BOT (www.usb.org)
//				6.7.2 Host expects to receive data from the device
//				3. On a STALL condition receiving data, then:
//				The host shall accept the data received.
//				The host shall clear the Bulk-In pipe.
//				4. The host shall attempt to receive a CSW.*/
//			}
//			else
//			{
//			}
//			break;
//
//		case BOT_DATA_OUT:
//			(void)USBH_BulkSendData(phost, hbot.pbuf, MSC_Handle->OutEpSize, MSC_Handle->OutPipe, 1U);
//
//			hbot.state  = BOT_DATA_OUT_WAIT;
//			break;
//
//		case BOT_DATA_OUT_WAIT:
//			URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->OutPipe);
//
//			if (URB_Status == USBH_URB_DONE)
//			{
//				/* Adjust Data pointer and data length */
//				if (hbot.cbw.field.DataTransferLength > MSC_Handle->OutEpSize)
//				{
//					hbot.pbuf += MSC_Handle->OutEpSize;
//					hbot.cbw.field.DataTransferLength -= MSC_Handle->OutEpSize;
//				}
//				else
//				{
//					hbot.cbw.field.DataTransferLength = 0U;
//				}
//
//				/* More Data To be Sent */
//				if (hbot.cbw.field.DataTransferLength > 0U)
//				{
//					(void)USBH_BulkSendData(phost, hbot.pbuf, MSC_Handle->OutEpSize, MSC_Handle->OutPipe, 1U);
//				}
//				else
//				{
//					/* If value was 0, and successful transfer, then change the state */
//					hbot.state  = BOT_RECEIVE_CSW;
//				}
//			}
//			else if (URB_Status == USBH_URB_NOTREADY)
//			{
//				/* Resend same data */
//				hbot.state  = BOT_DATA_OUT;
//			}
//			else if (URB_Status == USBH_URB_STALL)
//			{
//				hbot.state  = BOT_ERROR_OUT;
//
//				/* Refer to USB Mass-Storage Class : BOT (www.usb.org)
//				6.7.3 Ho - Host expects to send data to the device
//				3. On a STALL condition sending data, then:
//				" The host shall clear the Bulk-Out pipe.
//				4. The host shall attempt to receive a CSW.
//				*/
//			}
//			else
//			{
//			}
//			break;
//
//		case BOT_RECEIVE_CSW:
//			(void)USBH_BulkReceiveData(phost, hbot.csw.data, BOT_CSW_LENGTH, MSC_Handle->InPipe);
//
//			hbot.state  = BOT_RECEIVE_CSW_WAIT;
//			break;
//
//		case BOT_RECEIVE_CSW_WAIT:
//			URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->InPipe);
//
//			/* Decode CSW */
//			if (URB_Status == USBH_URB_DONE)
//			{
//				hbot.state = BOT_SEND_CBW;
//				hbot.cmd_state = BOT_CMD_SEND;
//				CSW_Status = USBH_MSC_DecodeCSW(phost);
//
//				if (CSW_Status == BOT_CSW_CMD_PASSED)
//				{
//					status = USBH_OK;
//				}
//				else
//				{
//					status = USBH_FAIL;
//				}
//			}
//			else if (URB_Status == USBH_URB_STALL)
//			{
//				hbot.state  = BOT_ERROR_IN;
//			}
//			else
//			{
//			}
//			break;
//
//		case BOT_ERROR_IN:
//			error = USBH_MSC_BOT_Abort(phost, lun, BOT_DIR_IN);
//
//			if (error == USBH_OK)
//			{
//				hbot.state = BOT_RECEIVE_CSW;
//			}
//			else if (error == USBH_UNRECOVERED_ERROR)
//			{
//				/* This means that there is a STALL Error limit, Do Reset Recovery */
//				hbot.state = BOT_UNRECOVERED_ERROR;
//			}
//			else
//			{
//			}
//			break;
//
//		case BOT_ERROR_OUT:
//			error = USBH_MSC_BOT_Abort(phost, lun, BOT_DIR_OUT);
//
//			if (error == USBH_OK)
//			{
//				toggle = USBH_LL_GetToggle(phost, MSC_Handle->OutPipe);
//				(void)USBH_LL_SetToggle(phost, MSC_Handle->OutPipe, 1U - toggle);
//				(void)USBH_LL_SetToggle(phost, MSC_Handle->InPipe, 0U);
//				hbot.state = BOT_ERROR_IN;
//			}
//			else
//			{
//				if (error == USBH_UNRECOVERED_ERROR)
//				{
//					hbot.state = BOT_UNRECOVERED_ERROR;
//				}
//			}
//			break;
//
//
//		case BOT_UNRECOVERED_ERROR:
//			status = USBH_MSC_BOT_REQ_Reset(phost);
//			if (status == USBH_OK)
//			{
//				hbot.state = BOT_SEND_CBW;
//			}
//			break;
//
//		default:
//			break;
//	}
//	return status;
//}

