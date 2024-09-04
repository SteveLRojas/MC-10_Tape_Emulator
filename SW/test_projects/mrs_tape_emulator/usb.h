/*
 * usb.h
 *
 *  Created on: Dec 2, 2023
 *      Author: Steve
 */

#ifndef USB_H_
#define USB_H_

#define PORT_CH376S_CTRL GPIOA  		// USB File System Chip

#define PIN_CH376S_CTRL_RST GPIO_Pin_14
#define PIN_CH376S_CTRL_CS GPIO_Pin_15


#define CMD_CH376S_NOP 			   0x00
#define CMD_CH376S_GET_IC_VER      0x01
#define CMD_CH376S_SET_BAUDRATE    0x02
#define CMD_CH376S_ENTER_SLEEP     0x03
#define CMD_CH376S_RESET_ALL       0x05
#define CMD_CH376S_CHECK_EXIST     0x06
#define CMD_CH376S_SET_SD0_INT     0x0B
#define CMD_CH376S_GET_FILE_SIZE   0x0C
#define CMD_CH376S_SET_USB_MODE    0x15
#define CMD_CH376S_GET_STATUS      0x22
#define CMD_CH376S_RD_USB_DATA0    0x27
#define CMD_CH376S_WR_USB_DATA     0x2C
#define CMD_CH376S_WR_REQ_DATA     0x2D
#define CMD_CH376S_WR_OFS_DATA     0x2E
#define CMD_CH376S_SET_FILE_NAME   0x2F
#define CMD_CH376S_DISK_CONNECT    0x30
#define CMD_CH376S_DISK_MOUNT      0x31
#define CMD_CH376S_FILE_OPEN       0x32
#define CMD_CH376S_FILE_ENUM_GO      0x33
#define CMD_CH376S_FILE_CREATE       0x34
#define CMD_CH376S_FILE_ERASE        0x35
#define CMD_CH376S_FILE_CLOSE        0x36
#define CMD_CH376S_DIR_INFO_READ     0x37
#define CMD_CH376S_DIR_INFO_SAVE     0x38
#define CMD_CH376S_BYTE_LOCATE       0x39
#define CMD_CH376S_BYTE_READ         0x3A
#define CMD_CH376S_BYTE_RD_GO        0x3B
#define CMD_CH376S_BYTE_WRITE        0x3C
#define CMD_CH376S_BYTE_WR_GO        0x3D
#define CMD_CH376S_DISK_CAPACITY     0x3E
#define CMD_CH376S_DISK_QUERY        0x3F
#define CMD_CH376S_DIR_CREATE        0x40
#define CMD_CH376S_SEG_LOCATE        0x4A
#define CMD_CH376S_SEC_READ          0x4B
#define CMD_CH376S_SEC_WRITE         0x4C
#define CMD_CH376S_DISK_BOC_CMD      0x50
#define CMD_CH376S_DISK_READ         0x54
#define CMD_CH376S_DISK_RD_GO        0x55
#define CMD_CH376S_DISK_WRITE        0x56
#define CMD_CH376S_DISK_WR_GO        0x57


extern uint8_t timeout_flag;
extern unsigned int file_size;	//file size in bytes
extern unsigned int capacity;	//drive capacity in sectors
extern unsigned int total_sectors;	//total sectors in partition
extern unsigned int free_sectors;	//free sectors in partition


uint8_t wait_status(uint8_t target, uint16_t timeout);
void usb_file_create();
void usb_byte_write();
void usb_byte_wr_go();
void usb_write_test_data();
void usb_disk_capacity();
void usb_disk_query();
void usb_reset_all();
void usb_set_file_name();
void usb_file_open();
void usb_file_close();
void usb_byte_read();
void usb_byte_read_go();
void usb_read_data0();
void usb_file_read();
void usb_get_file_size();
uint8_t usb_check_exist(uint8_t);
uint8_t usb_get_version();
void usb_set_mode(uint8_t);
void usb_disk_connect();
void usb_disk_mount();
uint8_t usb_get_status();
void usb_file_enum_go();
void usb_enumerate();

#endif /* USB_H_ */
