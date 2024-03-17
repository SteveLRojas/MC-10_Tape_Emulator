/*
 * UDisk_HW.h
 *
 *  Created on: Mar 15, 2024
 *      Author: Steve
 */

#ifndef USER_HOST_UDISK_UDISK_HW_H_
#define USER_HOST_UDISK_UDISK_HW_H_

#include "usb_host_config.h"


extern struct   _ROOT_HUB_DEVICE RootHubDev[ DEF_TOTAL_ROOT_HUB ];
extern struct   __HOST_CTL       HostCtl[ DEF_TOTAL_ROOT_HUB * DEF_ONE_USB_SUP_DEV_TOTAL ];

extern __attribute__((aligned(4)))  uint8_t  Com_Buffer[ DEF_COM_BUF_LEN ];     // even address , used for host enumcation and udisk operation
extern __attribute__((aligned(4)))  uint8_t  DevDesc_Buf[ 18 ];                 // Device Descriptor Buffer


void mStopIfError( uint8_t iError );
void Udisk_USBH_Initialization( void );
uint8_t Udisk_USBH_EnumRootDevice( uint8_t usb_port );

#endif /* USER_HOST_UDISK_UDISK_HW_H_ */
