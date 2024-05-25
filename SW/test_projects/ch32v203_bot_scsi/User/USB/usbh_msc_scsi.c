#include "usb_host_config.h"
#include "usbh_msc.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"

#define USB_DEFAULT_BLOCK_SIZE 512

/**
  * @brief  USBH_MSC_SCSI_TestUnitReady
  *         Issue TestUnitReady command.
  * @param  lun: Logical Unit Number
  * @retval USBH Status
  */
uint8_t USBH_MSC_SCSI_TestUnitReady(uint8_t lun)
{
	/*Prepare the CBW and relevant field*/
	hbot.cbw.field.DataTransferLength = DATA_LEN_MODE_TEST_UNIT_READY;
	hbot.cbw.field.Flags = USB_EP_DIR_OUT;
	hbot.cbw.field.CBLength = CBW_LENGTH;

	(void)USBH_memset(hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
	hbot.cbw.field.CB[0]  = OPCODE_TEST_UNIT_READY;

	return USBH_MSC_BOT_Command(lun);
}

/**
  * @brief  USBH_MSC_SCSI_ReadCapacity
  *         Issue Read Capacity command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the capacity structure
  * @retval USBH Status
  */
uint8_t USBH_MSC_SCSI_ReadCapacity(uint8_t lun, SCSI_CapacityTypeDef *capacity)
{
	uint8_t error;

	/*Prepare the CBW and relevant field*/
	hbot.cbw.field.DataTransferLength = DATA_LEN_READ_CAPACITY10;
	hbot.cbw.field.Flags = USB_EP_DIR_IN;
	hbot.cbw.field.CBLength = CBW_LENGTH;

	(void)USBH_memset(hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
	hbot.cbw.field.CB[0]  = OPCODE_READ_CAPACITY10;

	hbot.pbuf = (uint8_t *)(void *)hbot.data;

	// Do Command
	error = USBH_MSC_BOT_Command(lun);

	if (error == ERR_SUCCESS)
	{
		/*assign the capacity*/
		capacity->block_nbr = hbot.pbuf[3] | ((uint32_t)hbot.pbuf[2] << 8U) | ((uint32_t)hbot.pbuf[1] << 16U) | ((uint32_t)hbot.pbuf[0] << 24U);

		/*assign the page length*/
		capacity->block_size = (uint16_t)(hbot.pbuf[7] | ((uint32_t)hbot.pbuf[6] << 8U));
	}

	return error;
}

/**
  * @brief  USBH_MSC_SCSI_Inquiry
  *         Issue Inquiry command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the inquiry structure
  * @retval USBH Status
  */
uint8_t USBH_MSC_SCSI_Inquiry(uint8_t lun, SCSI_StdInquiryDataTypeDef *inquiry)
{
	uint8_t error;

	/*Prepare the CBW and relevant field*/
	hbot.cbw.field.DataTransferLength = DATA_LEN_INQUIRY;
	hbot.cbw.field.Flags = USB_EP_DIR_IN;
	hbot.cbw.field.CBLength = CBW_LENGTH;

	(void)USBH_memset(hbot.cbw.field.CB, 0, CBW_LENGTH);
	hbot.cbw.field.CB[0]  = OPCODE_INQUIRY;
	hbot.cbw.field.CB[1]  = (lun << 5);
	hbot.cbw.field.CB[2]  = 0U;
	hbot.cbw.field.CB[3]  = 0U;
	hbot.cbw.field.CB[4]  = 0x24U;
	hbot.cbw.field.CB[5]  = 0U;

	hbot.pbuf = (uint8_t *)(void *)hbot.data;

	// Do Command
	error = USBH_MSC_BOT_Command(lun);
	if(error == ERR_SUCCESS)
	{
		(void)USBH_memset(inquiry, 0, sizeof(SCSI_StdInquiryDataTypeDef));
		/*assign Inquiry Data */
		inquiry->DeviceType = hbot.pbuf[0] & 0x1FU;
		inquiry->PeripheralQualifier = hbot.pbuf[0] >> 5U;

		if (((uint32_t)hbot.pbuf[1] & 0x80U) == 0x80U)
		{
			inquiry->RemovableMedia = 1U;
		}
		else
		{
			inquiry->RemovableMedia = 0U;
		}

		(void)USBH_memcpy(inquiry->vendor_id, &hbot.pbuf[8], 8U);
		(void)USBH_memcpy(inquiry->product_id, &hbot.pbuf[16], 16U);
		(void)USBH_memcpy(inquiry->revision_id, &hbot.pbuf[32], 4U);
	}

	return error;
}

/**
  * @brief  USBH_MSC_SCSI_Read
  *         Issue Read10 command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sectors to read
  * @retval USBH Status
  */
uint8_t USBH_MSC_SCSI_Read(uint8_t lun, uint32_t address, uint8_t *pbuf, uint32_t length)
{
	uint8_t error;

	/*Prepare the CBW and relevant field*/
	hbot.cbw.field.DataTransferLength = length * USB_DEFAULT_BLOCK_SIZE;
	hbot.cbw.field.Flags = USB_EP_DIR_IN;
	hbot.cbw.field.CBLength = CBW_LENGTH;

	(void)USBH_memset(hbot.cbw.field.CB, 0, CBW_CB_LENGTH);

	hbot.cbw.field.CB[0]  = OPCODE_READ10;

	/*logical block address*/
	hbot.cbw.field.CB[2]  = (((uint8_t *)(void *)&address)[3]);
	hbot.cbw.field.CB[3]  = (((uint8_t *)(void *)&address)[2]);
	hbot.cbw.field.CB[4]  = (((uint8_t *)(void *)&address)[1]);
	hbot.cbw.field.CB[5]  = (((uint8_t *)(void *)&address)[0]);

	/*Transfer length */
	hbot.cbw.field.CB[7]  = (((uint8_t *)(void *)&length)[1]);
	hbot.cbw.field.CB[8]  = (((uint8_t *)(void *)&length)[0]);

	hbot.pbuf = pbuf;

	error = USBH_MSC_BOT_Command(lun);

	return error;
}

/**
  * @brief  USBH_MSC_SCSI_Write
  *         Issue write10 command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sectors to write
  * @retval USBH Status
  */
uint8_t USBH_MSC_SCSI_Write(uint8_t lun, uint32_t address, uint8_t *pbuf, uint32_t length)
{
	uint8_t error;

	/*Prepare the CBW and relevant field*/
	hbot.cbw.field.DataTransferLength = length * USB_DEFAULT_BLOCK_SIZE;
	hbot.cbw.field.Flags = USB_EP_DIR_OUT;
	hbot.cbw.field.CBLength = CBW_LENGTH;

	(void) USBH_memset(hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
	hbot.cbw.field.CB[0] = OPCODE_WRITE10;

	/*logical block address*/
	hbot.cbw.field.CB[2] = (((uint8_t *) (void *) &address)[3]);
	hbot.cbw.field.CB[3] = (((uint8_t *) (void *) &address)[2]);
	hbot.cbw.field.CB[4] = (((uint8_t *) (void *) &address)[1]);
	hbot.cbw.field.CB[5] = (((uint8_t *) (void *) &address)[0]);

	/*Transfer length */
	hbot.cbw.field.CB[7] = (((uint8_t *) (void *) &length)[1]);
	hbot.cbw.field.CB[8] = (((uint8_t *) (void *) &length)[0]);

	hbot.pbuf = pbuf;

	error = USBH_MSC_BOT_Command(lun);

	return error;
}

/**
  * @brief  USBH_MSC_SCSI_RequestSense
  *         Issue RequestSense command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the sense data structure
  * @retval USBH Status
  */
uint8_t USBH_MSC_SCSI_RequestSense(uint8_t lun, SCSI_SenseTypeDef *sense_data)
{
	uint8_t error;

	/*Prepare the CBW and relevant field*/
	hbot.cbw.field.DataTransferLength = DATA_LEN_REQUEST_SENSE;
	hbot.cbw.field.Flags = USB_EP_DIR_IN;
	hbot.cbw.field.CBLength = CBW_LENGTH;

	(void) USBH_memset(hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
	hbot.cbw.field.CB[0] = OPCODE_REQUEST_SENSE;
	hbot.cbw.field.CB[1] = (lun << 5);
	hbot.cbw.field.CB[2] = 0U;
	hbot.cbw.field.CB[3] = 0U;
	hbot.cbw.field.CB[4] = DATA_LEN_REQUEST_SENSE;
	hbot.cbw.field.CB[5] = 0U;

	hbot.pbuf = (uint8_t *) (void *) hbot.data;

	error = USBH_MSC_BOT_Command(lun);

	if (error == ERR_SUCCESS)
	{
		sense_data->key = hbot.pbuf[2] & 0x0FU;
		sense_data->asc = hbot.pbuf[12];
		sense_data->ascq = hbot.pbuf[13];
	}

	return error;
}
