/*
 * usbfsh_msc_scsi.h
 *
 *  Created on: Sep 24, 2024
 *      Author: Steve
 */

#ifndef _USBFSH_MSC_SCSI_H_
#define _USBFSH_MSC_SCSI_H_

typedef struct _SCSI_CAPACITY
{
	uint32_t num_blocks;
	uint16_t block_size;
} scsi_capacity_t;

typedef struct _SCSI_INQUIRY_DATA
{
	uint8_t peripheral_qualifier;
	uint8_t device_type;
	uint8_t removable_media;
	uint8_t vendor_id[9];
	uint8_t product_id[17];
	uint8_t revision_id[5];
} scsi_inquiry_data_t;

typedef struct _SCSI_SENSE_DATA
{
	uint8_t key;
	uint8_t asc;
	uint8_t ascq;
} scsi_sense_data_t;

#define OPCODE_TEST_UNIT_READY            0x00U
#define OPCODE_READ_CAPACITY10            0x25U
#define OPCODE_READ10                     0x28U
#define OPCODE_WRITE10                    0x2AU
#define OPCODE_REQUEST_SENSE              0x03U
#define OPCODE_INQUIRY                    0x12U

#define DATA_LEN_MODE_TEST_UNIT_READY     0U
#define DATA_LEN_READ_CAPACITY10          8U
#define DATA_LEN_INQUIRY                  36U
#define DATA_LEN_REQUEST_SENSE            14U

#define CBW_CB_LENGTH                     16U
#define CB_LENGTH_TEST_UNIT_READY         6U

#endif /* _USBFSH_MSC_SCSI_H_ */
