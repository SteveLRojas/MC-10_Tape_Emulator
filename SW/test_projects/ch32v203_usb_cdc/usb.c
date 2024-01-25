/*
 * usb.c
 *
 *  Created on: Dec 2, 2023
 *      Author: Steve
 */
#include "debug.h"
#include "support.h"
#include "usb.h"

uint8_t timeout_flag = 0;
uint16_t bytes_read = 0;
unsigned int file_size;		//in bytes
unsigned int capacity;		//in sectors
unsigned int total_sectors;
unsigned int free_sectors;

uint8_t wait_status(uint8_t target, uint16_t timeout)
{
	uint8_t last_status = 0;
	timeout_flag = 0;

	while (timeout != 0)
	{
		Delay_Ms(1);
		last_status = usb_get_status();
		if(last_status == target)
			return 0x00;
		timeout--;
	}

	//printf("ERR: Timeout - target:%02X last:%02X \n", target, last_status);
	uart_print_string("Timeout!\nTarget: ");
	print_hex_byte(target);
	uart_print_string("Last: ");
	print_hex_byte(last_status);
	timeout_flag = 1;
	return last_status;
}

void usb_file_create()
{
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_FILE_CREATE); //file create command
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_byte_write()
{
	uint8_t* size = (uint8_t*)((void*)&file_size);
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_BYTE_WRITE); //byte write command
	Delay_Us(2);
	spi_transfer(SPI1, size[0]);  //length low byte
	Delay_Us(1);
	spi_transfer(SPI1, size[1]);	//length high byte
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_byte_wr_go()
{
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_BYTE_WR_GO);
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_write_test_data()
{
	uint16_t bytes_written;
	uint8_t bytes_requested;

	//usb_disk_query();
	if(free_sectors < (0x20000 >> 9))
	{
		printf("Not enough space\n");
		return;
	}

	random_init(0);

	usb_file_create();
	wait_status(0x14, 3000);
	if(timeout_flag)
		return;

	for(uint8_t d = 0; d < 4; ++d)	//WE WILL WRITE 4 BLOCKS OF 32KB
	{
		bytes_written = 0;
		file_size = 0x8000;	//set file size to 32 KB
		usb_byte_write();
		wait_status(0x1E, 3000);
		if(timeout_flag)
			return;

		while(1)
		{
			GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
			spi_transfer(SPI1, CMD_CH376S_WR_REQ_DATA); //WR_REQ_DATA command
			Delay_Us(2);
			bytes_requested = spi_transfer(SPI1, 0x00);

			while(bytes_requested)
			{
				Delay_Us(1);
				spi_transfer(SPI1, get_test_byte());
				++bytes_written;
				if(bytes_written == file_size)
					break;
				--bytes_requested;
			}
			GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
			wait_status(0x1E, 3000);
			if(timeout_flag)
				return;
			if(bytes_written == file_size)
				break;

			usb_byte_wr_go();
			wait_status(0x1E, 3000);
			if(timeout_flag)
				return;
		}
		usb_byte_wr_go();
		wait_status(0x14, 3000);
		if(timeout_flag)
			return;
	}

	usb_file_close();
	wait_status(0x14, 3000);
}

void usb_disk_capacity()
{
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_DISK_CAPACITY); //disk capacity command
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);

	wait_status(0x14, 3000);
	if(timeout_flag)
		return;

	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_RD_USB_DATA0); //RD_USB_DATA0 command
	Delay_Us(2);
	spi_transfer(SPI1, 0x00); //get number of bytes (always 4)
	for(uint8_t d = 0; d < 4; ++d)
	{
		Delay_Us(1);
		capacity = capacity >> 8;
		capacity = capacity | ((unsigned int)spi_transfer(SPI1, 0x00) << 24);
	}
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_disk_query()
{
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_DISK_QUERY); //disk query command
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);

	wait_status(0x14, 3000);
	if(timeout_flag)
		return;

	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_RD_USB_DATA0); //RD_USB_DATA0 command
	Delay_Us(2);
	spi_transfer(SPI1, 0x00); //get number of bytes (always 8)
	for(uint8_t d = 0; d < 4; ++d)
	{
		Delay_Us(1);
		total_sectors = total_sectors >> 8;
		total_sectors = total_sectors | ((unsigned int)spi_transfer(SPI1, 0x00) << 24);
	}
	for(uint8_t d = 0; d < 4; ++d)
	{
		Delay_Us(1);
		free_sectors = free_sectors >> 8;
		free_sectors = free_sectors | ((unsigned int)spi_transfer(SPI1, 0x00) << 24);
	}
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_reset_all()
{
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_RESET_ALL); //reset all
	GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_set_file_name()
{
	printf("input file name\n");

	char fname[20];
	uint8_t len = uart_get_string(fname, 20);
	--len;	//do not count the newline char
	fname[len] = '\0';	//replace the newline with null

	// The CH376S expects the name to be all caps.
	to_caps(fname);

	uint8_t count = 0;
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);  // enable chip
    spi_transfer(SPI1, CMD_CH376S_SET_FILE_NAME); //set file name
	Delay_Us(2);
    spi_transfer(SPI1, '/'); // "/" character at start of name
	while(fname[count])
	{
		Delay_Us(1);
		spi_transfer(SPI1, fname[count]);
		putchar(fname[count]);
		++count;
	}
	Delay_Us(1);
	spi_transfer(SPI1, '\0');
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
	putchar('\n');
	Delay_Ms(1);
}

void usb_file_open()
{
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);  // enable chip
    spi_transfer(SPI1, CMD_CH376S_FILE_OPEN); // file open
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_file_close()
{
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_FILE_CLOSE); // file close command
	Delay_Us(2);
    spi_transfer(SPI1, 0x01);  // update length or not
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_byte_read()
{
    uint8_t* size = (uint8_t*)((void*)&file_size);

    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_BYTE_READ); // byte read command
	Delay_Us(2);
    spi_transfer(SPI1, size[0]);
	Delay_Us(1);
    spi_transfer(SPI1, size[1]);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_byte_read_go()
{
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_BYTE_RD_GO);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_read_data0()
{
    uint8_t n_bytes = 0;
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_RD_USB_DATA0); // RD_USB_DATA0 command
	Delay_Us(2);
    n_bytes = spi_transfer(SPI1, CMD_CH376S_NOP);
    bytes_read = bytes_read + n_bytes;

    while (n_bytes)
    {
    	Delay_Us(1);
        putchar((char)spi_transfer(SPI1, CMD_CH376S_NOP));
        --n_bytes;
    }

    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
    putchar('\n');
}

void usb_file_read()
{
	bytes_read = 0;

	usb_file_open();
	wait_status(0x14, 100);
	if(timeout_flag)
		return;

	usb_get_file_size();
	wait_status(0x14, 100);
	if(timeout_flag)
		return;
	if(file_size == 0 || file_size > 65535)
	{
		printf("Got bad file size\n");
		return;
	}

	usb_byte_read();
	wait_status(0x1D, 100);
	if(timeout_flag)
		return;

	while(1)
	{
		usb_read_data0();
		printf("%d/%d\n", bytes_read, file_size);
		if(bytes_read == file_size)
			break;
		usb_byte_read_go();
		wait_status(0x1D, 255);
		if(timeout_flag)
			return;
	}

	usb_file_close();
	wait_status(0x14, 100);
}

void usb_get_file_size()
{
	uint8_t* size = (uint8_t*)((void*)&file_size);

    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_GET_FILE_SIZE); //get file size
	Delay_Us(2);
	spi_transfer(SPI1, 0x68); //command data
	Delay_Us(2);
	size[0] = spi_transfer(SPI1, CMD_CH376S_NOP);
	Delay_Us(1);
	size[1] = spi_transfer(SPI1, CMD_CH376S_NOP);
	Delay_Us(1);
	size[2] = spi_transfer(SPI1, CMD_CH376S_NOP);
	Delay_Us(1);
	size[3] = spi_transfer(SPI1, CMD_CH376S_NOP);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

uint8_t usb_check_exist(uint8_t test_data)
{
	uint8_t temp;
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_CHECK_EXIST);			//check exist
	Delay_Us(2);
	spi_transfer(SPI1, test_data);	//dummy data
	Delay_Us(2);
	temp = spi_transfer(SPI1, CMD_CH376S_NOP);	//should get inverted dummy data
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
	return temp;
}

uint8_t usb_get_version()
{
	uint8_t temp;
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_GET_IC_VER); //get version
	Delay_Us(2);
	temp = spi_transfer(SPI1, CMD_CH376S_NOP);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
	return temp;
}

void usb_set_mode(uint8_t mode)
{
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_SET_USB_MODE);	//set USB mode
	Delay_Us(2);
    spi_transfer(SPI1, mode);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_disk_connect()
{
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_DISK_CONNECT);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

void usb_disk_mount()
{
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_DISK_MOUNT);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

uint8_t usb_get_status()
{
    uint8_t temp;
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_GET_STATUS);
	Delay_Us(2);
    temp = spi_transfer(SPI1, CMD_CH376S_NOP);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
    return temp;
}

void usb_file_enum_go()
{
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
    spi_transfer(SPI1, CMD_CH376S_FILE_ENUM_GO);
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
}

// Enumerate the contents of the current directory
void usb_enumerate()
{
	uint8_t n_bytes = 0;
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
	spi_transfer(SPI1, CMD_CH376S_SET_FILE_NAME); // CMD_SET_FILE_NAME
	Delay_Us(2);
	spi_transfer(SPI1, '/');
	Delay_Us(1);
	spi_transfer(SPI1, '*');
	Delay_Us(1);
	spi_transfer(SPI1, '\0');
    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);

	usb_file_open();

	while(1)
	{
		wait_status(0x1D, 100); // Wait for USB_INT_DISK_READ
		if(timeout_flag)
			break;

	    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_RESET);
		spi_transfer(SPI1, CMD_CH376S_RD_USB_DATA0); //RD_USB_DATA0 command
		Delay_Us(2);
		n_bytes = spi_transfer(SPI1, CMD_CH376S_NOP);

		for(uint8_t d = 0; (d < 11) && n_bytes; ++d)
		{
			Delay_Us(1);
			putchar((char)spi_transfer(SPI1, CMD_CH376S_NOP));
			--n_bytes;
		}

		--n_bytes;
		Delay_Us(1);
		if(spi_transfer(SPI1, CMD_CH376S_NOP) & 0x10)
			printf(" (DIR)");
		else
			printf(" (FILE)");

		while(n_bytes)
		{
			Delay_Us(1);
			spi_transfer(SPI1, CMD_CH376S_NOP);
			--n_bytes;
		}

	    GPIO_WriteBit(PORT_CH376S_CTRL, PIN_CH376S_CTRL_CS, Bit_SET);
		putchar('\n');

		usb_file_enum_go();
	}
}
