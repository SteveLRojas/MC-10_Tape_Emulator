/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "support.h"
#include "display.h"
#include "usb.h"

/* Global define */

/* Global Variable */

void gpio_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure = {0};
	SPI_InitTypeDef SPI_InitStructure={0};

	// Init PORTA (LCD_BUS)
	GPIO_InitStructure.GPIO_Pin = (0x00FF);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_LCD_BUS, &GPIO_InitStructure);

	// Init PORTB (LCD_CTRL)
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1 | PIN_LCD_CTRL_RW | PIN_LCD_CTRL_E | PIN_LCD_CTRL_RS);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_LCD_CTRL, &GPIO_InitStructure);

	// Init display control signals to 0
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RW, Bit_RESET);
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_E, Bit_RESET);
	GPIO_WriteBit(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, Bit_RESET);

	//USB/SPI stuff
	//RST, CS pin
	GPIO_InitStructure.GPIO_Pin = (PIN_CH376S_CTRL_RST | PIN_CH376S_CTRL_CS);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_CH376S_CTRL, &GPIO_InitStructure);

	//SCK, MOSI pin
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_3 | GPIO_Pin_5);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//MISO pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;	//TODO: adjust
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_RST, Bit_RESET);

	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
}

void serial_listen();
void usb_autoconfig();
void usb_print_disk_info();

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint8_t i = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    gpio_init();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk: %d \n", SystemCoreClock);
    printf("Booba\n");

    //Reset USB chip
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_RST, Bit_SET);
    Delay_Ms(1);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_RST, Bit_RESET);
    Delay_Ms(40);

    // USB/SPI test code
    uint8_t res = usb_check_exist(0x55);
    printf("usb_check_exist res: %02X \n", res);
    Delay_Ms(1);

    res = usb_get_version();
    printf("usb_get_version res: %02X \n", res);

    // LCD test code?
    lcd_init();
    lcd_print_string("Hello World!");
    lcd_cursor_pos(1, 0);
    lcd_print_string("I am RISC-V");
    lcd_cursor_pos(2, 0);
    lcd_print_string("Line 2");
    lcd_cursor_pos(3, 0);
    lcd_print_string("Line 3");

    lcd_cursor_pos(0, 13);
    lcd_print_string("Test0");
    lcd_cursor_pos(1, 12);
    lcd_print_string("Test1");
    lcd_cursor_pos(2, 10);
    lcd_print_string("Test2");
    lcd_cursor_pos(3, 10);
    lcd_print_string("Test3");

    lcd_cursor_pos(2, 7);

//    printf("Input a string\n");
//    char fname[20];
//    fname[0] = 'A';
//    fname[1] = 'B';
//    fname[2] = 'C';
//    fname[3] = 0x00;
//	uint8_t len = uart_get_string(fname, 20);
//	--len;	//do not count the newline char
//	fname[len] = '\0';	//replace the newline with null
//	to_caps(fname);
//    printf("Got string: %s\n", fname);
//    printf("Got %d chars\n", len);

    while(1)
    {
        //Delay_Ms(250);
    	serial_listen();
        GPIO_WriteBit(GPIOB, GPIO_Pin_1, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
    }
}

void serial_listen()
{
	//uint8_t temp;

	switch(get_hex())
	{
		case 0x00:
			printf("check exist: ");
			fflush(stdout);
			print_hex_byte(usb_check_exist(0x55));
			break;
		case 0x01:
			printf("get version: ");
			fflush(stdout);
			print_hex_byte(usb_get_version());
			break;
		case 0x02:
			printf("set USB mode 5\n");
			usb_set_mode(0x05);
			break;
		case 0x03:
			printf("set USB mode 7\n");
			usb_set_mode(0x07);
			break;
		case 0x04:
			printf("set USB mode 6\n");
			usb_set_mode(0x06);
			break;
		case 0x05:
			printf("CMD_DISK_CONNECT\n");
			usb_disk_connect();
			break;
		case 0x06:
			printf("CMD_DISK_MOUNT\n");
			usb_disk_mount();
			break;
		case 0x07:
			printf("get status: ");
			fflush(stdout);
			print_hex_byte(usb_get_status());
			break;
		case 0x08:
			printf("usb set file name: \n");
			usb_set_file_name();
			break;
		case 0x09:
			printf("CMD_FILE_OPEN\n");
			usb_file_open();
			break;
		case 0x0A:
			printf("CMD_GET_FILE_SIZE: ");
			fflush(stdout);
			usb_get_file_size();
			printf("%d\n", file_size);
			break;
		case 0x0B:
			printf("CMD_FILE_CLOSE\n");
			usb_file_close();
			break;
		case 0x0C:
			printf("CMD_BYTE_READ\n");
			usb_byte_read();
			break;
		case 0x0D:
			printf("CMD_BYTE_RD_GO\n");
			usb_byte_read_go();
			break;
		case 0x0E:
			printf("CMD_RD_USB_DATA0\n");
			usb_read_data0();
			break;
		case 0x0F:
			printf("USB Autoconfig:\n");
			usb_autoconfig();
			break;
		case 0x10:
			printf("USB reset all\n");
			usb_reset_all();
			break;
		case 0x11:
			printf("usb_file_read:\n");
			usb_file_read();
			break;
		case 0x12:
			printf("CMD_DISK_CAPACITY (KB)\n");
			usb_disk_capacity();
			printf("%d\n", capacity >> 1);	//convert sectors to KB
			break;
		case 0x13:
			printf("CMD_DISK_QUERY\n");
			usb_disk_query();
			printf("logical KBs: ");
			printf("%d\n", total_sectors >> 1);
			printf("free KBs: ");
			printf("%d\n", free_sectors >> 1);
			break;
		case 0x14:
			printf("print test data: \n");
			print_test_data();
			break;
		case 0x15:
			printf("CMD_FILE_CREATE\n");
			usb_file_create();
			break;
		case 0x16:
			printf("write USB test data\n");
			usb_write_test_data();
			break;
		case 0x17:
			printf("file enum go\n");
			usb_file_enum_go();
			break;
		case 0x18:
			printf("USB enumerate\n");
			usb_enumerate();
			break;
		case 0x19:
			printf("USB print disk info\n");
			usb_print_disk_info();
			break;
//		case 0x80:
//			printf("Input byte\n");
//			temp = get_hex();
//			GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
//			print_hex_byte(spi_transfer(SPI1, temp));
//			Delay_Us(100);
//			print_hex_byte(spi_transfer(SPI1, 0x00));
//			GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
//			break;
	}
	Delay_Ms(100);
	printf("Ready.\n");
}

void usb_print_disk_info()
{
	printf("capacity in KBs: ");
	printf("%d\n", capacity >> 1);	//convert sectors to KB
	printf("logical KBs: ");
	printf("%d\n", total_sectors >> 1);
	printf("free KBs: ");
	printf("%d\n", free_sectors >> 1);
}

void usb_autoconfig()
{
	uart_print_string("check exist: ");
	print_hex_byte(usb_check_exist(0x55));
	uart_print_string("get version: ");
	print_hex_byte(usb_get_version());

	printf("Setting mode 0x05\n");
	usb_set_mode(0x05);
	Delay_Us(10);

	printf("Setting mode 0x07\n");
	usb_set_mode(0x07);
	Delay_Us(10);

	printf("Setting mode 0x06\n");
	usb_set_mode(0x06);
	Delay_Us(10);

	printf("usb_disk_connect\n");
	usb_disk_connect();
	wait_status(0x14, 3000);

	printf("usb_disk_mount\n");
	usb_disk_mount();
	wait_status(0x14, 3000);

	printf("usb_get_status\n");
	print_hex_byte(usb_get_status());

	printf("usb_disk_capacity\n");
	usb_disk_capacity();

	printf("usb_disk_query\n");
	usb_disk_query();

	printf("usb_print_disk_info\n");
	usb_print_disk_info();
}







