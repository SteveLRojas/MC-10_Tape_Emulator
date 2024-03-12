/********************************** (C) COPYRIGHT *******************************
* File Name          : UDisk_HW.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/09/01
* Description        : USB full-speed port host operation functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*******************************************************************************/
/* Header File */
#include "Udisk_Operation.h"

/*******************************************************************************/
/* Variable Definition */
__attribute__((aligned(4)))  uint8_t  Com_Buffer[ DEF_COM_BUF_LEN ];     // even address , used for host enumcation and udisk operation
__attribute__((aligned(4)))  uint8_t  DevDesc_Buf[ 18 ];                 // Device Descriptor Buffer
struct   _ROOT_HUB_DEVICE RootHubDev[ DEF_TOTAL_ROOT_HUB ];
struct   __HOST_CTL HostCtl[ DEF_TOTAL_ROOT_HUB * DEF_ONE_USB_SUP_DEV_TOTAL ];
volatile uint8_t  UDisk_Opeation_Flag = 0;

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   Checking the operation status, displaying the error code and stopping if there is an error
 *          input : iError - Error code input
 *
 * @return  none
 */
void mStopIfError( uint8_t iError )
{
    if ( iError == ERR_SUCCESS )
    {
        /* operation success, return */
        return;
    }
    /* Display the errors */
    DUG_PRINTF( "Error:%02x\r\n", iError );
    /* After encountering an error, you should analyze the error code and CHRV3DiskStatus status, for example,
     * call CHRV3DiskReady to check whether the current USB disk is connected or not,
     * if the disk is disconnected then wait for the disk to be plugged in again and operate again,
     * Suggested steps to follow after an error:
     *     1, call CHRV3DiskReady once, if successful, then continue the operation, such as Open, Read/Write, etc.
     *     2?If CHRV3DiskReady is not successful, then the operation will be forced to start from the beginning.
     */
    while(1)
    {  }
}

/*********************************************************************
 * @fn      Udisk_USBH_Initialization
 *
 * @brief   USB Host Udisk process Initialization, include usb-host initialization
 *          usb libs initialization, udisk-related values Initialization 
            and the others
 *
 * @para    none
 *
 * @return  none
 */
void Udisk_USBH_Initialization( void )
{
	/* USB Host Initialization */
    printf( "USB Host & UDisk Lib Initialization. \r\n" );
    /* Initialize USBFS host */
    DUG_PRINTF( "USBFS Host Init\r\n" );
    USBFS_RCC_Init( );
    USBFS_Host_Init( ENABLE );
    memset( &RootHubDev[ DEF_USB_PORT_FS ].bStatus, 0, sizeof( struct _ROOT_HUB_DEVICE ) );
    memset( &HostCtl[ DEF_USB_PORT_FS ].InterfaceNum, 0, sizeof( struct __HOST_CTL ) );
	
	/* USB Libs Initialization */
    printf( "UDisk library Initialization. \r\n" );
    //CHRV3LibInit( );
}

/*********************************************************************
 * @fn      Udisk_USBH_EnumRootDevice
 *
 * @brief   Generally enumerate a device connected to host port.
 *
 * @para    index: USB host port
 *
 * @return  Enumeration result
 */
uint8_t Udisk_USBH_EnumRootDevice( uint8_t usb_port )
{
    uint8_t  success_cnt;
    uint8_t  enum_cnt;
    uint8_t  cfg_val;
    uint16_t i;
    uint16_t len;
    uint8_t enum_result;

    DUG_PRINTF( "Enum:\r\n" );

    enum_cnt = 0;
    while(enum_cnt < 6)
    {
		/* Delay and wait for the device to stabilize */
		Delay_Ms( 100 );
		enum_cnt++;
		Delay_Ms( 8 << enum_cnt );

		/* Reset the USB device and wait for the USB device to reconnect */
		USBFSH_ResetRootHubPort( 0 );
		for( i = 0, success_cnt = 0; i < DEF_RE_ATTACH_TIMEOUT; i++ )
		{
			if( USBFSH_EnableRootHubPort( &RootHubDev[ usb_port ].bSpeed ) == ERR_SUCCESS )
			{
				i = 0;
				success_cnt++;
				if( success_cnt > 6 )
				{
					break;
				}
			}
			Delay_Ms( 1 );
		}
		if( i )
		{
			enum_result = ERR_USB_DISCON;
			continue;
		}

		/* Select USB speed */
		USBFSH_SetSelfSpeed( RootHubDev[ usb_port].bSpeed );

		/* Get USB device device descriptor */
		printf("getting device descriptor\n");
		uint8_t get_device_desc_result = USBFSH_GetDeviceDescr( &RootHubDev[ usb_port ].bEp0MaxPks, DevDesc_Buf );
		if( get_device_desc_result == ERR_SUCCESS )
		{
			//print device descriptor
			printf("bLength: %02X\n", DevDesc_Buf[0]);
			printf("bDescriptorType: %02X\n", DevDesc_Buf[1]);
			printf("bcdUSB: %04X\n", *((uint16_t*)(DevDesc_Buf + 2)));
			printf("bDeviceClass: %02X\n", DevDesc_Buf[4]);
			printf("bDeviceSubClass: %02X\n", DevDesc_Buf[5]);
			printf("bDeviceProtocol: %02X\n", DevDesc_Buf[6]);
			printf("bMaxPacketSize0: %02X\n", DevDesc_Buf[7]);
			printf("idVendor: %04X\n", *((uint16_t*)(DevDesc_Buf + 8)));
			printf("idProduct: %04X\n", *((uint16_t*)(DevDesc_Buf + 10)));
			printf("bcdDevice: %04X\n", *((uint16_t*)(DevDesc_Buf + 12)));
			printf("iManufacturer: %02X\n", DevDesc_Buf[14]);
			printf("iProduct: %02X\n", DevDesc_Buf[15]);
			printf("iSerialNumber: %02X\n", DevDesc_Buf[16]);
			printf("bNumConfigurations: %02X\n\n", DevDesc_Buf[17]);
		}
		else
		{
			DUG_PRINTF( "Err(%02x)\n", get_device_desc_result );
			enum_result = DEF_DEV_DESCR_GETFAIL;
			continue;
		}

		/* Set the USB device address */
		DUG_PRINTF("Set DevAddr: ");
		RootHubDev[ usb_port ].bAddress = (uint8_t)( DEF_USB_PORT_FS + USB_DEVICE_ADDR );
		uint8_t set_usb_addr_result = USBFSH_SetUsbAddress( RootHubDev[ usb_port ].bEp0MaxPks, RootHubDev[ usb_port ].bAddress );
		if( set_usb_addr_result == ERR_SUCCESS )
		{
			DUG_PRINTF( "OK\n" );
		}
		else
		{
			DUG_PRINTF( "Err(%02x)\n", set_usb_addr_result );
			enum_result = DEF_DEV_ADDR_SETFAIL;
			continue;
		}
		Delay_Ms( 5 );

		/* Get the USB device configuration descriptor */
		DUG_PRINTF("Get CfgDesc: ");
		uint8_t get_config_desc_result = USBFSH_GetConfigDescr( RootHubDev[ usb_port ].bEp0MaxPks, Com_Buffer, DEF_COM_BUF_LEN, &len );
		if( get_config_desc_result == ERR_SUCCESS )
		{
			cfg_val = ( (PUSB_CFG_DESCR)Com_Buffer )->bConfigurationValue;

			/* Print USB device configuration descriptor  */
			for( i = 0; i < len; i++ )
			{
				//TODO: make this nice
				DUG_PRINTF( "%02x ", Com_Buffer[ i ] );
			}
			DUG_PRINTF("\n");
		}
		else
		{
			DUG_PRINTF( "Err(%02x)\n", get_config_desc_result );
			enum_result = DEF_CFG_DESCR_GETFAIL;
			continue;
		}

		/* Set USB device configuration value */
		DUG_PRINTF("Set Cfg: ");
		uint8_t set_usb_config_result = USBFSH_SetUsbConfig( RootHubDev[ usb_port ].bEp0MaxPks, cfg_val );
		if( set_usb_config_result == ERR_SUCCESS )
		{
			DUG_PRINTF( "OK\n" );
		}
		else
		{
			DUG_PRINTF( "Err(%02x)\n", set_usb_config_result );
			enum_result = ERR_USB_UNSUPPORT;
			continue;
		}

		//TODO: get interface and endpoint descriptors

		enum_result = ERR_SUCCESS;
		break;
    }

    return enum_result;
}


