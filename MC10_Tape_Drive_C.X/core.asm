#include <xc.inc>
#define IRP 7
    
#define DISP_E PORTA, 2
#define DISP_RS PORTA, 3
#define USB_INT PORTB, 0
#define TAPE_IN PORTB, 1
;#define TAPE_OUT PORTB, 2
#define SW0 PORTB, 3
#define SW1 PORTB, 4
#define SW2 PORTB, 5
#define SW3 PORTB, 6
#define RAM_CS PORTB, 7
#define USB_CS PORTD, 2
#define USB_RST PORTD, 3
#define DISP_CLK PORTD, 6
#define DISP_DATA PORTD, 7
    
#define F_SRL_VALID _flag_reg, 0
#define F_BB_FIRST _flag_reg, 1
#define F_TIMEOUT _flag_reg, 2
#define F_SPACE_OK _flag_reg, 3
    
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
    
;FLAG REG USAGE:
;BIT0 SET IF S_REG_LOW CONTAINS VALID DATA
;BIT1 SET IF BUILD_BYTE IS WAITING FOR THE FIRST BYTE OF A TRANSMISSION (PREVENTS RESET IF TRANSMISSION FAILS)
;BIT2 SET IF TIMEOUT OCCURED WHILE WAITING FOR USB STATUS
;BIT3 SET IF FREE SPACE EXCEEDS TAPE SIZE
;BIT4 SET IF FREE SPACE EXCEEDS 0x20000
;BIT5 UNUSED
;BIT6 UNUSED
;BIT7 UNUSED

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

PSECT intentry,class=CODE,delta=2,space=0
    GOTO _tape_int

psect   barfunc,local,class=CODE,delta=2 ; PIC10/12/16
; psect   barfunc,local,class=CODE,reloc=2 ; PIC18

global _temp1
global _temp2
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
global _gen_count5
global _gen_count6
global _tape_size_high
global _tape_size_low
global _bit_count
global _hex_char_high
global _hex_char_low
global _usb_bytes_sent_high
global _usb_bytes_sent_low
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
    MOVWF BANKMASK(TRISA)	;CONFIGURE PORTA
    MOVLW 0b01111011
    MOVWF BANKMASK(TRISB) ;CONFIGURE PORTB
    MOVLW 0b11010000
    MOVWF BANKMASK(TRISC) ;CONFIGURE PORTC
    MOVLW 0b00000000
    MOVWF BANKMASK(TRISD)	;CONFIGURE PORTD
    MOVLW 0b11111000
    MOVWF BANKMASK(TRISE)	;CONFIGURE PORTE
    MOVLW 0b00000000	    ;SMP=0, CKE=0, DA=0, P=0, S=0, RW=0, UA=0, BF=0
    MOVWF BANKMASK(SSPSTAT) ;CONFIGURE SPI
    MOVLW 0b00001111	    ;PCFG=1111
    MOVWF BANKMASK(ADCON1) ;DISABLE ANALOG INPUTS
    MOVLW 0b00100110	;CSRC=0, TX9=0, TXEN=1, SYNC=0, UNIMPLEMENTED=0, BRGH=1, TRMT=1, TX9D=0
    MOVWF BANKMASK(TXSTA) ;CONFIGURE UART
    MOVLW 25
    MOVWF BANKMASK(SPBRG) ;CONFIGURE BAUD RATE
    MOVLW 0b11011000	;RBPU = 1, INTEDG = 1, T0CS = 0, T0SE = 1, PSA = 1, PS2 = 0, PS1 = 0, PS0 = 0
    MOVWF BANKMASK(OPTION_REG)	;CONFIGURE TIMER
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
    BCF USB_RST
    MOVLW 50
    CALL _delay_millis   ;50 MS DELAY AT STARTUP
    CALL _fifo_init
    CALL RAM_CONFIG
    CLRF _ram_address_h
    CLRF _ram_address_l
    MOVLW 0x55
    CALL RAM_WRITE
    MOVLW 0xAA
    CALL RAM_WRITE_AUTO
    ;CALL _mem_clear
    CALL FUNCTIONSET_LCD    ;INITIALIZE DISPLAY
    CALL DISPLAYON  ;POWER ON THE DISPLAY
    CALL CLEARDISPLAY	;CLEAR THE DISPLAY
    CALL SETHOME_LCD	;SET CURSOR TO THE START OF LINE 1
    RETURN

global _mem_clear
_mem_clear
    MOVLW 0xFF
    MOVWF _ram_address_l
    MOVWF _ram_address_h
MEM_CLEAR_LOOP
    ;BSF PORTD, 4
    MOVLW 0xFF
    CALL RAM_WRITE_AUTO
    ;BCF PORTD, 4    ;PORTD 4 IS A DEBUG OUTPUT TO MEASURE THE DURATION OF RAM_WRITE
    MOVF _ram_address_l, W
    XORLW 0xFF
    BTFSS ZERO
    GOTO MEM_CLEAR_LOOP
    MOVF _ram_address_h, W
    XORLW 0xFF
    BTFSS ZERO
    GOTO MEM_CLEAR_LOOP
    RETURN

global _mem_dump
_mem_dump
    MOVLW 0xFF
    MOVWF _ram_address_l
    MOVWF _ram_address_h
MEM_DUMP_LOOP
    CALL RAM_READ_AUTO
    CALL _uart_send
    MOVF _ram_address_l, W
    XORLW 0xFF
    BTFSS ZERO
    GOTO MEM_DUMP_LOOP
    MOVF _ram_address_h, W
    XORLW 0xFF
    BTFSS ZERO
    GOTO MEM_DUMP_LOOP
    RETURN

global _file_dump
_file_dump
    MOVF _usb_file_size_high, W
    IORWF _usb_file_size_low, W
    BTFSC ZERO
    RETURN
    MOVF _usb_file_size_high, W
    MOVWF _gen_count
    MOVF _usb_file_size_low, W
    MOVWF _gen_count2
    MOVLW 0xFF
    MOVWF _ram_address_l
    MOVWF _ram_address_h
    INCF _gen_count, F
FD_INNER
    CALL RAM_READ_AUTO
    CALL _uart_send
    DECFSZ _gen_count2, F
    GOTO FD_INNER
    DECFSZ _gen_count, F
    GOTO FD_INNER
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
    INCFSZ _ram_address_l, F    ;INCREMENT LOW ADDRESS
    DECF _ram_address_h, F
    INCF _ram_address_h, F	;IF LOW ADDRESS IS ZERO INCREMENT THE HIGH ADDRESS TOO
RAM_WRITE
    MOVWF _temp5
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

;TAPE_IN
;    MOVLW H'FF'
;    MOVWF RAM_ADDRESS_H
;    MOVWF RAM_ADDRESS_L
;    BSF FLAG_REG,1 ;DISSABLE RESET IF TRANSMISSION FAILS
;    CALL GET_LEADER
;    BCF FLAG_REG,1 ;ENABLE RESET IF TRANSMISSION FAILS
;    CALL SAVE_FIRST_BLOCK
;    INCFSZ RAM_ADDRESS_L, F    ;INCREMENT LOW ADDRESS
;    DECF RAM_ADDRESS_H, F
;    INCF RAM_ADDRESS_H, F	;IF LOW ADDRESS IS ZERO INCREMENT THE HIGH ADDRESS TOO
;    MOVF RAM_ADDRESS_H, W
;    MOVWF TAPE_SIZE_HIGH
;    MOVF RAM_ADDRESS_L, W
;    MOVWF TAPE_SIZE_LOW
;    CLRF RAM_ADDRESS_H
;    MOVLW H'03'
;    MOVWF RAM_ADDRESS_L
;    BCF STATUS, IRP ;SET BANKS 0 AND 1
;    MOVLW TAPE_NAME
;    MOVWF FSR
;    MOVLW D'08'
;    MOVWF GEN_COUNT
;NAME_COPY
;    CALL RAM_READ_AUTO
;    MOVF DREG, W
;    MOVWF INDF
;    INCF FSR, F
;    DECFSZ GEN_COUNT, F
;    GOTO NAME_COPY
;    RETURN

;GET_LEADER
;    MOVLW D'128'    ;LENGTH OF LEADER
;    MOVWF GEN_COUNT2
;LEADER_LOOP
;    CALL BUILD_BYTE
;    MOVF DREG,W
;    XORLW H'55'
;    BTFSS STATUS, Z
;    GOTO GL_FIX
;    DECFSZ GEN_COUNT2, F
;    GOTO LEADER_LOOP
;    RETURN
;GL_FIX
;    DECFSZ GEN_COUNT2, F
;    GOTO GL_FIX2
;    GOTO BUILD_PANIC
;GL_FIX2
;    CALL BUILD_ALIGN
;    MOVF DREG,W
;    XORLW H'55'
;    BTFSS STATUS,Z
;    GOTO BUILD_PANIC
;    DECFSZ GEN_COUNT2, F
;    GOTO LEADER_LOOP
;    GOTO BUILD_PANIC

;SAVE_FIRST_BLOCK
;    CALL BUILD_BYTE
;    BTFSS DREG,0    ;CHECK IF BIT 0 IS SET (IF IT IS CLEARED THERE HAS BEEN AN ALIGNMENT ERROR)
;    GOTO BUILD_PANIC
;    CALL RAM_WRITE_AUTO	;SAVE LEADER BYTE
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE SYNC BYTE
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE BLOCK TYPE BYTE
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE BLOCK LENGTH
;    MOVF DREG, W
;    MOVWF GEN_COUNT2
;FIRST_BLOCK_LOOP
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO
;    DECFSZ GEN_COUNT2, F
;    GOTO FIRST_BLOCK_LOOP
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE CHECKSUM
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE LEADER BYTE
;    CALL GET_LEADER ;GETS THE SECOND LEADER
;SAVE_BLOCKS
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE LEADER BYTE
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE SYNC BYTE
;    CALL BUILD_BYTE
;    INCFSZ DREG, W  ;CHECK IF BLOCK IS END OF FILE
;    GOTO NOT_EOF_BLOCK
;    CALL RAM_WRITE_AUTO	;SAVE BLOCK TYPE
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE BLOCK LENGTH
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE CHECKSUM
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE LEADER BYTE
;    RETURN
;NOT_EOF_BLOCK
;    CALL RAM_WRITE_AUTO	;SAVE BLOCK TYPE
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE LENGTH BYTE
;    MOVF DREG, W
;    MOVWF GEN_COUNT2 ;COPY LENGTH TO GEN_COUNT2
;BLOCK_DATA_LOOP
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO
;    DECFSZ GEN_COUNT2, F
;    GOTO BLOCK_DATA_LOOP
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE CHECKSUM
;    CALL BUILD_BYTE
;    CALL RAM_WRITE_AUTO	;SAVE LEADER BYTE
;    GOTO SAVE_BLOCKS

;BUILD_ALIGN ;SKIPS INITIAL WAIT IN AN ATTEMPT TO FIX ALIGNMENT. BUILD_BYTE IS THE NORMAL ENTRY POINT
;    MOVLW H'08'
;    MOVWF GEN_COUNT ;INITIALIZE COUNTER TO 8
;    MOVF PORTB, W
;    XORLW H'02' ;INVERT TAPE BIT
;    MOVWF TEMP1	    ;COPY PORTB TO TEMP1
;    GOTO WAIT_STOP
;BUILD_BYTE
;    MOVLW H'08'
;    MOVWF GEN_COUNT ;INITIALIZE COUNTER TO 8
;BUILD_LOOP
;    MOVF PORTB, W
;    MOVWF TEMP1	    ;COPY PORTB TO TEMP1
;    CLRF PULSE_LENGTH
;    RRF DREG, F
;    BCF DREG, 7
;WAIT_START
;    MOVF PORTB, W
;    XORWF TEMP1, W  ;COMPARE PORTB WITH PREVIOUS STATE
;    MOVWF TEMP2	    ;STORE RESULT IN TEMP2
;    BTFSS TEMP2, 1
;    GOTO WAIT_START ;IF STATE HAS NOT CHANGED KEEP WAITING
;WAIT_STOP
;    CALL TAPE_DELAY
;    INCF PULSE_LENGTH, F	;#1
;    MOVF PULSE_LENGTH, W    ;#2
;    SUBLW D'22'	    ;#3
;    BTFSC STATUS, C ;CHECK IF LENGTH IS GREATER THAN 22 #4
;    GOTO BUILD_NO_ERROR	;IF NOT CONTINUE NORMALLY #6
;BUILD_ERROR
;    BTFSS FLAG_REG, 1 ;IF ERROR IS IN FIRST BYTE TRY TO RECOVER
;    GOTO BUILD_PANIC   ;IF NOT RESET TO AVOID INFINITE LOOP
;    ;MOVF GEN_COUNT, W
;    ;SUBLW H'08'
;    ;BTFSS STATUS, Z ;CHECK IF ERROR IS IN FIRST BIT
;    ;GOTO BUILD_PANIC   ; IF NOT RESET TO AVOID INFINITE LOOP
;    GOTO BUILD_BYTE ;IF IT IS IN FIRST BIT OF FIRST BYTE RESTART SUBROUTINE
;BUILD_NO_ERROR
;    MOVF PORTB, W   ;#7
;    XORWF TEMP1, W  ;#8
;    MOVWF TEMP2	    ;#9
;    BTFSC TEMP2, 1  ;#10
;    GOTO WAIT_STOP  ;WAIT UNTIL TAPE INPUT RETURNS TO ORIGINAL STATE #12
;    MOVF PULSE_LENGTH, W
;    SUBLW D'08'
;    BTFSC STATUS, C ;CHECK IF LENGTH IS LESS THAN 8
;    GOTO BUILD_ERROR	;IF IT IS GO TO LOCAL ERROR HANDLER
;    MOVLW D'15'
;    SUBWF PULSE_LENGTH, F   ;SUBTRACT THRESHOLD FROM PULSE LENGTH
;    BTFSC PULSE_LENGTH, 7   ;CHECK IF PULSE LENGHT IS NEGATIVE
;    BSF DREG, 7		    ;IF IT IS SET BIT IN DREG
;    DECFSZ GEN_COUNT, F
;    GOTO BUILD_LOOP
;    RETLW H'00'	;RETURN ZERO TO INDICATE SUCCESS

;BUILD_PANIC ;ENABLES THE WATCHDOG TO CAUSE A RESET
;    BANKSEL WDTCON
;    BSF WDTCON, SWDTEN  ;ENABLE WATCHDOG
;BP_HALT
;    GOTO BP_HALT    ;WAIT FOR THE WATCHDOG TO DO THE THING

;TAPE_DELAY
;    NOP
;    NOP
;    NOP
;    NOP
;    NOP	;5 CYCLE DELAY
;    RETURN

TAPE_OUT
    CLRF _temp1
    CLRF _bit_count
    MOVLW 0xFF
    MOVWF _ram_address_h
    MOVWF _ram_address_l	;RAM READ FUNCTION WILL AUTO INCREMENT ADDRESS, SO IT SHOULD OVERFLOW WHEN IT IS FIRST CALLED
    MOVLW 0x55
    MOVWF _temp5    ;TAPE LEADER VALUE
    MOVLW 127
    MOVWF _gen_count ;128 LEADER BYTES
    CLRF TMR0
    NOP
    BCF T0IF
    BSF GIE	;ENABLE INTERRUPT
LEADER_OUT
    BTFSS _bit_count, 2
    GOTO LEADER_OUT	;WAIT UNTIL BIT COUNT IS NOT ZERO
LEADER_WAIT
    BTFSC _bit_count, 2
    GOTO LEADER_WAIT	;WAIT UNTIL BIT_COUNT IS ZERO
    DECFSZ _gen_count, F
    GOTO LEADER_OUT	;LOOP UNTIL 128 LEADER BYTES HAVE BEEN SENT
    MOVLW 22
    MOVWF _gen_count	;ONE PAST THE END OF THE NAME FILE
TAPE_NAMEBLOCK
    CALL RAM_READ_AUTO	;GET FIRST BYTE
    MOVWF _temp5
TAPE_W1
    BTFSS _bit_count, 2
    GOTO TAPE_W1    ;WAIT UNTIL BIT COUNT IS NOT ZERO
TAPE_W2
    BTFSC _bit_count, 2
    GOTO TAPE_W2
    DECFSZ _gen_count, F
    GOTO TAPE_NAMEBLOCK
    BCF GIE ;STOP INTERRUPT
    CALL TAPE_PAUSE ;1/2 SECOND SILENCE
    MOVLW 127
    MOVWF _gen_count ;128 LEADER BYTES
    CLRF TMR0
    CLRF _temp1
    NOP
    BCF T0IF
    BSF GIE	;ENABLE INTERRUPT
LEADER_OUT2
    BTFSS _bit_count, 2
    GOTO LEADER_OUT2	;WAIT UNTIL BIT COUNT IS NOT ZERO
LEADER_WAIT2
    BTFSC _bit_count, 2
    GOTO LEADER_WAIT2	;WAIT UNTIL BIT_COUNT IS ZERO
    DECFSZ _gen_count, F
    GOTO LEADER_OUT2	;LOOP UNTIL 128 LEADER BYTES HAVE BEEN SENT
TAPE_NE
    CALL RAM_READ_AUTO
    MOVWF _temp5
TAPE_W3
    BTFSS _bit_count, 2
    GOTO TAPE_W3    ;WAIT UNTIL BIT_COUNT IS NOT ZERO
TAPE_W4
    BTFSC _bit_count, 2
    GOTO TAPE_W4
    MOVF _tape_size_high, W
    XORWF _ram_address_h, W
    MOVWF _temp3	;STORE RESULT IN TEMP3
    INCF _temp3, F   ;INCREMENT RESULT
    DECFSZ _temp3, F
    GOTO TAPE_NE
    MOVF _tape_size_low, W
    XORWF _ram_address_l, W
    MOVWF _temp3	;STORE RESULT IN TEMP3
    INCF _temp3, F  ;TODO: this needs optimizing
    DECFSZ _temp3, F
    GOTO TAPE_NE
    BCF GIE
    RETURN

global _tape_int
_tape_int
    MOVWF _temp4	;SAVE W REG
    BCF T0IF
    MOVLW 59	;USED TO BE 58, CHANGED IT BECAUSE I ADDED THE SAVE W INSTRUCTION
    MOVWF TMR0	;PRELOAD TIMER
    BTFSC _temp1, 0  ;IDENTIFY PHASE
    GOTO PHASE1
    BTFSC _temp1, 1
    GOTO PHASE2
    BTFSC _temp1, 2
    GOTO PHASE3
    BTFSC _temp1, 3
    GOTO PHASE4
    MOVLW 0b00000100	;DEFAULT PHASE - LOAD NEW DATA AND INITIALIZE VARIABLES
    XORWF PORTB, F  ;TOGGLE TAPE OUT
    MOVF _temp5, W
    MOVWF _temp2	    ;COPY NEW DATA TO BUFFER
    CLRF _bit_count
    MOVLW 0x01
    MOVWF _temp1	;SET PHASE TO 1
    MOVF _temp4, W   ;RESTORE W
    RETFIE
PHASE1	;TOGGLE IF BIT IS 1
	MOVLW 0b00000100	;WILL USE THIS TO TOGGLE OUTPUT
	BTFSC _temp2, 0	;CHECK IF BIT IS 1 OR 0
	XORWF PORTB, F	;IF BIT IS 1 TOGGLE OUTPUT
	MOVLW 0x02
	MOVWF _temp1	;SET PHASE TO 2
	MOVF _temp4, W   ;RESTORE W
	RETFIE
PHASE2	;ALWAYS TOGGLE
	MOVLW 0b00000100
	XORWF PORTB, F	;TOGGLE OUTPUT
	BTFSC _temp2, 0	;CHECK IF BIT IS 1 OR 0
	GOTO PHASE2_NEXT	;IF BIT IS SET THEN LOAD NEXT BIT AND RESET PHASE
	MOVLW 0x04
	MOVWF _temp1	;SET PHASE TO 3
	MOVF _temp4, W	;RESTORE W
	RETFIE
PHASE3	;NEVER TOGGLE
	MOVLW 0x08
	MOVWF _temp1	;SET PHASE TO 4
	MOVF _temp4, W
	RETFIE
PHASE4	;ALWAYS TOGGLE
	MOVLW 0b00000100
	XORWF PORTB, F	;TOGGLE OUTPUT
PHASE2_NEXT
    MOVLW 0x01
    MOVWF _temp1	;SET PHASE TO 1
    RRF _temp2, F	;SHIFT TO NEXT BIT
    INCF _bit_count, F	;NEXT BIT COUNT
    BTFSS _bit_count, 3	;CHECK IF BIT COUNT HAS REACHED 8
    GOTO PHASE_DONE	;IF NOT, RETURN
    CLRF _bit_count	;CLEAR BIT COUNT
    MOVF _temp5, W
    MOVWF _temp2	;COPY NEW DATA TO BUFFER
PHASE_DONE
    MOVF _temp4, W
    RETFIE

TAPE_PAUSE
    MOVLW 32
    MOVWF _gen_count
TP1
    MOVLW 54
    MOVWF _gen_count2
TP2
    MOVLW 95
    MOVWF _ldel_count
TP3
    DECFSZ _ldel_count, F
    GOTO TP3
    DECFSZ _gen_count2, F
    GOTO TP2
    DECFSZ _gen_count, F
    GOTO TP1
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
    ANDLW 0x01	    ;GET BIT 0
    XORLW 0x01	    ;FLIP BIT 0
    XORWF _temp3, F   ;TEMP3 NOW CONTAINS BIT0 ^ BIT7 ^ 1
    RRF _temp3, F
    RRF _s_reg_high, F
    RRF _s_reg_low, F
    DECFSZ _gen_count4, F
    GOTO BUILD_WORD_LOOP
    RETURN

GET_TEST_BYTE   ;SPLITS PSEUDO-RANDOM TEST DATA INTO SINGLE BYTES
    BTFSS F_SRL_VALID
    GOTO GTB_UPDATE
    MOVF _s_reg_low, W
    BCF F_SRL_VALID
    RETURN
GTB_UPDATE
    CALL BUILD_WORD
    BSF F_SRL_VALID
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
    PAGESEL _byte_to_hex
    CALL (_byte_to_hex & 0x7FF)
    PAGESEL _print_test_data
    MOVF _hex_char_high, W
    CALL _uart_send  ;HIGH NIBBLE HIGH BYTE
    MOVF _hex_char_low ,W
    CALL _uart_send  ;LOW NIBBLE HIGH BYTE
    CALL GET_TEST_BYTE  ;GET LSB
    PAGESEL _byte_to_hex
    CALL (_byte_to_hex & 0x7FF)
    PAGESEL _print_test_data
    MOVF _hex_char_high, W
    CALL _uart_send  ;HIGH NIBBLE LOW BYTE
    MOVF _hex_char_low, W
    CALL _uart_send  ;LOW NIBBLE LOW BYTE
    DECFSZ _gen_count2, F
    GOTO PTD_INNER
    DECFSZ _gen_count, F
    GOTO PTD_OUTER
    RETURN

;CHECK_FREE_SPACE
;    BCF FLAG_REG, 3
;    BCF FLAG_REG, 4
;    MOVF USB_FREE_SPACE3, F
;    BTFSS STATUS, Z
;    GOTO CFS2
;    BSF FLAG_REG, 3
;    BSF FLAG_REG, 4
;    RETURN
;CFS2
;    MOVF USB_FREE_SPACE2, W
;    SUBLW H'02'
;    BTFSS STATUS, C
;    GOTO CFS3
;    BSF FLAG_REG, 3
;    BSF FLAG_REG, 4
;    RETURN
;CFS3
;    MOVF USB_FREE_SPACE1, W
;    XORWF TAPE_SIZE_HIGH, W
;    BTFSC STATUS, Z
;    GOTO CFS4
;    MOVF TAPE_SIZE_HIGH, W
;    SUBWF USB_FREE_SPACE1, W
;    BTFSC STATUS, C
;    BSF FLAG_REG, 3
;    RETURN
;CFS4
;    MOVF TAPE_SIZE_LOW, F
;    BTFSC STATUS, Z
;    BSF FLAG_REG, 3
;    RETURN

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
    ;CALL RAM_WRITE_AUTO	;this is a problem, two active spi devices
    CALL _fifo_push
    DECFSZ _gen_count4, F
    GOTO URD_LOOP
    BSF USB_CS
    CALL _fifo_tf_to_mem
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

global _usb_file_create
_usb_file_create
    BCF USB_CS
    MOVLW 0x34
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

USB_BYTE_WRITE
    BCF USB_CS
    MOVLW 0x3C
    CALL SPI_TRANSFER
    MOVF _usb_file_size_low, W
    CALL SPI_TRANSFER
    MOVF _usb_file_size_high, W
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

USB_BYTE_WR_GO
    BCF USB_CS
    MOVLW 0x3D
    CALL SPI_TRANSFER
    BSF USB_CS
    RETURN

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
    BCF F_TIMEOUT
    INCF _timeout_high, F
    INCF _timeout_low, F
UWS_LOOP
    MOVLW 0x01
    CALL _delay_millis   ;DELAY 1 MILLISECOND
    CALL _usb_get_status
    MOVWF _temp5
    XORWF _usb_target_status, W
    BTFSC ZERO
    RETLW 0x00
    DECFSZ _timeout_low, F
    GOTO UWS_LOOP
    DECFSZ _timeout_high, F
    GOTO UWS_LOOP
    BSF F_TIMEOUT    ;SET FLAG TO INDICATE A TIMEOUT
    MOVF _temp5, W
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
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
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
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
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

global _usb_write_test_data
_usb_write_test_data
    CLRF _s_reg_low
    CLRF _s_reg_high
    BCF F_SRL_VALID ;INITIALIZE PSUEDO-RANDOM DATA GENERATOR
    CALL _usb_file_create
    MOVLW 0xB8
    MOVWF _timeout_low
    MOVLW 0x0B
    MOVWF _timeout_high  ;SET TIMEOUT TO 3 SECONDS
    MOVLW 0x14
    MOVWF _usb_target_status ;SET TARGET STATUS TO 0x14
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    MOVLW 0x04	;WE WILL WRITE 4 BLOCKS OF 32KB
    MOVWF _gen_count6
UWTD_L4
    CLRF _usb_bytes_sent_high
    CLRF _usb_bytes_sent_low
    MOVLW 0x80	;SET FILE SIZE TO 32KB
    MOVWF _usb_file_size_high
    CLRF _usb_file_size_low
    CALL USB_BYTE_WRITE
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low	;3 SECOND TIMEOUT
    MOVLW 0x1E
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
UWTD_ND_LOOP
    BCF USB_CS
    MOVLW 0x2D
    CALL SPI_TRANSFER   ;SEND WR_REQ_DATA
    CLRW
    CALL SPI_TRANSFER   ;GET NUMBER OF BYTES TO SEND
    MOVWF _gen_count5
UWTD_WR_LOOP
    CALL GET_TEST_BYTE
    CALL SPI_TRANSFER
    INCFSZ _usb_bytes_sent_low, F
    DECF _usb_bytes_sent_high, F
    INCF _usb_bytes_sent_high, F
    MOVF _usb_bytes_sent_high, W
    XORWF _usb_file_size_high, W
    BTFSS ZERO
    GOTO UWTD_NB
    MOVF _usb_bytes_sent_low, W
    XORWF _usb_file_size_low, W
    BTFSC ZERO
    GOTO UWTD_BREAK
UWTD_NB	;GO HERE IF BYTES SENT != FILE SIZE
    DECFSZ _gen_count5, F
    GOTO UWTD_WR_LOOP
UWTD_BREAK  ;GO HERE IF BYTES SENT == FILE SIZE
    BSF USB_CS
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low	;3 SECOND TIMEOUT
    MOVLW 0x1E
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    MOVF _usb_bytes_sent_high, W
    XORWF _usb_file_size_high, W
    BTFSS ZERO
    GOTO UWTD_NE
    MOVF _usb_bytes_sent_low, W
    XORWF _usb_file_size_low, W
    BTFSC ZERO
    GOTO UWTD_EQUAL
UWTD_NE
    CALL USB_BYTE_WR_GO
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low	;3 SECOND TIMEOUT
    MOVLW 0x1E
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    GOTO UWTD_ND_LOOP
UWTD_EQUAL
    CALL USB_BYTE_WR_GO
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low	;3 SECOND TIMEOUT
    MOVLW 0x14
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    DECFSZ _gen_count6, F
    GOTO UWTD_L4
    CALL _usb_file_close
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low	;3 SECOND TIMEOUT
    MOVLW 0x14
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    RETURN

global _usb_file_write
_usb_file_write
    CALL _fifo_init
    CALL _usb_set_file_name
    MOVLW 0x10
    MOVWF _timeout_low
    CLRF _timeout_high
    MOVLW 0x14
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    CALL _usb_file_create
    MOVLW 0xB8
    MOVWF _timeout_low
    MOVLW 0x0B
    MOVWF _timeout_high  ;SET TIMEOUT TO 3 SECONDS
    MOVLW 0x14
    MOVWF _usb_target_status ;SET TARGET STATUS TO 0x14
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    CLRF _usb_bytes_sent_high
    CLRF _usb_bytes_sent_low
    MOVF _tape_size_high, W
    MOVWF _usb_file_size_high
    MOVF _tape_size_low, W
    MOVWF _usb_file_size_low
    CALL USB_BYTE_WRITE
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low
    MOVLW 0x1E
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
UFW_ND_LOOP
    ;CALL BUFF_INIT  ;TODO: CHANGE THIS TO USE FIFO
    MOVF _usb_bytes_sent_low, W
    MOVWF _ram_address_l
    MOVF _usb_bytes_sent_high, W
    MOVWF _ram_address_h
    DECF _ram_address_l, F
    BTFSS CARRY
    DECF _ram_address_h, F
    CALL _fifo_tf_from_mem
    BCF USB_CS
    MOVLW 0x2D
    CALL SPI_TRANSFER   ;SEND WR_REQ_DATA
    CLRW
    CALL SPI_TRANSFER   ;GET NUMBER OF BYTES TO SEND
    MOVWF _gen_count5
UFW_WR_LOOP
    CALL _fifo_pop
    CALL SPI_TRANSFER
    INCFSZ _usb_bytes_sent_low, F
    DECF _usb_bytes_sent_high, F
    INCF _usb_bytes_sent_high, F
    MOVF _usb_bytes_sent_high, W
    XORWF _usb_file_size_high, W
    BTFSS ZERO
    GOTO UFW_NB
    MOVF _usb_bytes_sent_low, W
    XORWF _usb_file_size_low, W
    BTFSC ZERO
    GOTO UFW_BREAK
UFW_NB
    DECFSZ _gen_count5, F
    GOTO UFW_WR_LOOP
UFW_BREAK
    BSF USB_CS
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low
    MOVLW 0x1E
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    MOVF _usb_bytes_sent_high, W
    XORWF _usb_file_size_high, W
    BTFSS ZERO
    GOTO UFW_NE
    MOVF _usb_bytes_sent_low, W
    XORWF _usb_file_size_low, W
    BTFSC ZERO
    GOTO UFW_EQUAL
UFW_NE
    CALL USB_BYTE_WR_GO
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low
    MOVLW 0x1E
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    GOTO UFW_ND_LOOP
UFW_EQUAL
    CALL USB_BYTE_WR_GO
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low
    MOVLW 0x14
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
    CALL _usb_file_close
    MOVLW 0x0B
    MOVWF _timeout_high
    MOVLW 0xB8
    MOVWF _timeout_low
    MOVLW 0x14
    MOVWF _usb_target_status
    CALL USB_WAIT_STATUS
    RETURN

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
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
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
    BTFSC F_TIMEOUT   ;CHECK TIMEOUT FLAG
    RETURN
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
