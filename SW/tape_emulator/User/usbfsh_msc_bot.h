/*
 * usbfsh_msc_bot.h
 *
 *  Created on: Sep 5, 2024
 *      Author: Steve
 */

#ifndef USER_USBFSH_MSC_BOT_H_
#define USER_USBFSH_MSC_BOT_H_

/* Table 9-4. Standard Request Codes  */
/* bRequest , Value */
#ifndef USB_REQ_GET_STATUS
#define  USB_REQ_GET_STATUS                                0x00U
#define  USB_REQ_CLEAR_FEATURE                             0x01U
#define  USB_REQ_SET_FEATURE                               0x03U
#define  USB_REQ_SET_ADDRESS                               0x05U
#define  USB_REQ_GET_DESCRIPTOR                            0x06U
#define  USB_REQ_SET_DESCRIPTOR                            0x07U
#define  USB_REQ_GET_CONFIGURATION                         0x08U
#define  USB_REQ_SET_CONFIGURATION                         0x09U
#define  USB_REQ_GET_INTERFACE                             0x0AU
#define  USB_REQ_SET_INTERFACE                             0x0BU
#define  USB_REQ_SYNCH_FRAME                               0x0CU
#endif

/* Table 9-5. Descriptor Types of USB Specifications */
#ifndef USB_DESC_TYPE_DEVICE
#define  USB_DESC_TYPE_DEVICE                              0x01U
#define  USB_DESC_TYPE_CONFIGURATION                       0x02U
#define  USB_DESC_TYPE_STRING                              0x03U
#define  USB_DESC_TYPE_INTERFACE                           0x04U
#define  USB_DESC_TYPE_ENDPOINT                            0x05U
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    0x06U
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           0x07U
#define  USB_DESC_TYPE_INTERFACE_POWER                     0x08U
#define  USB_DESC_TYPE_HID                                 0x21U
#define  USB_DESC_TYPE_HID_REPORT                          0x22U
#endif

#define USB_REQ_BOT_RESET       0xFFU
#define USB_REQ_GET_MAX_LUN     0xFEU

#define BOT_CBW_SIGNATURE   0x43425355U
#define BOT_CBW_TAG         0x20304050U
#define BOT_CSW_SIGNATURE   0x53425355U
#define BOT_CBW_LENGTH      31U
#define BOT_CSW_LENGTH      13U

#define  MSC_BOT_DIR_OUT    0x00U
#define  MSC_BOT_DIR_IN     0x80U
#define  MSC_BOT_DIR_MSK    0x80U

typedef struct _MSC_BOT_CBW
{
	uint32_t Signature;
	uint32_t Tag;
	uint32_t DataTransferLength;
	uint8_t  Flags;
	uint8_t  LUN;
	uint8_t  CBLength;
	uint8_t  CB[16];
} msc_bot_cbw_t;

typedef struct _MSC_BOT_CSW
{
	uint32_t Signature;
	uint32_t Tag;
	uint32_t DataResidue;
	uint8_t  Status;
} msc_bot_csw_t;

//HINT: User interface begins below this line
#define usbfsh_msc_bot_cbw ((msc_bot_cbw_t*)usbfsh_msc_bot_common_buf)
#define usbfsh_msc_bot_csw ((msc_bot_csw_t*)usbfsh_msc_bot_common_buf)

void usbfsh_msc_bot_init(void);
uint8_t usbfsh_msc_bot_configure(void);
uint8_t usbfsh_msc_bot_reset(void);
uint8_t usbfsh_msc_bot_get_max_lun(void);
uint8_t usbfsh_msc_bot_command(uint8_t lun, uint8_t* pbuf);

#endif /* USER_USBFSH_MSC_BOT_H_ */
