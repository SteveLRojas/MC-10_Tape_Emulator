#include "debug.h"
#include "stdio.h"
#include "string.h"
#include "UDisk_HW.h"
#include "usbh_msc_bot.h"
#include "usbh_msc_scsi.h"
#include "display.h"
#include "ff.h"

#define PIN_LED GPIO_Pin_1
#define PIN_SW0 GPIO_Pin_13
#define PIN_SW1 GPIO_Pin_14
#define PIN_SW2 GPIO_Pin_8
#define PIN_SW3 GPIO_Pin_9
#define PORT_LED GPIOB
#define PORT_SW0_SW1 GPIOC
#define PORT_SW2_SW3 GPIOB

char booba_str[] = "deer sneeze";

void platform_init()
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);

	// Init PORTA (LCD_BUS)
	GPIO_InitStructure.GPIO_Pin = (0x00FF);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_LCD_BUS, &GPIO_InitStructure);

	// Init PORTB (LCD_CTRL)
	GPIO_InitStructure.GPIO_Pin = (PIN_LED | PIN_LCD_CTRL_RW | PIN_LCD_CTRL_E | PIN_LCD_CTRL_RS);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_LCD_CTRL, &GPIO_InitStructure);

	// Init display control signals to 0
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RW, Bit_RESET);
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_E, Bit_RESET);
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_RESET);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = PIN_SW0 | PIN_SW1;
	GPIO_Init(PORT_SW0_SW1, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PIN_SW2 | PIN_SW3;
	GPIO_Init(PORT_SW2_SW3, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
}

int main(void)
{
	platform_init();
    USART_Printf_Init( 115200 );
    Delay_Init( );
    printf( "SystemClk:%d\n", SystemCoreClock );
    lcd_init();
    lcd_cursor_home();
    lcd_print_string(booba_str);

    /* General USB Host UDisk Operation Initialization */
    Udisk_USBH_Initialization( );

    uint8_t usb_state = 0;
    uint8_t bot_reset_status;
	uint8_t max_lun;
	//uint8_t single_sector_buf[512];
	FATFS FatFs;   /* Work area (filesystem object) for logical drive */
	FIL fil;        /* File object */
	char buffer[100]; /* File buffer */
    UINT br, bw;         /* File read/write count */

    while(1)
    {
    	switch(usb_state)
    	{
			case 0:
				GPIO_WriteBit(PORT_LED, PIN_LED, Bit_SET);
				if(USBOTG_H_FS->MIS_ST & USBFS_UMS_DEV_ATTACH)
				{
					printf("Device attached!\n");
					GPIO_WriteBit(PORT_LED, PIN_LED, Bit_RESET);
					usb_state = 1;
				}
				break;
			case 1:
				printf("EnumRootDevice result: %02X\n", Udisk_USBH_EnumRootDevice(0));
				usb_state = 2;
				break;
			case 2:
				bot_reset_status = USBH_MSC_BOT_REQ_Reset();
				printf("BOT reset status: %02X\n", bot_reset_status);
				Delay_Ms(10);

				USBH_MSC_BOT_REQ_GetMaxLUN(&max_lun);
				printf("Max LUN is: %02x\n", max_lun);

				USBH_MSC_BOT_Init();
				printf("Starting USBH_MSC_SCSI_TestUnitReady!\n");
				USBH_MSC_SCSI_TestUnitReady(0);
				printf("Finished USBH_MSC_SCSI_TestUnitReady!\n");

				printf("\n---CBW---\n");
				printf("Signature............[%08X]\n", hbot.cbw.field.Signature);
				printf("Tag..................[%08X]\n", hbot.cbw.field.Tag);
				printf("DataTransferLength...[%08X]\n", hbot.cbw.field.DataTransferLength);
				printf("Flags................[%02X]\n", hbot.cbw.field.Flags);
				printf("LUN..................[%02X]\n", hbot.cbw.field.LUN);
				printf("CBLength.............[%02X]\n", hbot.cbw.field.CBLength);
				printf("CB[0]................[%02X]\n", hbot.cbw.field.CB[0]);

				printf("\n---CSW---\n");
				printf("Signature............[%08X]\n", hbot.csw.field.Signature);
				printf("Tag..................[%08X]\n", hbot.csw.field.Tag);
				printf("Data Residue.........[%08X]\n", hbot.csw.field.DataResidue);
				printf("Status...............[%02X]\n", hbot.csw.field.Status);

				printf("\nDone.\n\n");

				printf("\n---SCSI RequestSense---\n");
				SCSI_SenseTypeDef sense_data;
				USBH_MSC_SCSI_RequestSense(0, &sense_data);
				printf("key   : %02X\n", sense_data.key);
				printf("asc   : %02X\n", sense_data.asc);
				printf("ascq  : %02X\n", sense_data.ascq);

				printf("\n---SCSI Inquiry---\n");
				SCSI_StdInquiryDataTypeDef inquiry;
				USBH_MSC_SCSI_Inquiry(0, &inquiry);
				printf("Inquiry Vendor  : %s\n", inquiry.vendor_id);
				printf("Inquiry Product : %s\n", inquiry.product_id);
				printf("Inquiry Version : %s\n", inquiry.revision_id);

				printf("\nSCSI ReadCapacity:\n");
				SCSI_CapacityTypeDef size_info;
				USBH_MSC_SCSI_ReadCapacity(0, &size_info);
				printf("block_nbr: %u\n", size_info.block_nbr);
				printf("block_size: %hu\n", size_info.block_size);

				usb_state = 3;
				break;
			case 3:
				//idle
				if(!(USBOTG_H_FS->MIS_ST & USBFS_UMS_DEV_ATTACH))
				{
					printf("Device detached!\n");
					usb_state = 0;
				}
				else if(!GPIO_ReadInputDataBit(PORT_SW0_SW1, PIN_SW0))
				{
					printf("Button 0 pressed!\n");
					usb_state = 1;
				}
				else if(!GPIO_ReadInputDataBit(PORT_SW0_SW1, PIN_SW1))
				{
					printf("Button 1 pressed!\n");
					usb_state = 4;
				}
				else if(!GPIO_ReadInputDataBit(PORT_SW2_SW3, PIN_SW2))
				{
					printf("Button 2 pressed!\n");
					usb_state = 5;
				}
				else if(!GPIO_ReadInputDataBit(PORT_SW2_SW3, PIN_SW3))
				{
					printf("Button 3 pressed!\n");
					usb_state = 6;
				}
				break;
			case 4:
				/* Give a work area to the default drive */
				f_mount(&FatFs, "", 0);

			    char str[12];
				f_getlabel("", str, 0);
				printf("Label: %s\n", str);

				usb_state = 3;
				break;
			case 5:
				/* Give a work area to the default drive */
				f_mount(&FatFs, "", 0);

				/* Open a text file */
				if (!f_open(&fil, "derganqq.txt", FA_READ))
				{
					/* Read every line and display it */
					f_read(&fil, buffer, sizeof(buffer), &br);
					buffer[br] = 0x00;
					printf("%s\n", buffer);

					/* Close the file */
					f_close(&fil);
				}

				usb_state = 3;
				break;
			case 6:
				//write file
				/* Give a work area to the default drive */
				f_mount(&FatFs, "", 0);

				/* Open a text file */
				if (!f_open(&fil, "derganqq.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ))
				{
					/* Read every line and display it */
					f_write(&fil, booba_str, sizeof(booba_str), &bw);

					/* Close the file */
					f_close(&fil);
				}

				usb_state = 3;
				break;
			default: break;
    	}
    	//uint8_t set_feature_status = USBH_SetFeature(USB_REQ_FEAT_REMOTE_WAKEUP, 0x00);
    	//printf("set feature status: %02X\n", set_feature_status);

    	Delay_Ms(10);
    }
}

