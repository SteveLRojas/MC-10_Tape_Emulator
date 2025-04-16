/*
 * usbfsh_msc_bot.c
 *
 *  Created on: Sep 5, 2024
 *      Author: Steve
 */
//TODO: remove debug prints
#include "ch32v20x.h"
#include "ch32v203_usbfsh.h"
#include "debug.h"
#include "usbfsh_msc_bot.h"
#include <string.h>

//HINT: debug stuff
/* USB Device Descriptor */
typedef struct __attribute__((packed)) _USB_DEVICE_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

/* USB Configuration Descriptor */
typedef struct __attribute__((packed)) _USB_CONFIG_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

/* USB Interface Descriptor */
typedef struct __attribute__((packed)) _USB_INTERF_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  bInterfaceProtocol;
    uint8_t  iInterface;
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

/* USB Endpoint Descriptor */
typedef struct __attribute__((packed)) _USB_ENDPOINT_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t  wMaxPacketSize;
    uint8_t  bInterval;
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

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
//HINT: end debug stuff

#define common_buf_size 256
uint8_t usbfsh_msc_bot_common_buf[common_buf_size] __attribute__ ((aligned(4)));
usbfsh_ep_info_t usbfsh_msc_bot_ep0_info;
usbfsh_ep_info_t usbfsh_msc_bot_ep_in_info;
usbfsh_ep_info_t usbfsh_msc_bot_ep_out_info;

static const uint8_t usb_request_dev_descr[] =
{
	USB_REQ_TYP_IN | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_REQ_GET_DESCRIPTOR,
	0x00, USB_DESC_TYPE_DEVICE,
	0x00, 0x00,
	18, 0x00
};

static const uint8_t usb_request_config_descr[] =
{
	USB_REQ_TYP_IN | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_REQ_GET_DESCRIPTOR,
	0x00, USB_DESC_TYPE_CONFIGURATION,
	0x00, 0x00,
	9, 0x00
};

static const uint8_t usb_request_set_addr[] =
{
	USB_REQ_TYP_OUT | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_REQ_SET_ADDRESS,
	0x01, 0x00,
	0x00, 0x00,
	0x00, 0x00
};

static const uint8_t usb_request_set_config[] =
{
	USB_REQ_TYP_OUT | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_REQ_SET_CONFIGURATION,
	0x01, 0x00,
	0x00, 0x00,
	0x00, 0x00
};

static const uint8_t usb_request_bot_reset[] =
{
	USB_REQ_TYP_OUT | USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF,
	USB_REQ_BOT_RESET,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00
};

static const uint8_t usb_request_get_max_lun[] =
{
	USB_REQ_TYP_IN | USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF,
	USB_REQ_GET_MAX_LUN,
	0x00, 0x00,
	0x00, 0x00,
	0x01, 0x00
};

void usbfsh_msc_bot_init(void)
{
	usbfsh_init();
	usbfsh_in_transfer_nak_limit = 0xFFFF;
	usbfsh_out_transfer_nak_limit = 0xFFFF;

	(void)memset((void*)(&usbfsh_msc_bot_ep0_info), 0x00, 4);
	(void)memset((void*)(&usbfsh_msc_bot_ep_in_info), 0x00, 4);
	(void)memset((void*)(&usbfsh_msc_bot_ep_out_info), 0x00, 4);

	usbfsh_msc_bot_ep0_info.max_packet_size = 8;
	usbfsh_msc_bot_ep_in_info.max_packet_size = 4;
	usbfsh_msc_bot_ep_out_info.max_packet_size = 4;
}

//HINT: returns 1 on success, 0 on failure
uint8_t usbfsh_msc_bot_configure(void)
{
	uint8_t retry_count = 0;
	uint8_t response;
	uint8_t num_configs;
	uint8_t config_idx;
	uint16_t config_descr_len;
	uint8_t* config_data;
	uint8_t descriptor_length;
	uint8_t descriptor_type;
	uint8_t current_config;
	uint8_t current_iface;
	uint8_t bot_scsi_config;
	uint8_t bot_scsi_iface;
	uint8_t ep_address;
	uint16_t ep_max_size;

	do
	{
		//check if there is a device attached
		if(usbfsh_port_is_attached())
		{
			Delay_Ms(50);
		}
		else
		{
			return 0;	//no device attached
		}

		//reset the device
		usbfsh_begin_port_reset();
		Delay_Ms(50);
		usbfsh_end_port_reset();

		//check the device speed
		Delay_Ms(1);
		if(usbfsh_port_is_low_speed())
		{
			usbfsh_configure_port(USBFSH_LOW_SPEED | USBFSH_PORT_ENABLE);
			usbfsh_ctrl_set_low_speed();
		}
		else
		{
			usbfsh_configure_port(USBFSH_FULL_SPEED | USBFSH_PORT_ENABLE);
			usbfsh_ctrl_set_full_speed();
		}
		usbfsh_set_address(0x00);

		// get device descriptor
		Delay_Ms(10);
		(void)memcpy(usbfsh_tx_buf, usb_request_dev_descr, 8);
		response = usbfsh_control_transfer(&usbfsh_msc_bot_ep0_info, usbfsh_msc_bot_common_buf);

		if(response == USB_PID_ACK)
		{
			printf("Got device descriptor\n");
			usb_print_device_descriptor((USB_DEV_DESCR*)usbfsh_msc_bot_common_buf);
		}
		else
		{
			printf("Failed to get device descriptor\n");
			continue;
		}
		usbfsh_msc_bot_ep0_info.max_packet_size = usbfsh_msc_bot_common_buf[7];
		num_configs = usbfsh_msc_bot_common_buf[17];

		//set address
		(void)memcpy(usbfsh_tx_buf, usb_request_set_addr, 8);
		response = usbfsh_control_transfer(&usbfsh_msc_bot_ep0_info, usbfsh_msc_bot_common_buf);

		if(response == USB_PID_DATA1)
		{
			printf("Device address set successfully\n");
		}
		else
		{
			printf("Failed to set device address\n");
			continue;
		}
		usbfsh_set_address(0x01);

		//get configuration descriptors
		Delay_Ms(1);
		config_idx = 0;
		current_config = 0x00;
		current_iface = 0x00;
		bot_scsi_config = 0x00;
		bot_scsi_iface = 0xFF;
		do
		{
			(void)memcpy(usbfsh_tx_buf, usb_request_config_descr, 8);
			usbfsh_tx_buf[2] = config_idx;
			response = usbfsh_control_transfer(&usbfsh_msc_bot_ep0_info, usbfsh_msc_bot_common_buf);

			if(response != USB_PID_ACK)
			{
				printf("Failed to get configuration descriptor\n");
				printf("response: %02X, toggle: %02X\n", response, usbfsh_is_toggle_ok());
				break;
			}

			config_descr_len = *((uint16_t*)(usbfsh_msc_bot_common_buf + 2));
			if(config_descr_len > common_buf_size)
			{
				printf("Configuration descriptor too long\n");
				return 0;	//could not parse descriptor
			}

			*(uint16_t*)(usbfsh_tx_buf + 6) = config_descr_len;
			response = usbfsh_control_transfer(&usbfsh_msc_bot_ep0_info, usbfsh_msc_bot_common_buf);

			if(response != USB_PID_ACK)
			{
				printf("Failed to get configuration descriptor (full)\n");
				printf("response: %02X, toggle: %02X\n", response, usbfsh_is_toggle_ok());
				break;
			}

			//parse configuration descriptor
			printf("Config descriptor total length: %d\n", config_descr_len);
			config_data = usbfsh_msc_bot_common_buf;
			while(config_data < (usbfsh_msc_bot_common_buf + config_descr_len))
			{
				descriptor_length = config_data[0];
				descriptor_type = config_data[1];
				switch(descriptor_type)
				{
				case USB_DESC_TYPE_CONFIGURATION:
					usb_print_configuration_descriptor((USB_CFG_DESCR*)config_data);
					current_config = config_data[5];   //keep track of which configuration we are parsing
					break;
				case USB_DESC_TYPE_INTERFACE:
					current_iface = config_data[2];   //keep track of this interface

					//use interface descriptor to determine if the device is a flash drive
					if(config_data[5] == 8)    //class code is mass storage
					{
						if(config_data[6] == 6)    //subclass is SCSI transparent command
						{
							if(config_data[7] == 0x50)    //protocol code bulk-only transport
							{
								printf("Found flash drive interface\n");
								bot_scsi_config = current_config;
								bot_scsi_iface = current_iface;
							}
						}
					}
					usb_print_interface_descriptor((USB_ITF_DESCR*)config_data);
					break;
				case USB_DESC_TYPE_ENDPOINT:
					usb_print_endpoint_descriptor((USB_ENDP_DESCR*)config_data);

					if(current_iface == bot_scsi_iface)
					{
						ep_address = config_data[2];
						((uint8_t*)(&ep_max_size))[0] = config_data[4];	//must not cast config_data to uint16_t* due to alignment
						((uint8_t*)(&ep_max_size))[1] = config_data[5];

						if(ep_max_size > 64)
							ep_max_size = 64;

						if(ep_address & 0x80)	//IN
						{
							usbfsh_msc_bot_ep_in_info.ep_num = (ep_address & 0x0F);
							usbfsh_msc_bot_ep_in_info.max_packet_size = (uint8_t)ep_max_size;
						}
						else	//OUT
						{
							usbfsh_msc_bot_ep_out_info.ep_num = (ep_address & 0x0F);
							usbfsh_msc_bot_ep_out_info.max_packet_size = (uint8_t)ep_max_size;
						}
					}
					break;
				default:
					printf("Got an unknown descriptor: %02X\n", descriptor_type);
					break;
				}
				config_data = config_data + descriptor_length;
			}

			if(bot_scsi_iface != 0xFF)
			{
				break;
			}
		} while(++config_idx < num_configs);

		if(bot_scsi_iface != 0xFF)
		{
			//set configuration
			(void)memcpy(usbfsh_tx_buf, usb_request_set_config, 8);
			usbfsh_tx_buf[2] = bot_scsi_config;
			response = usbfsh_control_transfer(&usbfsh_msc_bot_ep0_info, usbfsh_msc_bot_common_buf);

			if(response == USB_PID_DATA1)
			{
				printf("Device configured\n");
				return 1;	//success
			}
			else
			{
				printf("Set configuration failed\n");
				continue;
			}
		}

		if(config_idx == num_configs)
		{
			return 0;	//did not find flash drive interface
		}
    } while(++retry_count < 6);

	return 0;	//failed too many times
}

uint8_t usbfsh_msc_bot_reset(void)
{
	uint8_t response;

	(void)memcpy(usbfsh_tx_buf, usb_request_bot_reset, 8);
	response = usbfsh_control_transfer(&usbfsh_msc_bot_ep0_info, usbfsh_msc_bot_common_buf);

	return (response == USB_PID_DATA1);
}

//HINT: returns the maximum LUN supported by the device, or 0xFF if the request fails.
uint8_t usbfsh_msc_bot_get_max_lun(void)
{
	uint8_t response;

	(void)memcpy(usbfsh_tx_buf, usb_request_get_max_lun, 8);
	response = usbfsh_control_transfer(&usbfsh_msc_bot_ep0_info, usbfsh_msc_bot_common_buf);

	return (response == USB_PID_ACK) ? usbfsh_msc_bot_common_buf[0] : 0xFF;
}

uint8_t usbfsh_msc_bot_command(uint8_t lun, uint8_t* pbuf)
{
	uint8_t response;

	//Send CBW
	usbfsh_msc_bot_cbw->LUN = lun;
	response = usbfsh_out_transfer(&usbfsh_msc_bot_ep_out_info, usbfsh_msc_bot_common_buf, BOT_CBW_LENGTH);

	if(response != USB_PID_ACK)
	{
		printf("Non-Success CBW response: 0x%02X\n", response);
		//return 0;
	}
	Delay_Ms(100);	//TODO: remove this delay?

	//Transfer data
	if (usbfsh_msc_bot_cbw->DataTransferLength)
	{
		if ((usbfsh_msc_bot_cbw->Flags & MSC_BOT_DIR_MSK) == MSC_BOT_DIR_IN)
		{
			//Data direction is IN
			response = usbfsh_in_transfer(&usbfsh_msc_bot_ep_in_info, pbuf, usbfsh_msc_bot_cbw->DataTransferLength);

			if((response != USB_PID_DATA0) && (response != USB_PID_DATA1))
			{
				printf("Non-Success data IN response: 0x%02X\n", response);
				//return 0;
			}
		}
		else
		{
			//Data direction is OUT
			response = usbfsh_out_transfer(&usbfsh_msc_bot_ep_out_info, pbuf, usbfsh_msc_bot_cbw->DataTransferLength);

			if(response != USB_PID_ACK)
			{
				printf("Non-Success data OUT response: 0x%02X\n", response);
				//return 0;
			}
		}
	}
	Delay_Ms(100);	//TODO: remove this delay?

	//Receive CSW
	response = usbfsh_in_transfer(&usbfsh_msc_bot_ep_in_info, usbfsh_msc_bot_common_buf, BOT_CSW_LENGTH);

	if((response != USB_PID_DATA0) && (response != USB_PID_DATA1))
	{
		printf("Non-Success CSW response: 0x%02X\n", response);
		//return 0;
	}

	return 1;
}
