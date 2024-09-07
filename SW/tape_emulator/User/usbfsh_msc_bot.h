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

void usbfsh_msc_bot_init(void);
uint8_t usbfsh_msc_bot_configure(void);

#endif /* USER_USBFSH_MSC_BOT_H_ */
