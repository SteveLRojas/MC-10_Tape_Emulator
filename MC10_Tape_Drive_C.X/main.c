/*
 * File:   main.c
 * Author: Steve
 *
 * Created on August 25, 2022, 1:44 PM
 */
// CONFIG1
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // MCLR/VPP/RE3 Pin Function Select bit (MCLR/VPP/RE3 pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (Enabled)
#pragma config BORV = 42        // Brown-out Reset Voltage bits (VBOR set to 4.2V)
#pragma config CCP2MX = RC1     // CCP2 Multiplex bit (CCP2 is on RC1)
#pragma config CP = OFF         // Flash Program Memory Code Protection bits (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)
#pragma config BORSEN = OFF     // Brown-out Reset Software Enable bit (Disabled)

#define MHz *1000000UL
#define _XTAL_FREQ (4 MHz)

#include <xc.h>
#include <pic16f747.h>

//ASM globals
//unsigned char dreg;
unsigned char temp1;
unsigned char temp2;
unsigned char temp3;
unsigned char temp4;
unsigned char temp5;
unsigned char ram_address_h;
unsigned char ram_address_l;
unsigned char sdel_count;
unsigned char ldel_count;
unsigned char gen_count;
unsigned char gen_count2;
unsigned char gen_count3;
unsigned char gen_count4;
unsigned char gen_count5;
unsigned char gen_count6;
unsigned char tape_size_high;
unsigned char tape_size_low;
unsigned char bit_count;
unsigned char hex_char_high;
unsigned char hex_char_low;
unsigned char usb_bytes_sent_high;
unsigned char usb_bytes_sent_low;
unsigned char usb_file_size_high;
unsigned char usb_file_size_low;
unsigned char usb_bytes_read_high;
unsigned char usb_bytes_read_low;
unsigned char usb_drive_size1;
unsigned char usb_drive_size2;
unsigned char usb_drive_size3;
unsigned char usb_free_space1;
unsigned char usb_free_space2;
unsigned char usb_free_space3;
unsigned char usb_logical_bytes1;
unsigned char usb_logical_bytes2;
unsigned char usb_logical_bytes3;
unsigned char usb_target_status;
unsigned char s_reg_high;
unsigned char s_reg_low;
unsigned char timeout_high;
unsigned char timeout_low;
unsigned char delay_counter;
unsigned char flag_reg;
unsigned char fifo_read_idx;
unsigned char fifo_read_bank;
unsigned char fifo_write_idx;
unsigned char fifo_write_bank;
unsigned char fifo_num_bytes;

unsigned char tape_name[8] = {'D', 'E', 'R', 'G', 'A', 'N', 'Q', 'Q'};

//ASM function declarations
//void interrupt tape_int(void);
void initialize();
void mem_clear();
void mem_dump();
void file_dump();
unsigned char uart_read();
void uart_send(unsigned char);
void lcd_putc(unsigned char);
void lcd_line2();
void lcd_clear2();
void print_name();
void delay_millis(unsigned char);
void print_test_data();
void usb_reset_all();
void usb_set_file_name();
void usb_file_open();
void usb_file_close();
void usb_get_file_size();
unsigned char usb_check_exists(unsigned char);
void usb_file_create();
unsigned char usb_get_status();
void usb_disk_mount();
void usb_disk_connect();
void usb_set_mode(unsigned char);
unsigned char usb_get_version();
void usb_disk_capacity();
void usb_disk_query();
void usb_write_test_data();
void usb_file_write();
unsigned char usb_file_read();

//C globals
unsigned char fifo_bank_A[64];
unsigned char fifo_bank_B[96];
unsigned char fifo_bank_C[96];

const char str_timeout[] = "Timeout: ";
const char str_cmd_mode[] = "Command mode\n";

const unsigned char hex_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


unsigned char fifo_push(unsigned char data)
{
    switch(fifo_write_bank)
    {
        case 0:
        case 1:
            fifo_bank_A[fifo_write_idx] = data;
            break;
        case 2:
        case 3:
        case 4:
            fifo_bank_B[fifo_write_idx - 64] = data;
            break;
        case 5:
        case 6:
        case 7:
            fifo_bank_C[fifo_write_idx - (64 + 96)] = data;
            break;
    }
    ++fifo_write_idx;
    if(!(fifo_write_idx & 0x1F))
    {
        ++fifo_write_bank;
    }
    ++fifo_num_bytes;
    return fifo_num_bytes;
}

unsigned char fifo_pop(void)
{
    unsigned char temp;
    switch(fifo_read_bank)
    {
        case 0:
        case 1:
            temp = fifo_bank_A[fifo_read_idx];
            break;
        case 2:
        case 3:
        case 4:
            temp = fifo_bank_B[fifo_read_idx - 64];
            break;
        case 5:
        case 6:
        case 7:
            temp = fifo_bank_C[fifo_read_idx - (64 + 96)];
            break;
    }
    ++fifo_read_idx;
    if(!(fifo_read_idx & 0x1F))
    {
        ++fifo_read_bank;
    }
    --fifo_num_bytes;
    return temp;
}

void byte_to_hex(unsigned char data)
{
    hex_char_low = hex_table[data & (unsigned char)0x0F];
    data = data >> 4;
    hex_char_high = hex_table[data & (unsigned char)0x0F];
    return;
}

void print_string_lcd(const char* string)
{
    unsigned char i = 0;
    while(string[i])
    {
        lcd_putc(string[i]);
        ++i;
    }
    return;
}

void print_string_serial(const char* string)
{
    unsigned char i = 0;
    while(string[i])
    {
        uart_send(string[i]);
        ++i;
    }
    return;
}

void timeout_error()
{
    print_string_serial(str_timeout);
    byte_to_hex(usb_target_status);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    uart_send('\n');
    return;
}

void print_capacity()
{
    usb_disk_capacity();
    print_string_serial("Physical bytes: ");
    byte_to_hex(usb_drive_size3);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    byte_to_hex(usb_drive_size2);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    byte_to_hex(usb_drive_size1);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    uart_send('0');
    uart_send('0');
    uart_send('\n');
    return; 
}
void print_query()
{
    usb_disk_query();
    print_string_serial("Logical bytes: ");
    byte_to_hex(usb_logical_bytes3);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    byte_to_hex(usb_logical_bytes2);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    byte_to_hex(usb_logical_bytes1);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    uart_send('0');
    uart_send('0');
    uart_send('\n');
    print_string_serial("Free bytes: ");
    byte_to_hex(usb_free_space3);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    byte_to_hex(usb_free_space2);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    byte_to_hex(usb_free_space1);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    uart_send('0');
    uart_send('0');
    uart_send('\n');
    return;
}

void usb_autoconfig()
{
    unsigned char temp;
    print_string_serial("USB Autoconfig:\n");
    temp = usb_check_exists(0x55);
    byte_to_hex(temp);
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    uart_send('\n');
  
    temp = usb_get_version();
    byte_to_hex(temp);
    print_string_serial("Version: ");
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    uart_send('\n');

    usb_set_mode(0x05);
    delay_millis(1);
    usb_set_mode(0x07);
    delay_millis(1);
    usb_set_mode(0x06);
    delay_millis(1);
    usb_disk_connect();
    delay_millis(250);
    delay_millis(250);
    usb_disk_mount();
    delay_millis(250);
    delay_millis(250);
  
    temp = usb_get_status();
    byte_to_hex(temp);
    print_string_serial("Status: ");
    uart_send(hex_char_high);
    uart_send(hex_char_low);
    uart_send('\n');
  
    print_query();
    return;
}

void main(void)
{
    unsigned char state = 0;
    initialize();
    //usb_reset_all();
    //delay_millis(100);
    //usb_autoconfig();
    //print_name();

    print_string_lcd("Dragons");
    lcd_line2();

    print_string_serial("deadbeef\n");
    
    unsigned char temp;
    while(1)
    {
        temp = uart_read();
        
        switch(state)
        {
            case 0:
                if(temp == 0xDE)
                    state = 1;
                break;
            case 1:
                if(temp == 0xAD)
                    state = 2;
                else
                    state = 0;
                break;
            case 2:
                if(temp == 0xBE)
                    state = 3;
                else
                    state = 0;
                break;
            case 3:
                if(temp == 0xEF)
                    print_string_serial(str_cmd_mode);
                state = 4;
                break;
            case 4:
                switch(temp)
                {
                    case 0x00:
                        usb_reset_all();
                        break;
                    case 0x01:
                        usb_autoconfig();
                        break;
                    case 0x02:
                        temp = usb_get_version();
                        break;
                    case 0x03:
                        usb_set_mode(0x05);
                        break;
                    case 0x04:
                        usb_set_mode(0x06);
                        break;
                    case 0x05:
                        usb_set_mode(0x07);
                        break;
                    case 0x06:
                        usb_disk_connect();
                        break;
                    case 0x07:
                        usb_disk_mount();
                        break;
                    case 0x08:
                        temp = usb_get_status();
                        break;
                    case 0x09:
                        print_query();
                        break;
                    case 0x0A:
                        print_capacity();
                        break;
                    case 0x0B:
                        usb_set_file_name();
                        print_name();
                        break;
                    case 0x0C:
                        usb_file_open();
                        break;
                    case 0x0D:
                        usb_file_close();
                        break;
                    case 0x0E:
                        usb_get_file_size();
                        print_string_serial("File size: ");
                        byte_to_hex(usb_file_size_high);
                        uart_send(hex_char_high);
                        uart_send(hex_char_low);
                        byte_to_hex(usb_file_size_low);
                        uart_send(hex_char_high);
                        uart_send(hex_char_low);
                        uart_send('\n');
                        break;
                    case 0x0F:
                        temp = usb_file_read();
                        break;
                    case 0x10:
                        usb_file_create();
                        break;
                    case 0x11:
                        usb_file_write();
                        break;
                    case 0x12:
                        usb_write_test_data();
                        break;
                    case 0x20:
                        mem_clear();
                        break;
                    case 0x21:
                        mem_dump();
                        break;
                    case 0x22:
                        file_dump();
                        break;
                    case 0x23:
                        print_test_data();
                        break;
                    case 0x24:
                        usb_file_size_low = 16;
                        usb_file_size_high = 0;
                        file_dump();
                }
                byte_to_hex(temp);
                uart_send(hex_char_high);
                uart_send(hex_char_low);
                lcd_clear2();
                lcd_putc(hex_char_high);
                lcd_putc(hex_char_low);
                break;
        }
    }
    return;
}
