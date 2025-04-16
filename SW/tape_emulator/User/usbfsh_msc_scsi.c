/*
 * usbfsh_msc_scsi.c
 *
 *  Created on: Sep 24, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "usbfsh_msc_bot.h"
#include "usbfsh_msc_scsi.h"
#include <string.h>

uint8_t usbfsh_msc_scsi_test_unit_ready(uint8_t lun)
{
	usbfsh_msc_bot_cbw->Signature = BOT_CBW_SIGNATURE;
	usbfsh_msc_bot_cbw->Tag = BOT_CBW_TAG;
	usbfsh_msc_bot_cbw->DataTransferLength = 0;
	usbfsh_msc_bot_cbw->Flags = MSC_BOT_DIR_OUT;
	usbfsh_msc_bot_cbw->LUN = lun;
	usbfsh_msc_bot_cbw->CBLength = CB_LENGTH_TEST_UNIT_READY;

	(void)memset(usbfsh_msc_bot_cbw->CB, 0, CBW_CB_LENGTH);
	usbfsh_msc_bot_cbw->CB[0] = OPCODE_TEST_UNIT_READY;

	return usbfsh_msc_bot_command(lun, 0);
}

uint8_t usbfsh_mscs_scsi_read_capacity(uint8_t lun, scsi_capacity_t* capacity)
{

}

uint8_t usbfsh_msc_scsi_inquiry(uint8_t lun)
{

}

uint8_t usbfsh_msc_scsi_read(uint8_t lun, uint32_t block_addr, uint8_t* pbuf, uint32_t num_blocks)
{

}

uint8_t usbfsh_msc_scsi_write(uint8_t lun, uint32_t block_addr, uint8_t* pbuf, uint32_t num_blocks)
{

}

uint8_t usbfsh_msc_scsi_request_sense(uint8_t lun)
{

}
