#include <xc.inc>
#define IRP 7
    
#define DISP_E PORTA, 2
#define DISP_RS PORTA, 3
#define USB_INT PORTB, 0
#define TAPE_IN PORTB, 1
#define TAPE_OUT PORTB, 2
#define SW0 PORTB, 3
#define SW1 PORTB, 4
#define SW2 PORTB, 5
#define SW3 PORTB, 6
#define RAM_CS PORTB, 7
#define USB_CS PORTD, 2
#define USB_RST PORTD, 3
#define DISP_CLK PORTD, 6
#define DISP_DATA PORTD, 7
    
;PIN USAGE:
; PORTA,0 UNUSED OUTPUT
; PORTA,1 UNUSED OUTPUT
; PORTA,2 DISPLAY CE OUTPUT
; PORTA,3 DISPLAY RS OUTPUT
; PORTA,4 UNUSED (OPEN DRAIN) OUTPUT
; PORTA,5 UNUSED OUTPUT
; PORTA,6 CLOCK INPUT
; PORTA,7 CLOCK INPUT
; PORTB,0 CH376S INTERRUPT INPUT
; PORTB,1 TAPE INPUT
; PORTB,2 TAPE OUTPUT
; PORTB,3 BUTTON 1 INPUT
; PORTB,4 BUTTON 2 INPUT
; PORTB,5 BUTTON 3 INPUT
; PORTB,6 BUTTON 4 INPUT    ICSP CLOCK
; PORTB,7 RAM CE OUTPUT     ICSP DATA
; PORTC,0 UNUSED OUTPUT
; PORTC,1 UNUSED OUTPUT
; PORTC,2 UNUSED OUTPUT
; PORTC,3 SCK (HARDWARE SPI) OUTPUT
; PORTC,4 SDI (HARDWARE SPI) INPUT
; PORTC,5 SDO (HARDWARE SPI) OUTPUT
; PORTC,6 TXD (HARDWARE UART) INPUT (MUST BE CONFIGURED AS INPUT FOR UART MODULE TO WORK. THIS IS ACTUALLY AN OUTPUT)
; PORTC,7 RXD (HARDWARE UART) INPUT
; PORTD,0 UNUSED OUTPUT
; PORTD,1 UNUSED OUTPUT
; PORTD,2 CH376S CE OUTPUT
; PORTD,3 CH376S RESET OUTPUT
; PORTD,4 TEST OUTPUT
; PORTD,5 UNUSED OUTPUT
; PORTD,6 DISPLAY CLOCK OUTPUT
; PORTD,7 DISPLAY DATA OUTPUT
; PORTE,0 UNUSED OUTPUT
; PORTE,1 UNUSED OUTPUT
; PORTE,2 UNUSED OUTPUT

; When assembly code is placed in a psect, it can be manipulated as a
; whole by the linker and placed in memory.  
;
; In this example, barfunc is the program section (psect) name, 'local' means
; that the section will not be combined with other sections even if they have
; the same name.  class=CODE means the barfunc must go in the CODE container.
; PIC18s should have a delta (addressible unit size) of 1 (default) since they
; are byte addressible.  PIC10/12/16s have a delta of 2 since they are word
; addressible.  PIC18s should have a reloc (alignment) flag of 2 for any
; psect which contains executable code.  PIC10/12/16s can use the default
; reloc value of 1.  Use one of the psects below for the device you use:

psect   barfunc,local,class=CODE,delta=2 ; PIC10/12/16
; psect   barfunc,local,class=CODE,reloc=2 ; PIC18
    
global _temp3
global _temp4
global _temp5
global _ram_address_h
global _ram_address_l
global _sdel_count
global _ldel_count
global _gen_count
global _gen_count2
global _gen_count3
global _gen_count4
global _tape_size_high
global _tape_size_low
global _hex_char_high
global _hex_char_low
global _usb_file_size_high
global _usb_file_size_low
global _usb_bytes_read_high
global _usb_bytes_read_low
global _usb_drive_size1
global _usb_drive_size2
global _usb_drive_size3
global _usb_free_space1
global _usb_free_space2
global _usb_free_space3
global _usb_logical_bytes1
global _usb_logical_bytes2
global _usb_logical_bytes3
global _usb_target_status
global _s_reg_high
global _s_reg_low
global _timeout_high
global _timeout_low
global _delay_counter
global _flag_reg
global _fifo_read_idx
global _fifo_read_bank
global _fifo_write_idx
global _fifo_write_bank
global _fifo_num_bytes

global _tape_name


;extern entry__byte_to_hex
global _byte_to_hex
global _fifo_push
global _fifo_pop

 
global _initialize
_initialize
    BANKSEL PORTA
    CLRF PORTA
    CLRF PORTB
    CLRF PORTC
    CLRF PORTD
    COMF PORTB, F
    COMF PORTC, F
    COMF PORTD, F
    BANKSEL TRISA  ;SWITCH TO BANK 1
    MOVLW 0b11000000
    MOVWF TRISA	;CONFIGURE PORTA
    MOVLW 0b01111011
    MOVWF TRISB ;CONFIGURE PORTB
    MOVLW 0b11010000
    MOVWF TRISC ;CONFIGURE PORTC
    MOVLW 0b00000000
    MOVWF TRISD	;CONFIGURE PORTD
    MOVLW 0b11111000
    MOVWF TRISE	;CONFIGURE PORTE
    MOVLW 0b00000000	    ;SMP=0, CKE=0, DA=0, P=0, S=0, RW=0, UA=0, BF=0
    MOVWF SSPSTAT ;CONFIGURE SPI
    MOVLW 0b00001111	    ;PCFG=1111
    MOVWF ADCON1 ;DISABLE ANALOG INPUTS
    MOVLW 0b00100110	;CSRC=0, TX9=0, TXEN=1, SYNC=0, UNIMPLEMENTED=0, BRGH=1, TRMT=1, TX9D=0
    MOVWF TXSTA ;CONFIGURE UART
    MOVLW 25
    MOVWF SPBRG ;CONFIGURE BAUD RATE
    MOVLW 0b11011000	;RBPU = 1, INTEDG = 1, T0CS = 0, T0SE = 1, PSA = 1, PS2 = 0, PS1 = 0, PS0 = 0
    MOVWF OPTION_REG	;CONFIGURE TIMER
    BANKSEL SSPCON
    MOVLW 0b00110000	;WCOL=0, SSPOV=0, SSPEN=1, CKP=1, SSPM=0000
    MOVWF SSPCON ;CONFIGURE SPI
    MOVLW 0b10010000	;SPEN=1, RX9=0,SREN=0, CREN=1, ADDEN=0, FERR=0, OERR=0, RX9D=0
    MOVWF RCSTA ;CONFIGURE UART
    MOVLW 0b00100000
    MOVWF INTCON    ;ENABLE TIMER 0 INTERRUPT
    CLRF _flag_reg
    CLRF _s_reg_high
    CLRF _s_reg_low
    MOVLW 100
    CALL _delay_millis   ;100 MS DELAY AT STARTUP
    BCF USB_RST
    CALL RAM_CONFIG
    CLRF _ram_address_h
    CLRF _ram_address_l
    MOVLW 0x55
    CALL RAM_WRITE
    CALL FUNCTIONSET_LCD    ;INITIALIZE DISPLAY
    CALL DISPLAYON  ;POWER ON THE DISPLAY
    CALL CLEARDISPLAY	;CLEAR THE DISPLAY
    CALL SETHOME_LCD	;SET CURSOR TO THE START OF LINE 1
    RETURN

global _uart_read
_uart_read
    BTFSS RCIF    ;CHECK IF UART RECEIVE BUFFER IS FULL
    GOTO _uart_read  ;IF NOT WAIT UNTIL ITS FULL
    MOVF RCREG, W   ;COPY DATA TO W
    RETURN

global _uart_send
_uart_send
    BTFSS TXIF    ;CHECK IF UART TRANSMIT BUFFER IS EMPTY
    GOTO _uart_send  ;IF NOT WAIT UNTIL IT IS
    MOVWF TXREG	    ;COPY DATA TO TXREG
    RETURN

RAM_READ_AUTO
    INCFSZ _ram_address_l, F    ;INCREMENT LOW ADDRESS
    DECF _ram_address_h, F
    INCF _ram_address_h, F	;IF LOW ADDRESS IS ZERO INCREMENT THE HIGH ADDRESS TOO
RAM_READ
    BCF RAM_CS    ;CHIP ENABLE
    MOVLW 0x03	;PUT WRITE COMMAND IN W
    MOVWF SSPBUF    ;SEND COMMAND
    MOVF _ram_address_h, W   ;COPY ADDRESS TO W
    BANKSEL SSPSTAT
RAM_WAIT_AH
    BTFSS BF	;IS THE TRANSMISSION READY
    GOTO RAM_WAIT_AH	;IF NOT WAIT UNTIL IT IS
    BANKSEL SSPBUF
    MOVWF SSPBUF    ;SEND ADDRESS HIGH BYTE
    MOVF _ram_address_l, W   ;COPY ADDRESS TO W
    BANKSEL SSPSTAT
RAM_WAIT_AL
    BTFSS BF
    GOTO RAM_WAIT_AL
    BANKSEL SSPBUF
    MOVWF SSPBUF    ;SEND ADDRESS LOW BYTE
    BANKSEL SSPSTAT
RAM_WAIT_D
    BTFSS BF
    GOTO RAM_WAIT_D
    BANKSEL SSPBUF
    CLRF SSPBUF
    BANKSEL SSPSTAT
RAM_WAIT_R
    BTFSS BF
    GOTO RAM_WAIT_R
    BANKSEL SSPBUF
    MOVF SSPBUF, W  ;COPY DATA TO W
    BSF RAM_CS
    RETURN

RAM_WRITE_AUTO
    MOVWF _temp5
    INCFSZ _ram_address_l, F    ;INCREMENT LOW ADDRESS
    DECF _ram_address_h, F
    INCF _ram_address_h, F	;IF LOW ADDRESS IS ZERO INCREMENT THE HIGH ADDRESS TOO
RAM_WRITE
    BCF RAM_CS    ;CHIP ENABLE
    MOVLW 0x02	;PUT WRITE COMMAND IN W
    MOVWF SSPBUF    ;SEND COMMAND
    MOVF _ram_address_h, W   ;COPY ADDRESS TO W
    BANKSEL SSPSTAT
RAM_WRITE_AH
    BTFSS BF	;IS THE TRANSMISSION READY
    GOTO RAM_WRITE_AH	;IF NOT WAIT UNTIL IT IS
    BANKSEL SSPBUF
    MOVWF SSPBUF    ;SEND ADDRESS HIGH BYTE
    MOVF _ram_address_l, W   ;COPY ADDRESS TO W
    BANKSEL SSPSTAT
RAM_WRITE_AL
    BTFSS BF
    GOTO RAM_WRITE_AL
    BANKSEL SSPBUF
    MOVWF SSPBUF    ;SEND ADDRESS LOW BYTE
    MOVF _temp5, W    ;COPY DATA TO W
    BANKSEL SSPSTAT
RAM_WRITE_D
    BTFSS BF
    GOTO RAM_WRITE_D
    BANKSEL SSPBUF
    MOVWF SSPBUF    ;SEND DATA
    BANKSEL SSPSTAT
RAM_WRITE_R
    BTFSS BF
    GOTO RAM_WRITE_R
    BANKSEL SSPBUF
    MOVF SSPBUF, W  ;COPY DUMMY DATA TO W
    BSF RAM_CS
    RETURN
    
RAM_CONFIG
    BCF RAM_CS    ;CHIP ENABLE
    MOVLW 0x01	;PUT WRITE COMMAND IN W
    MOVWF SSPBUF    ;SEND COMMAND
    MOVLW 0x00	;CONFIGURE FOR BYTE MODE
    BANKSEL SSPSTAT
RAM_CONFIG_C
    BTFSS BF	;IS THE TRANSMISSION READY
    GOTO RAM_CONFIG_C	;IF NOT WAIT UNTIL IT IS
    BANKSEL SSPBUF
    MOVWF SSPBUF    ;SEND CONFIG DATA
    BANKSEL SSPSTAT
RAM_CONFIG_D
    BTFSS BF
    GOTO RAM_CONFIG_D
    BANKSEL SSPBUF
    MOVF SSPBUF, W  ;COPY DUMMY DATA TO W
    BSF RAM_CS
    RETURN

global _lcd_putc
_lcd_putc
    BSF DISP_RS	;RS LINE TO 1 (PORT A BIT 3)
    CALL _lcd_send
    CALL _pulse_e
    MOVLW 0x02
    CALL _delay_millis
    RETURN
    
global _lcd_send
_lcd_send    ;SENDS DATA OR COMMAND TO LCD SHIFT REGISTER
    MOVWF _temp3	;COPY DATA TO TEMP VARIABLE
    MOVLW 0x08
    MOVWF _gen_count3	;COUNT 8 BITS
LCD_SEND_LOOP
    BTFSC _temp3,7
    BSF DISP_DATA
    BTFSS _temp3,7
    BCF DISP_DATA
    RLF _temp3,F
    BSF DISP_CLK
    NOP
    NOP
    BCF DISP_CLK
    DECFSZ _gen_count3,F
    GOTO LCD_SEND_LOOP
    RETURN

FUNCTIONSET_LCD
    BCF DISP_E	;MAKE SURE THAT E STARTS LOW
    BCF DISP_CLK     ;MAKE SURE DISPLAY CLOCK STARTS LOW
    BCF DISP_RS	;RS LINE TO 0 (PORT A BIT 3)
    MOVLW 0b00111000	;FUNCTION SET COMMAND
    CALL _lcd_send
    CALL _pulse_e    ;PULSE E LINE HIGH (PORT A BIT 2)
    MOVLW 10
    CALL _delay_millis
    RETURN

DISPLAYON
    BCF DISP_RS   ;RS LINE TO 0 (PORT A BIT 3)
    MOVLW 0x0F  ;DISPLAY ON/OFF AND CURSOR COMMAND
    CALL _lcd_send
    CALL _pulse_e
    MOVLW 10
    CALL _delay_millis
    RETURN

CLEARDISPLAY
    BCF DISP_RS	;RS LINE LOW
    MOVLW 0x01	;CLEAR DISPLAY COMMAND
    CALL _lcd_send	;WRITE TO DATA LINES
    CALL _pulse_e
    MOVLW 10
    CALL _delay_millis
    RETURN

SETHOME_LCD
    BCF DISP_RS ;RS LINE LOW
    MOVLW 0x03   ;DISPLAY AND CURSOR HOME COMMAND
    CALL _lcd_send ;WRITE TO DATA LINES
    CALL _pulse_e
    MOVLW 5
    CALL _delay_millis
    RETURN

global _lcd_line2
_lcd_line2
    CALL SETHOME_LCD
    MOVLW 28H   ;SET COUNTER TO 40
    MOVWF _gen_count2
    MOVLW 0b00010000   ;CURSOR LEFT COMMAND
    CALL _lcd_send ;WRITE COMMAND TO DATA LINES
CLLOOP40
    BCF DISP_RS	;RS LINE LOW (COMMAND MODE)
    CALL _pulse_e
    MOVLW 2
    CALL _delay_millis
    DECFSZ _gen_count2, F
    GOTO CLLOOP40
    RETURN

LCD_CLEAR1
    CALL SETHOME_LCD
    MOVLW 0x20 ;SPACE CHAR
    BSF DISP_RS ;RS LINE HIGH FOR DATA
    CALL _lcd_send	;PUT CHAR ON THE DATA LINES
    MOVLW 20 ;COUNT 20 SPACE CHARS
    MOVWF _gen_count2
LCD_CLEAR12
    CALL _pulse_e
    MOVLW 2
    CALL _delay_millis
    DECFSZ _gen_count2, F
    GOTO LCD_CLEAR12
    CALL SETHOME_LCD
    RETURN

global _lcd_clear2
_lcd_clear2
    CALL _lcd_line2
    MOVLW 0x20 ;SPACE CHAR
    BSF DISP_RS ;RS LINE HIGH FOR DATA
    CALL _lcd_send	;PUT CHAR ON THE DATA LINES
    MOVLW 20 ;COUNT 20 SPACE CHARS
    MOVWF _gen_count2
LCD_CLEAR22
    CALL _pulse_e
    MOVLW 2
    CALL _delay_millis
    DECFSZ _gen_count2, F
    GOTO LCD_CLEAR22
    CALL _lcd_line2
    RETURN

global _pulse_e
_pulse_e
    BSF DISP_E   ;SET E LINE HIGH
    NOP
    NOP
    NOP
    NOP
    NOP ;MODIFIED FOR 4MHZ CLOCK
    NOP
    NOP
    NOP
    BCF DISP_E   ;SET E LINE LOW
    RETURN

global _print_name
_print_name
    BCF STATUS, IRP ;SET BANK 0 AND 1
    MOVLW _tape_name
    MOVWF FSR
    MOVLW 8  ;COUNT 8 CHARS
    MOVWF _gen_count2
    BSF DISP_RS ;RS LINE HIGH FOR DATA
PN_LOOP
    MOVF INDF, W
    CALL _lcd_send
    CALL _pulse_e
    MOVLW 2
    CALL _delay_millis
    INCF FSR, F
    DECFSZ _gen_count2, F
    GOTO PN_LOOP
    RETURN

SPI_TRANSFER
    MOVWF SSPBUF    ;SEND DATA
    BANKSEL SSPSTAT
SPI_WAIT
    BTFSS BF	;IS THE TRANSMISSION READY
    GOTO SPI_WAIT	;IF NOT WAIT UNTIL IT IS
    BANKSEL SSPBUF
    MOVF SSPBUF, W  ;COPY DATA TO W
    RETURN

BUILD_WORD  ;GENERATES PSEUDO-RANDOM TEST DATA
    MOVLW 16
    MOVWF _gen_count4
BUILD_WORD_LOOP
    ;GET WORD HERE
    CLRF _temp3  ;THIS WILL CONTAIN BIT 7 OF S_REG_LOW
    BTFSC _s_reg_low, 7
    BSF _temp3, 0
    MOVF _s_reg_low, W
    ANDLW 0x01     ;GET BIT 0
    XORWF _temp3, F   ;TEMP3 NOW CONTAINS BIT0 AND BIT7 XORED TOGETHER
    RRF _temp3, F
    RRF _s_reg_high, F
    RRF _s_reg_low, F
    DECFSZ _gen_count4, F
    GOTO BUILD_WORD_LOOP
    RETURN

GET_TEST_BYTE   ;SPLITS PSEUDO-RANDOM TEST DATA INTO SINGLE BYTES
    BTFSS _flag_reg, 0
    GOTO GTB_UPDATE
    MOVF _s_reg_low, W
    BCF _flag_reg, 0
    RETURN
GTB_UPDATE
    CALL BUILD_WORD
    BSF _flag_reg, 0
    MOVF _s_reg_high, W
    RETURN

global _delay_millis
_delay_millis    ;DESIGNED FOR 4MHZ CLOCK
    MOVWF _delay_counter
DM_MILLIS
    MOVLW 20
    MOVWF _ldel_count
DM_OUTER
    MOVLW 16
    MOVWF _sdel_count
DM_INNER
    DECFSZ _sdel_count,F
    GOTO DM_INNER
    DECFSZ _ldel_count,F
    GOTO DM_OUTER
    DECFSZ _delay_counter,F
    GOTO DM_MILLIS
    RETURN

global _print_test_data
_print_test_data
    CLRF _gen_count
PTD_OUTER
    CLRF _gen_count2
PTD_INNER
    MOVF _gen_count2, W
    ANDLW 0x0F
    BTFSS ZERO  ;EVERY 16 WORDS PRINT A NEWLINE
    GOTO PTD_NO_NEWLINE
    MOVLW 0x0A ;NEWLINE CHAR
    CALL _uart_send
PTD_NO_NEWLINE
    CALL GET_TEST_BYTE  ;GET MSB
    CALL _byte_to_hex
    MOVF _hex_char_high, W
    CALL _uart_send  ;HIGH NIBBLE HIGH BYTE
    MOVF _hex_char_low ,W
    CALL _uart_send  ;LOW NIBBLE HIGH BYTE
    CALL GET_TEST_BYTE  ;GET LSB
    CALL _byte_to_hex
    MOVF _hex_char_high, W
    CALL _uart_send  ;HIGH NIBBLE LOW BYTE
    MOVF _hex_char_low, W
    CALL _uart_send  ;LOW NIBBLE LOW BYTE
    DECFSZ _gen_count2, F
    GOTO PTD_INNER
    DECFSZ _gen_count, F
    GOTO PTD_OUTER
    RETURN

global _usb_reset_all
_usb_reset_all
    BCF USB_CS ;CH376 CE LINE LOW
    MOVLW 0x05 ;RESET ALL COMMAND
    CALL SPI_TRANSFER
    BSF USB_CS ;CH376S CE LINE HIGH
    RETURN

global _usb_set_file_name
_usb_set_file_name
    BCF STATUS, IRP
    MOVLW _tape_name ;NAME ADDRESS
    MOVWF FSR
    MOVLW 0x08 ;NAME LENGTH
    MOVWF _gen_count4
    BCF USB_CS
    MOVLW 0x2F ;SET FILE NAME COMMAND
    CALL SPI_TRANSFER
    MOVLW 0x2F ;'\' CHAR NEEDED AT START OF NAME
    CALL SPI_TRANSFER
USFN_LOOP
    MOVF INDF, W
    CALL SPI_TRANSFER
    INCF FSR, F
    DECFSZ _gen_count4, F
    GOTO USFN_LOOP
    CLRW   ;NULL CHAR AT END OF NAME
    CALL SPI_TRANSFER
    BSF USB_CS ;CE LINE HIGH
    RETURN

global _usb_file_open
_usb_file_open
    BCF USB_CS
    MOVLW 0x32 ;FILE OPEN COMMAND
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

global _usb_file_close
_usb_file_close
    BCF USB_CS
    MOVLW 0x36 ;FILE CLOSE COMMAND
    CALL SPI_TRANSFER
    MOVLW 0x01 ;UPDATE FILE LENGTH
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

USB_BYTE_READ
    BCF USB_CS
    MOVLW 0x3A ;BYTE READ COMMAND
    CALL SPI_TRANSFER
    MOVF _usb_file_size_low, W
    CALL SPI_TRANSFER
    MOVF _usb_file_size_high, W
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

USB_BYTE_READ_GO
    BCF USB_CS
    MOVLW 0x3B ;BYTE READ GO COMMAND
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

USB_READ_DATA0
    BCF USB_CS
    MOVLW 0x27 ;READ DATA0 COMMAND
    CALL SPI_TRANSFER
    CLRW
    CALL SPI_TRANSFER   ;GET NUMBER OF BYTES TO READ
    ADDWF _usb_bytes_read_low, F
    BTFSC CARRY
    INCF _usb_bytes_read_high, F
    MOVWF _gen_count4
URD_LOOP
    CLRW
    CALL SPI_TRANSFER
    CALL RAM_WRITE_AUTO	;this is a problem, two active spi devices
    DECFSZ _gen_count4, F
    GOTO URD_LOOP
    BSF USB_CS
    RETURN

global _usb_get_file_size
_usb_get_file_size
    BCF USB_CS
    MOVLW 0x0C ;GET FILE SIZE COMMAND
    CALL SPI_TRANSFER
    MOVLW 0x68 ;COMMAND DATA
    CALL SPI_TRANSFER
    CLRW
    CALL SPI_TRANSFER
    MOVWF _usb_file_size_low
    CLRW
    CALL SPI_TRANSFER
    MOVWF _usb_file_size_high
    CLRW
    CALL SPI_TRANSFER
    CLRW
    CALL SPI_TRANSFER   ;DISCARD UPPER BYTES OF FILE SIZE
    BSF USB_CS
    RETURN

global _usb_check_exists
_usb_check_exists
    BCF USB_CS
    MOVWF _temp4
    MOVLW 0x06
    CALL SPI_TRANSFER   ;SEND CHECK_EXISTS COMMAND
    MOVF _temp4, W
    CALL SPI_TRANSFER   ;SEND TEST DATA
    CLRW
    CALL SPI_TRANSFER   ;GET RESULT
    BSF USB_CS
    RETURN

;USB_FILE_CREATE
;    BCF USB_CS
;    MOVLW H'34'
;    MOVWF DREG
;    CALL SPI_TRANSFER
;    BSF USB_CS
;    RETURN

;USB_BYTE_WRITE
;    BCF USB_CS
;    MOVLW H'3C'
;    MOVWF DREG
;    CALL SPI_TRANSFER
;    MOVF USB_FILE_SIZE_LOW, W
;    MOVWF DREG
;    CALL SPI_TRANSFER
;    MOVF USB_FILE_SIZE_HIGH, W
;    MOVWF DREG
;    CALL SPI_TRANSFER
;    BSF USB_CS
;    RETURN

;USB_BYTE_WR_GO
;    BCF USB_CS
;    MOVLW H'3D'
;    MOVWF DREG
;    CALL SPI_TRANSFER
;    BSF USB_CS
;    RETURN

global _usb_get_status
_usb_get_status
    BCF USB_CS
    MOVLW 0x22
    CALL SPI_TRANSFER
    CLRW
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

global _usb_disk_mount
_usb_disk_mount
    BCF USB_CS
    MOVLW 0x31
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

global _usb_disk_connect
_usb_disk_connect
    BCF USB_CS
    MOVLW 0x30
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN
    
global _usb_set_mode
_usb_set_mode
    MOVWF _temp5
    BCF USB_CS
    MOVLW 0X15
    CALL SPI_TRANSFER
    MOVF _temp5, W
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

global _usb_get_version
_usb_get_version
    BCF USB_CS
    MOVLW 0x01
    CALL SPI_TRANSFER
    CLRW
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

USB_WAIT_STATUS
    INCF _timeout_high, F
    INCF _timeout_low, F
UWS_LOOP
    MOVLW 0x01
    CALL _delay_millis   ;DELAY 1 MILLISECOND
    CALL _usb_get_status
    XORWF _usb_target_status, W
    BTFSC ZERO
    RETURN
    DECFSZ _timeout_low, F
    GOTO UWS_LOOP
    DECFSZ _timeout_high, F
    GOTO UWS_LOOP
    BSF _flag_reg, 2    ;SET FLAG TO INDICATE A TIMEOUT
    RETURN

global _usb_disk_capacity
_usb_disk_capacity
    BCF USB_CS
    MOVLW 0x3E
    CALL SPI_TRANSFER   ;SEND DISK CAPACITY COMMAND
    BSF USB_CS
    MOVLW 0x14
    MOVWF _usb_target_status ;SET TARGET STATUS TO 14 (DEVICE READY)
    CLRF _timeout_high
    MOVLW 100
    MOVWF _timeout_low   ;SET TIMEOUT TO 16 MILLISECONDS
    CALL USB_WAIT_STATUS
    BCF USB_CS
    MOVLW 0x27
    CALL SPI_TRANSFER   ;SEND READ DATA0 COMMAND
    CLRW
    CALL SPI_TRANSFER   ;GET NUMBER OF BYTES TO READ (ALWAYS 4)
    CLRW
    CALL SPI_TRANSFER   ;GET LSB OF DISK SIZE
;THE DISK SIZE IS GIVEN IN SECTORS, SO TO GET THE NUMBER OF BYTES WE MULTIPLY BY 512 WHICH IS THE SAME AS SHIFTING BY 9
;THIS MEANS THE LSB IS ALWAYS ZERO AND THERE IS NO NEED TO HAVE A REGISTER FOR IT.
;INSTEAD WE PUT THE LSB IN USB_DRIVE_SIZE1 WHICH CORRESPONDS TO SHIFTING BY 8.
    MOVWF _usb_drive_size1
    CLRW
    CALL SPI_TRANSFER
    MOVWF _usb_drive_size2
    CLRW
    CALL SPI_TRANSFER
    MOVWF _usb_drive_size3
    CLRW
    CALL SPI_TRANSFER
    BSF USB_CS
    BCF CARRY   ;CLEAR CARRY FLAG
    RLF _usb_drive_size1, F
    RLF _usb_drive_size2, F
    RLF _usb_drive_size3, F
    RETURN

global _usb_disk_query
_usb_disk_query
    BCF USB_CS
    MOVLW 0x3F
    CALL SPI_TRANSFER   ;SEND DISK QUERY COMMAND
    BSF USB_CS
    MOVLW 0x14
    MOVWF _usb_target_status ;SET TARGET STATUS TO 14 (DEVICE READY)
    CLRF _timeout_high
    MOVLW 100
    MOVWF _timeout_low   ;SET TIMEOUT TO 16 MILLISECONDS
    CALL USB_WAIT_STATUS
    BCF USB_CS
    MOVLW 0x27
    CALL SPI_TRANSFER   ;SEND READ DATA0 COMMAND
    CLRW
    CALL SPI_TRANSFER   ;GET NUMBER OF BYTES TO READ (ALWAYS 8)
    CLRW
    CALL SPI_TRANSFER   ;GET LSB OF LOGICAL BYTES
    MOVWF _usb_logical_bytes1
    CLRW
    CALL SPI_TRANSFER
    MOVWF _usb_logical_bytes2
    CLRW
    CALL SPI_TRANSFER
    MOVWF _usb_logical_bytes3
    CLRW
    CALL SPI_TRANSFER   ;DISCARD MSB OF LOGICAL BYTES
    CLRW
    CALL SPI_TRANSFER   ;GET LSB OF FREE SPACE
    MOVWF _usb_free_space1
    CLRW
    CALL SPI_TRANSFER
    MOVWF _usb_free_space2
    CLRW
    CALL SPI_TRANSFER
    MOVWF _usb_free_space3
    CLRW
    CALL SPI_TRANSFER   ;DISCARD MSB OF FREE SPACE
    BSF USB_CS
    BCF CARRY
    RLF _usb_logical_bytes1, F
    RLF _usb_logical_bytes2, F
    RLF _usb_logical_bytes3, F
    BCF CARRY
    RLF _usb_free_space1, F
    RLF _usb_free_space2, F
    RLF _usb_free_space3, F
    RETURN

;USB_WRITE_TEST_DATA
;    CLRF S_REG_LOW
;    CLRF S_REG_HIGH
;    BCF FLAG_REG, 0 ;INITIALIZE PSUEDO-RANDOM DATA GENERATOR
;    CALL USB_FILE_CREATE
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH  ;SET TIMEOUT TO 3 SECONDS
;    MOVLW H'14'
;    MOVWF USB_TARGET_STATUS ;SET TARGET STATUS TO 0x14
;    CALL USB_WAIT_STATUS
;    MOVLW H'04'
;    MOVWF GEN_COUNT6
;UWTD_L4
;    CLRF USB_BYTES_SENT_HIGH
;    CLRF USB_BYTES_SENT_LOW
;    MOVLW H'80'
;    MOVWF USB_FILE_SIZE_HIGH
;    CLRF USB_FILE_SIZE_LOW
;    CALL USB_BYTE_WRITE
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'1E'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;UWTD_ND_LOOP
;    BCF USB_CS
;    MOVLW H'2D'
;    MOVWF DREG
;    CALL SPI_TRANSFER   ;SEND WR_REQ_DATA
;    CLRF DREG
;    CALL SPI_TRANSFER   ;GET NUMBER OF BYTES TO SEND
;    MOVF DREG, W
;    MOVWF GEN_COUNT5
;UWTD_WR_LOOP
;    CALL GET_TEST_BYTE
;    CALL SPI_TRANSFER
;    INCFSZ USB_BYTES_SENT_LOW, F
;    DECF USB_BYTES_SENT_HIGH, F
;    INCF USB_BYTES_SENT_HIGH, F
;    MOVF USB_BYTES_SENT_HIGH, W
;    XORWF USB_FILE_SIZE_HIGH, W
;    BTFSS STATUS, Z
;    GOTO UWTD_NB
;    MOVF USB_BYTES_SENT_LOW, W
;    XORWF USB_FILE_SIZE_LOW, W
;    BTFSC STATUS, Z
;    GOTO UWTD_BREAK
;UWTD_NB
;    DECFSZ GEN_COUNT5, F
;    GOTO UWTD_WR_LOOP
;UWTD_BREAK
;    BSF USB_CS
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'1E'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    MOVF USB_BYTES_SENT_HIGH, W
;    XORWF USB_FILE_SIZE_HIGH, W
;    BTFSS STATUS, Z
;    GOTO UWTD_NE
;    MOVF USB_BYTES_SENT_LOW, W
;    XORWF USB_FILE_SIZE_LOW, W
;    BTFSC STATUS, Z
;    GOTO UWTD_EQUAL
;UWTD_NE
;    CALL USB_BYTE_WR_GO
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'1E'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    GOTO UWTD_ND_LOOP
;UWTD_EQUAL
;    CALL USB_BYTE_WR_GO
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'14'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    DECFSZ GEN_COUNT6, F
;    GOTO UWTD_L4
;    CALL USB_FILE_CLOSE
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'14'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    RETURN

;USB_FILE_WRITE
;    CALL USB_SET_FILE_NAME
;    MOVLW H'10'
;    MOVWF TIMEOUT_LOW
;    CLRF TIMEOUT_HIGH
;    MOVLW H'14'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    CALL USB_FILE_CREATE
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH  ;SET TIMEOUT TO 3 SECONDS
;    MOVLW H'14'
;    MOVWF USB_TARGET_STATUS ;SET TARGET STATUS TO 0x14
;    CALL USB_WAIT_STATUS
;    CLRF USB_BYTES_SENT_HIGH
;    CLRF USB_BYTES_SENT_LOW
;    MOVF TAPE_SIZE_HIGH, W
;    MOVWF USB_FILE_SIZE_HIGH
;    MOVF TAPE_SIZE_LOW, W
;    MOVWF USB_FILE_SIZE_LOW
;    CALL USB_BYTE_WRITE
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'1E'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;UFW_ND_LOOP
;    CALL BUFF_INIT
;    MOVF USB_BYTES_SENT_LOW, W
;    MOVWF RAM_ADDRESS_L
;    MOVF USB_BYTES_SENT_HIGH, W
;    MOVWF RAM_ADDRESS_H
;    DECF RAM_ADDRESS_L, F
;    BTFSS STATUS, C
;    DECF RAM_ADDRESS_H, F
;    CALL BUFF_FILL
;    BCF USB_CS
;    MOVLW H'2D'
;    MOVWF DREG
;    CALL SPI_TRANSFER   ;SEND WR_REQ_DATA
;    CLRF DREG
;    CALL SPI_TRANSFER   ;GET NUMBER OF BYTES TO SEND
;    MOVF DREG, W
;    MOVWF GEN_COUNT5
;UFW_WR_LOOP
;    CALL BUFF_READ_AUTO
;    CALL SPI_TRANSFER
;    INCFSZ USB_BYTES_SENT_LOW, F
;    DECF USB_BYTES_SENT_HIGH, F
;    INCF USB_BYTES_SENT_HIGH, F
;    MOVF USB_BYTES_SENT_HIGH, W
;    XORWF USB_FILE_SIZE_HIGH, W
;    BTFSS STATUS, Z
;    GOTO UFW_NB
;    MOVF USB_BYTES_SENT_LOW, W
;    XORWF USB_FILE_SIZE_LOW, W
;    BTFSC STATUS, Z
;    GOTO UFW_BREAK
;UFW_NB
;    DECFSZ GEN_COUNT5, F
;    GOTO UFW_WR_LOOP
;UFW_BREAK
;    BSF USB_CS
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'1E'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    MOVF USB_BYTES_SENT_HIGH, W
;    XORWF USB_FILE_SIZE_HIGH, W
;    BTFSS STATUS, Z
;    GOTO UFW_NE
;    MOVF USB_BYTES_SENT_LOW, W
;    XORWF USB_FILE_SIZE_LOW, W
;    BTFSC STATUS, Z
;    GOTO UFW_EQUAL
;UFW_NE
;    CALL USB_BYTE_WR_GO
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'1E'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    GOTO UFW_ND_LOOP
;UFW_EQUAL
;    CALL USB_BYTE_WR_GO
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'14'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    CALL USB_FILE_CLOSE
;    MOVLW H'0B'
;    MOVWF TIMEOUT_HIGH
;    MOVLW H'B8'
;    MOVWF TIMEOUT_LOW
;    MOVLW H'14'
;    MOVWF USB_TARGET_STATUS
;    CALL USB_WAIT_STATUS
;    RETURN

global _usb_file_read
_usb_file_read
    MOVLW 0xFF
    MOVWF _ram_address_h
    MOVWF _ram_address_l ;RAM ADDRESS WILL OVERFLOW BEFORE FIRST WRITE
    CALL USB_BYTE_READ
    CLRF _timeout_high
    MOVLW 0x80
    MOVWF _timeout_low
    MOVLW 0x1D
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSS _flag_reg, 2   ;CHECK TIMEOUT FLAG
    ;GOTO _timeout_error
    RETURN
UFR_LOOP
    CALL USB_READ_DATA0
    MOVLW 0x01
    CALL _delay_millis
    MOVF _usb_file_size_low, W
    XORWF _usb_bytes_read_low, W
    BTFSS ZERO
    GOTO UFR_GO
    MOVF _usb_file_size_high, W
    XORWF _usb_bytes_read_high, W
    BTFSC ZERO
    GOTO UFR_DONE
UFR_GO
    CALL USB_BYTE_READ_GO
    CLRF _timeout_high
    MOVLW 0xFF
    MOVWF _timeout_low
    MOVLW 0x1D
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    GOTO UFR_LOOP
UFR_DONE
    MOVF _usb_file_size_low, W
    MOVWF _tape_size_low
    MOVF _usb_file_size_high, W
    MOVWF _tape_size_high
    BCF USB_CS
    MOVLW 0x36
    CALL SPI_TRANSFER   ;SEND FILE CLOSE COMMAND
    CLRW
    CALL SPI_TRANSFER   ;FILE CLOSE PARAMETER IS 0 (DO NOT UPDATE FILE SIZE)
    BSF USB_CS
    CLRF _timeout_high
    MOVLW 0x80
    MOVWF _timeout_low
    MOVLW 0x14
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    RETURN
    
_fifo_init
    CLRF _fifo_write_idx
    CLRF _fifo_write_bank
    CLRF _fifo_read_idx
    CLRF _fifo_read_bank
    CLRF _fifo_num_bytes
    RETURN
    
_fifo_tf_to_mem
    MOVF _fifo_num_bytes, F
    BTFSC ZERO
    RETURN
    CALL _fifo_pop
    CALL RAM_WRITE_AUTO
    GOTO _fifo_tf_to_mem

_fifo_tf_from_mem
    COMF _fifo_num_bytes, W
    BTFSC ZERO
    RETURN
    CALL RAM_READ_AUTO
    CALL _fifo_push
    GOTO _fifo_tf_from_mem