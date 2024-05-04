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
#include "ch32v20x.h"
#include "UDisk_HW.h"
#include "usbh_msc_bot.h"

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
    /* Initialize USBFS host */
    USBFS_RCC_Init( );
    USBFS_Host_Init( ENABLE );
    memset( &RootHubDev[ DEF_USB_PORT_FS ].bStatus, 0, sizeof( struct _ROOT_HUB_DEVICE ) );
    memset( &HostCtl[ DEF_USB_PORT_FS ].InterfaceNum, 0, sizeof( struct __HOST_CTL ) );
	
	/* USB Libs Initialization */
    //printf( "UDisk library Initialization. \r\n" );
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
    uint8_t  cfg_val = 0;
    uint16_t i;
    uint16_t len;
    uint8_t enum_result = ERR_USB_UNSUPPORT;

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
			usb_print_device_descriptor((USB_DEV_DESCR*)DevDesc_Buf);
		}
		else
		{
			printf( "Err(%02x)\n", get_device_desc_result );
			enum_result = DEF_DEV_DESCR_GETFAIL;
			continue;
		}

		/* Set the USB device address */
		printf("Set DevAddr: ");
		RootHubDev[ usb_port ].bAddress = (uint8_t)( DEF_USB_PORT_FS + USB_DEVICE_ADDR );
		uint8_t set_usb_addr_result = USBFSH_SetUsbAddress( RootHubDev[ usb_port ].bEp0MaxPks, RootHubDev[ usb_port ].bAddress );
		if( set_usb_addr_result == ERR_SUCCESS )
		{
			printf( "OK\n" );
		}
		else
		{
			printf( "Err(%02x)\n", set_usb_addr_result );
			enum_result = DEF_DEV_ADDR_SETFAIL;
			continue;
		}
		Delay_Ms( 5 );

		/* Get the USB device configuration descriptor */
		printf("Get CfgDesc: ");
		uint8_t get_config_desc_result = USBFSH_GetConfigDescr( RootHubDev[ usb_port ].bEp0MaxPks, Com_Buffer, DEF_COM_BUF_LEN, &len );
		if( get_config_desc_result == ERR_SUCCESS )
		{
			uint16_t total_length = ((uint16_t*)Com_Buffer)[1];
			uint8_t* config_data = Com_Buffer;
			uint8_t descriptor_length;
			uint8_t descriptor_type;
			uint8_t bot_scsi_iface = 0xFF;
			uint8_t current_iface = 0x00;
			uint8_t ep_addr;
			printf("Config descriptor total length: %d\n", total_length);
			while(config_data < (Com_Buffer + total_length))
			{
				descriptor_length = config_data[0];
				descriptor_type = config_data[1];
				switch(descriptor_type)
				{
					case USB_DESC_TYPE_CONFIGURATION:
						usb_print_configuration_descriptor((USB_CFG_DESCR*)((void*)config_data));
						cfg_val = config_data[5];   //keep track of which configuration we are parsing
						break;
					case USB_DESC_TYPE_INTERFACE:
						current_iface = config_data[2];   //keep track of this interface

						//use interface descriptor to determine if the device is a keyboard
						if(config_data[5] == 8)    //class code is mass storage
						{
							if(config_data[6] == 6)    //subclass is SCSI transparent command
							{
								if(config_data[7] == 0x50)    //protocol code bulk-only transport
								{
									printf("Found flash drive interface\n");
									bot_scsi_iface = current_iface;
									/* Set USB device configuration value */
									printf("Set Cfg: \n");
									uint8_t set_usb_config_result = USBFSH_SetUsbConfig( RootHubDev[ usb_port ].bEp0MaxPks, cfg_val );
									if( set_usb_config_result == ERR_SUCCESS )
									{
										printf( "OK\n" );
										enum_result = ERR_SUCCESS;
									}
									else
									{
										printf( "Err(%02x)\n", set_usb_config_result );
										continue;
									}
									//reset toggles after set configuration
									endp_tog_out = 0x00;
									endp_tog_in = 0x00;
									//TODO: set protocol
									//usb_ctrlReq(1, 0, bmREQ_HID_OUT, HID_REQUEST_SET_PROTOCOL, USB_HID_BOOT_PROTOCOL, 0x00, boot_iface, 0x0000, NULL);
									//TODO: need set idle?
									//usb_ctrlReq(1, 0, bmREQ_HID_OUT, HID_REQUEST_SET_IDLE, 0, 0, boot_iface, 0x0000, NULL);
									//TODO: set interface?
									printf("Device configured\n");
								}
							}
						}
						usb_print_interface_descriptor((USB_ITF_DESCR*)((void*)config_data));
						break;
					case USB_DESC_TYPE_ENDPOINT:
						usb_print_endpoint_descriptor((USB_ENDP_DESCR*)((void*)config_data));

						if(current_iface == bot_scsi_iface)
						{
							ep_addr = ((USB_ENDP_DESCR*)((void*)config_data))->bEndpointAddress;

							if(ep_addr & 0x80)	//IN
							{
								endp_addr_in = (ep_addr & 0x0F);
							}
							else	//OUT
							{
								endp_addr_out = (ep_addr & 0x0F);
							}
						}
						break;
					default:
						printf("Got an unknown descriptor: %02X\n", descriptor_type);
						break;
				}
				config_data = config_data + descriptor_length;
			}
		}
		else
		{
			printf( "Err(%02x)\n", get_config_desc_result );
			enum_result = DEF_CFG_DESCR_GETFAIL;
			continue;
		}

		// If we got the descriptor, we're done either way
		break;
    }

    return enum_result;
}

void usb_print_device_descriptor(USB_DEV_DESCR* descriptor)
{
    printf("Device descriptor:\n");
	//print device descriptor
	printf("bLength: %02X\n", descriptor->bLength);
	printf("bDescriptorType: %02X\n", descriptor->bDescriptorType);
	printf("bcdUSB: %04X\n", descriptor->bcdUSB);
	printf("bDeviceClass: %02X\n", descriptor->bDeviceClass);
	printf("bDeviceSubClass: %02X\n", descriptor->bDeviceSubClass);
	printf("bDeviceProtocol: %02X\n", descriptor->bDeviceProtocol);
	printf("bMaxPacketSize0: %02X\n", descriptor->bMaxPacketSize0);
	printf("idVendor: %04X\n", descriptor->idVendor);
	printf("idProduct: %04X\n", descriptor->idProduct);
	printf("bcdDevice: %04X\n", descriptor->bcdDevice);
	printf("iManufacturer: %02X\n", descriptor->iManufacturer);
	printf("iProduct: %02X\n", descriptor->iProduct);
	printf("iSerialNumber: %02X\n", descriptor->iSerialNumber);
	printf("bNumConfigurations: %02X\n\n", descriptor->bNumConfigurations);
}

void usb_print_configuration_descriptor(USB_CFG_DESCR* descriptor)
{
    printf("Configuration descriptor:\n");
	printf("bLength: %02X\n", descriptor->bLength);
	printf("bDescriptorType: %02X\n", descriptor->bDescriptorType);
	printf("wTotalLength: %04X\n", descriptor->wTotalLength);
	printf("bNumInterfaces: %02X\n", descriptor->bNumInterfaces);
	printf("bConfigurationValue: %02X\n", descriptor->bConfigurationValue);
	printf("iConfiguration: %02X\n", descriptor->iConfiguration);
	printf("bmAttributes: %02X\n", descriptor->bmAttributes);
	printf("MaxPower: %02X\n\n", descriptor->MaxPower);
}

void usb_print_interface_descriptor(USB_ITF_DESCR* descriptor)
{
    printf("Interface descriptor:\n");
	printf("bLength: %02X\n", descriptor->bLength);
	printf("bInterfaceNumber: %02X\n", descriptor->bInterfaceNumber);
	printf("bAlternateSetting: %02X\n", descriptor->bAlternateSetting);
	printf("bNumEndpoints: %02X\n", descriptor->bNumEndpoints);
	printf("bInterfaceClass: %02X\n", descriptor->bInterfaceClass);
	printf("bInterfaceSubClass: %02X\n", descriptor->bInterfaceSubClass);
	printf("bInterfaceProtocol: %02X\n", descriptor->bInterfaceProtocol);
	printf("iInterface: %02X\n\n", descriptor->iInterface);
}

void usb_print_endpoint_descriptor(USB_ENDP_DESCR* descriptor)
{
    printf("Endpoint descriptor:\n");
	printf("bLength: %02X\n", descriptor->bLength);
	printf("bDescriptorType: %02X\n", descriptor->bDescriptorType);
	printf("bEndpointAddress: %02X\n", descriptor->bEndpointAddress);
	printf("bmAttributes: %02X\n", descriptor->bmAttributes);
	printf("wMaxPacketSize: %04X\n", descriptor->wMaxPacketSize);
	printf("bInterval: %02X\n\n", descriptor->bInterval);
}

