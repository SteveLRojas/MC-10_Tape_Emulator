
# 1 "main.c"


# 8
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = ON
#pragma config BOREN = ON
#pragma config BORV = 42
#pragma config CCP2MX = RC1
#pragma config CP = OFF


#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config BORSEN = OFF

# 18 "/opt/microchip/xc8/v2.05/pic/include/xc.h"
extern const char __xc8_OPTIM_SPEED;

extern double __fpnormalize(double);


# 13 "/opt/microchip/xc8/v2.05/pic/include/c90/xc8debug.h"
#pragma intrinsic(__builtin_software_breakpoint)
extern void __builtin_software_breakpoint(void);

# 52 "/opt/microchip/xc8/v2.05/pic/include/pic16f747.h"
extern volatile unsigned char INDF __at(0x000);

asm("INDF equ 00h");




extern volatile unsigned char TMR0 __at(0x001);

asm("TMR0 equ 01h");




extern volatile unsigned char PCL __at(0x002);

asm("PCL equ 02h");




extern volatile unsigned char STATUS __at(0x003);

asm("STATUS equ 03h");


typedef union {
struct {
unsigned C :1;
unsigned DC :1;
unsigned Z :1;
unsigned nPD :1;
unsigned nTO :1;
unsigned RP :2;
unsigned IRP :1;
};
struct {
unsigned :5;
unsigned RP0 :1;
unsigned RP1 :1;
};
struct {
unsigned CARRY :1;
unsigned :1;
unsigned ZERO :1;
};
} STATUSbits_t;
extern volatile STATUSbits_t STATUSbits __at(0x003);

# 159
extern volatile unsigned char FSR __at(0x004);

asm("FSR equ 04h");




extern volatile unsigned char PORTA __at(0x005);

asm("PORTA equ 05h");


typedef union {
struct {
unsigned RA0 :1;
unsigned RA1 :1;
unsigned RA2 :1;
unsigned RA3 :1;
unsigned RA4 :1;
unsigned RA5 :1;
unsigned RA6 :1;
unsigned RA7 :1;
};
} PORTAbits_t;
extern volatile PORTAbits_t PORTAbits __at(0x005);

# 228
extern volatile unsigned char PORTB __at(0x006);

asm("PORTB equ 06h");


typedef union {
struct {
unsigned RB0 :1;
unsigned RB1 :1;
unsigned RB2 :1;
unsigned RB3 :1;
unsigned RB4 :1;
unsigned RB5 :1;
unsigned RB6 :1;
unsigned RB7 :1;
};
} PORTBbits_t;
extern volatile PORTBbits_t PORTBbits __at(0x006);

# 290
extern volatile unsigned char PORTC __at(0x007);

asm("PORTC equ 07h");


typedef union {
struct {
unsigned RC0 :1;
unsigned RC1 :1;
unsigned RC2 :1;
unsigned RC3 :1;
unsigned RC4 :1;
unsigned RC5 :1;
unsigned RC6 :1;
unsigned RC7 :1;
};
} PORTCbits_t;
extern volatile PORTCbits_t PORTCbits __at(0x007);

# 352
extern volatile unsigned char PORTD __at(0x008);

asm("PORTD equ 08h");


typedef union {
struct {
unsigned RD0 :1;
unsigned RD1 :1;
unsigned RD2 :1;
unsigned RD3 :1;
unsigned RD4 :1;
unsigned RD5 :1;
unsigned RD6 :1;
unsigned RD7 :1;
};
} PORTDbits_t;
extern volatile PORTDbits_t PORTDbits __at(0x008);

# 414
extern volatile unsigned char PORTE __at(0x009);

asm("PORTE equ 09h");


typedef union {
struct {
unsigned RE0 :1;
unsigned RE1 :1;
unsigned RE2 :1;
unsigned RE3 :1;
};
} PORTEbits_t;
extern volatile PORTEbits_t PORTEbits __at(0x009);

# 452
extern volatile unsigned char PCLATH __at(0x00A);

asm("PCLATH equ 0Ah");


typedef union {
struct {
unsigned PCLATH :5;
};
} PCLATHbits_t;
extern volatile PCLATHbits_t PCLATHbits __at(0x00A);

# 472
extern volatile unsigned char INTCON __at(0x00B);

asm("INTCON equ 0Bh");


typedef union {
struct {
unsigned RBIF :1;
unsigned INTF :1;
unsigned TMR0IF :1;
unsigned RBIE :1;
unsigned INTE :1;
unsigned TMR0IE :1;
unsigned PEIE :1;
unsigned GIE :1;
};
struct {
unsigned :1;
unsigned INT0IF :1;
unsigned T0IF :1;
unsigned :1;
unsigned INT0IE :1;
unsigned T0IE :1;
};
} INTCONbits_t;
extern volatile INTCONbits_t INTCONbits __at(0x00B);

# 562
extern volatile unsigned char PIR1 __at(0x00C);

asm("PIR1 equ 0Ch");


typedef union {
struct {
unsigned TMR1IF :1;
unsigned TMR2IF :1;
unsigned CCP1IF :1;
unsigned SSPIF :1;
unsigned TXIF :1;
unsigned RCIF :1;
unsigned ADIF :1;
unsigned PSPIF :1;
};
} PIR1bits_t;
extern volatile PIR1bits_t PIR1bits __at(0x00C);

# 624
extern volatile unsigned char PIR2 __at(0x00D);

asm("PIR2 equ 0Dh");


typedef union {
struct {
unsigned CCP2IF :1;
unsigned CCP3IF :1;
unsigned :1;
unsigned BCLIF :1;
unsigned :1;
unsigned LVDIF :1;
unsigned CMIF :1;
unsigned OSFIF :1;
};
} PIR2bits_t;
extern volatile PIR2bits_t PIR2bits __at(0x00D);

# 676
extern volatile unsigned short TMR1 __at(0x00E);

asm("TMR1 equ 0Eh");




extern volatile unsigned char TMR1L __at(0x00E);

asm("TMR1L equ 0Eh");




extern volatile unsigned char TMR1H __at(0x00F);

asm("TMR1H equ 0Fh");




extern volatile unsigned char T1CON __at(0x010);

asm("T1CON equ 010h");


typedef union {
struct {
unsigned TMR1ON :1;
unsigned TMR1CS :1;
unsigned nT1SYNC :1;
unsigned T1OSCEN :1;
unsigned T1CKPS :2;
unsigned T1RUN :1;
};
struct {
unsigned :2;
unsigned T1SYNC :1;
unsigned :1;
unsigned T1CKPS0 :1;
unsigned T1CKPS1 :1;
};
struct {
unsigned :2;
unsigned T1INSYNC :1;
};
} T1CONbits_t;
extern volatile T1CONbits_t T1CONbits __at(0x010);

# 778
extern volatile unsigned char TMR2 __at(0x011);

asm("TMR2 equ 011h");




extern volatile unsigned char T2CON __at(0x012);

asm("T2CON equ 012h");


typedef union {
struct {
unsigned T2CKPS :2;
unsigned TMR2ON :1;
unsigned TOUTPS :4;
};
struct {
unsigned T2CKPS0 :1;
unsigned T2CKPS1 :1;
unsigned :1;
unsigned TOUTPS0 :1;
unsigned TOUTPS1 :1;
unsigned TOUTPS2 :1;
unsigned TOUTPS3 :1;
};
} T2CONbits_t;
extern volatile T2CONbits_t T2CONbits __at(0x012);

# 856
extern volatile unsigned char SSPBUF __at(0x013);

asm("SSPBUF equ 013h");




extern volatile unsigned char SSPCON __at(0x014);

asm("SSPCON equ 014h");


typedef union {
struct {
unsigned SSPM :4;
unsigned CKP :1;
unsigned SSPEN :1;
unsigned SSPOV :1;
unsigned WCOL :1;
};
struct {
unsigned SSPM0 :1;
unsigned SSPM1 :1;
unsigned SSPM2 :1;
unsigned SSPM3 :1;
};
} SSPCONbits_t;
extern volatile SSPCONbits_t SSPCONbits __at(0x014);

# 933
extern volatile unsigned short CCPR1 __at(0x015);

asm("CCPR1 equ 015h");




extern volatile unsigned char CCPR1L __at(0x015);

asm("CCPR1L equ 015h");




extern volatile unsigned char CCPR1H __at(0x016);

asm("CCPR1H equ 016h");




extern volatile unsigned char CCP1CON __at(0x017);

asm("CCP1CON equ 017h");


typedef union {
struct {
unsigned CCP1M :4;
unsigned CCP1Y :1;
unsigned CCP1X :1;
};
struct {
unsigned CCP1M0 :1;
unsigned CCP1M1 :1;
unsigned CCP1M2 :1;
unsigned CCP1M3 :1;
};
} CCP1CONbits_t;
extern volatile CCP1CONbits_t CCP1CONbits __at(0x017);

# 1012
extern volatile unsigned char RCSTA __at(0x018);

asm("RCSTA equ 018h");


typedef union {
struct {
unsigned RX9D :1;
unsigned OERR :1;
unsigned FERR :1;
unsigned ADDEN :1;
unsigned CREN :1;
unsigned SREN :1;
unsigned RX9 :1;
unsigned SPEN :1;
};
struct {
unsigned RCD8 :1;
unsigned :5;
unsigned RC9 :1;
};
struct {
unsigned :6;
unsigned nRC8 :1;
};
struct {
unsigned :6;
unsigned RC8_9 :1;
};
} RCSTAbits_t;
extern volatile RCSTAbits_t RCSTAbits __at(0x018);

# 1107
extern volatile unsigned char TXREG __at(0x019);

asm("TXREG equ 019h");




extern volatile unsigned char RCREG __at(0x01A);

asm("RCREG equ 01Ah");




extern volatile unsigned short CCPR2 __at(0x01B);

asm("CCPR2 equ 01Bh");




extern volatile unsigned char CCPR2L __at(0x01B);

asm("CCPR2L equ 01Bh");




extern volatile unsigned char CCPR2H __at(0x01C);

asm("CCPR2H equ 01Ch");




extern volatile unsigned char CCP2CON __at(0x01D);

asm("CCP2CON equ 01Dh");


typedef union {
struct {
unsigned CCP2M :4;
unsigned CCP2Y :1;
unsigned CCP2X :1;
};
struct {
unsigned CCP2M0 :1;
unsigned CCP2M1 :1;
unsigned CCP2M2 :1;
unsigned CCP2M3 :1;
};
} CCP2CONbits_t;
extern volatile CCP2CONbits_t CCP2CONbits __at(0x01D);

# 1200
extern volatile unsigned char ADRESH __at(0x01E);

asm("ADRESH equ 01Eh");




extern volatile unsigned char ADCON0 __at(0x01F);

asm("ADCON0 equ 01Fh");


typedef union {
struct {
unsigned ADON :1;
unsigned CHS3 :1;
unsigned GO_nDONE :1;
unsigned CHS :3;
unsigned ADCS :2;
};
struct {
unsigned :2;
unsigned GO :1;
unsigned CHS0 :1;
unsigned CHS1 :1;
unsigned CHS2 :1;
unsigned ADCS0 :1;
unsigned ADCS1 :1;
};
struct {
unsigned :2;
unsigned nDONE :1;
};
struct {
unsigned :2;
unsigned GO_DONE :1;
};
} ADCON0bits_t;
extern volatile ADCON0bits_t ADCON0bits __at(0x01F);

# 1308
extern volatile unsigned char OPTION_REG __at(0x081);

asm("OPTION_REG equ 081h");


typedef union {
struct {
unsigned PS :3;
unsigned PSA :1;
unsigned T0SE :1;
unsigned T0CS :1;
unsigned INTEDG :1;
unsigned nRBPU :1;
};
struct {
unsigned PS0 :1;
unsigned PS1 :1;
unsigned PS2 :1;
};
} OPTION_REGbits_t;
extern volatile OPTION_REGbits_t OPTION_REGbits __at(0x081);

# 1378
extern volatile unsigned char TRISA __at(0x085);

asm("TRISA equ 085h");


typedef union {
struct {
unsigned TRISA0 :1;
unsigned TRISA1 :1;
unsigned TRISA2 :1;
unsigned TRISA3 :1;
unsigned TRISA4 :1;
unsigned TRISA5 :1;
unsigned TRISA6 :1;
unsigned TRISA7 :1;
};
} TRISAbits_t;
extern volatile TRISAbits_t TRISAbits __at(0x085);

# 1440
extern volatile unsigned char TRISB __at(0x086);

asm("TRISB equ 086h");


typedef union {
struct {
unsigned TRISB0 :1;
unsigned TRISB1 :1;
unsigned TRISB2 :1;
unsigned TRISB3 :1;
unsigned TRISB4 :1;
unsigned TRISB5 :1;
unsigned TRISB6 :1;
unsigned TRISB7 :1;
};
} TRISBbits_t;
extern volatile TRISBbits_t TRISBbits __at(0x086);

# 1502
extern volatile unsigned char TRISC __at(0x087);

asm("TRISC equ 087h");


typedef union {
struct {
unsigned TRISC0 :1;
unsigned TRISC1 :1;
unsigned TRISC2 :1;
unsigned TRISC3 :1;
unsigned TRISC4 :1;
unsigned TRISC5 :1;
unsigned TRISC6 :1;
unsigned TRISC7 :1;
};
} TRISCbits_t;
extern volatile TRISCbits_t TRISCbits __at(0x087);

# 1564
extern volatile unsigned char TRISD __at(0x088);

asm("TRISD equ 088h");


typedef union {
struct {
unsigned TRISD0 :1;
unsigned TRISD1 :1;
unsigned TRISD2 :1;
unsigned TRISD3 :1;
unsigned TRISD4 :1;
unsigned TRISD5 :1;
unsigned TRISD6 :1;
unsigned TRISD7 :1;
};
} TRISDbits_t;
extern volatile TRISDbits_t TRISDbits __at(0x088);

# 1626
extern volatile unsigned char TRISE __at(0x089);

asm("TRISE equ 089h");


typedef union {
struct {
unsigned TRISE0 :1;
unsigned TRISE1 :1;
unsigned TRISE2 :1;
unsigned TRISE3 :1;
unsigned PSPMODE :1;
unsigned IBOV :1;
unsigned OBF :1;
unsigned IBF :1;
};
} TRISEbits_t;
extern volatile TRISEbits_t TRISEbits __at(0x089);

# 1688
extern volatile unsigned char PIE1 __at(0x08C);

asm("PIE1 equ 08Ch");


typedef union {
struct {
unsigned TMR1IE :1;
unsigned TMR2IE :1;
unsigned CCP1IE :1;
unsigned SSPIE :1;
unsigned TXIE :1;
unsigned RCIE :1;
unsigned ADIE :1;
unsigned PSPIE :1;
};
} PIE1bits_t;
extern volatile PIE1bits_t PIE1bits __at(0x08C);

# 1750
extern volatile unsigned char PIE2 __at(0x08D);

asm("PIE2 equ 08Dh");


typedef union {
struct {
unsigned CCP2IE :1;
unsigned CCP3IE :1;
unsigned :1;
unsigned BCLIE :1;
unsigned :1;
unsigned LVDIE :1;
unsigned CMIE :1;
unsigned OSFIE :1;
};
} PIE2bits_t;
extern volatile PIE2bits_t PIE2bits __at(0x08D);

# 1802
extern volatile unsigned char PCON __at(0x08E);

asm("PCON equ 08Eh");


typedef union {
struct {
unsigned nBOR :1;
unsigned nPOR :1;
unsigned SBOREN :1;
};
struct {
unsigned nBO :1;
};
} PCONbits_t;
extern volatile PCONbits_t PCONbits __at(0x08E);

# 1842
extern volatile unsigned char OSCCON __at(0x08F);

asm("OSCCON equ 08Fh");


typedef union {
struct {
unsigned SCS :2;
unsigned IOFS :1;
unsigned OSTS :1;
unsigned IRCF :3;
};
struct {
unsigned SCS0 :1;
unsigned SCS1 :1;
unsigned :2;
unsigned IRCF0 :1;
unsigned IRCF1 :1;
unsigned IRCF2 :1;
};
} OSCCONbits_t;
extern volatile OSCCONbits_t OSCCONbits __at(0x08F);

# 1913
extern volatile unsigned char OSCTUNE __at(0x090);

asm("OSCTUNE equ 090h");


typedef union {
struct {
unsigned TUN :6;
};
struct {
unsigned TUN0 :1;
unsigned TUN1 :1;
unsigned TUN2 :1;
unsigned TUN3 :1;
unsigned TUN4 :1;
unsigned TUN5 :1;
};
} OSCTUNEbits_t;
extern volatile OSCTUNEbits_t OSCTUNEbits __at(0x090);

# 1971
extern volatile unsigned char SSPCON2 __at(0x091);

asm("SSPCON2 equ 091h");


typedef union {
struct {
unsigned SEN :1;
unsigned RSEN :1;
unsigned PEN :1;
unsigned RCEN :1;
unsigned ACKEN :1;
unsigned ACKDT :1;
unsigned ACKSTAT :1;
unsigned GCEN :1;
};
} SSPCON2bits_t;
extern volatile SSPCON2bits_t SSPCON2bits __at(0x091);

# 2033
extern volatile unsigned char PR2 __at(0x092);

asm("PR2 equ 092h");




extern volatile unsigned char SSPADD __at(0x093);

asm("SSPADD equ 093h");




extern volatile unsigned char SSPSTAT __at(0x094);

asm("SSPSTAT equ 094h");


typedef union {
struct {
unsigned BF :1;
unsigned UA :1;
unsigned R_nW :1;
unsigned S :1;
unsigned P :1;
unsigned D_nA :1;
unsigned CKE :1;
unsigned SMP :1;
};
struct {
unsigned :2;
unsigned R :1;
unsigned :2;
unsigned D :1;
};
struct {
unsigned :2;
unsigned I2C_READ :1;
unsigned I2C_START :1;
unsigned I2C_STOP :1;
unsigned I2C_DATA :1;
};
struct {
unsigned :2;
unsigned nW :1;
unsigned :2;
unsigned nA :1;
};
struct {
unsigned :2;
unsigned nWRITE :1;
unsigned :2;
unsigned nADDRESS :1;
};
struct {
unsigned :2;
unsigned R_W :1;
unsigned :2;
unsigned D_A :1;
};
struct {
unsigned :2;
unsigned READ_WRITE :1;
unsigned :2;
unsigned DATA_ADDRESS :1;
};
} SSPSTATbits_t;
extern volatile SSPSTATbits_t SSPSTATbits __at(0x094);

# 2216
extern volatile unsigned short CCPR3 __at(0x095);

asm("CCPR3 equ 095h");




extern volatile unsigned char CCPR3L __at(0x095);

asm("CCPR3L equ 095h");


typedef union {
struct {
unsigned CCPR3L :8;
};
} CCPR3Lbits_t;
extern volatile CCPR3Lbits_t CCPR3Lbits __at(0x095);

# 2243
extern volatile unsigned char CCPR3H __at(0x096);

asm("CCPR3H equ 096h");


typedef union {
struct {
unsigned CCPR3H :8;
};
} CCPR3Hbits_t;
extern volatile CCPR3Hbits_t CCPR3Hbits __at(0x096);

# 2263
extern volatile unsigned char CCP3CON __at(0x097);

asm("CCP3CON equ 097h");


typedef union {
struct {
unsigned CCP3M :4;
unsigned CCP3Y :1;
unsigned CCP3X :1;
};
struct {
unsigned CCP3M0 :1;
unsigned CCP3M1 :1;
unsigned CCP3M2 :1;
unsigned CCP3M3 :1;
};
} CCP3CONbits_t;
extern volatile CCP3CONbits_t CCP3CONbits __at(0x097);

# 2321
extern volatile unsigned char TXSTA __at(0x098);

asm("TXSTA equ 098h");


typedef union {
struct {
unsigned TX9D :1;
unsigned TRMT :1;
unsigned BRGH :1;
unsigned :1;
unsigned SYNC :1;
unsigned TXEN :1;
unsigned TX9 :1;
unsigned CSRC :1;
};
struct {
unsigned TXD8 :1;
unsigned :5;
unsigned nTX8 :1;
};
struct {
unsigned :6;
unsigned TX8_9 :1;
};
} TXSTAbits_t;
extern volatile TXSTAbits_t TXSTAbits __at(0x098);

# 2402
extern volatile unsigned char SPBRG __at(0x099);

asm("SPBRG equ 099h");




extern volatile unsigned char ADCON2 __at(0x09B);

asm("ADCON2 equ 09Bh");


typedef union {
struct {
unsigned :3;
unsigned ACQT :3;
};
struct {
unsigned :3;
unsigned ACQT0 :1;
unsigned ACQT1 :1;
unsigned ACQT2 :1;
};
} ADCON2bits_t;
extern volatile ADCON2bits_t ADCON2bits __at(0x09B);

# 2451
extern volatile unsigned char CMCON __at(0x09C);

asm("CMCON equ 09Ch");


typedef union {
struct {
unsigned CM :3;
unsigned CIS :1;
unsigned C1INV :1;
unsigned C2INV :1;
unsigned C1OUT :1;
unsigned C2OUT :1;
};
struct {
unsigned CM0 :1;
unsigned CM1 :1;
unsigned CM2 :1;
};
} CMCONbits_t;
extern volatile CMCONbits_t CMCONbits __at(0x09C);

# 2521
extern volatile unsigned char CVRCON __at(0x09D);

asm("CVRCON equ 09Dh");


typedef union {
struct {
unsigned CVR :4;
unsigned :1;
unsigned CVRR :1;
unsigned CVROE :1;
unsigned CVREN :1;
};
struct {
unsigned CVR0 :1;
unsigned CVR1 :1;
unsigned CVR2 :1;
unsigned CVR3 :1;
};
} CVRCONbits_t;
extern volatile CVRCONbits_t CVRCONbits __at(0x09D);

# 2586
extern volatile unsigned char ADRESL __at(0x09E);

asm("ADRESL equ 09Eh");




extern volatile unsigned char ADCON1 __at(0x09F);

asm("ADCON1 equ 09Fh");


typedef union {
struct {
unsigned PCFG :4;
unsigned VCFG :2;
unsigned ADCS2 :1;
unsigned ADFM :1;
};
struct {
unsigned PCFG0 :1;
unsigned PCFG1 :1;
unsigned PCFG2 :1;
unsigned PCFG3 :1;
unsigned VCFG0 :1;
unsigned VCFG1 :1;
};
} ADCON1bits_t;
extern volatile ADCON1bits_t ADCON1bits __at(0x09F);

# 2669
extern volatile unsigned char WDTCON __at(0x105);

asm("WDTCON equ 0105h");


typedef union {
struct {
unsigned SWDTEN :1;
unsigned WDTPS :4;
};
struct {
unsigned SWDTE :1;
unsigned WDTPS0 :1;
unsigned WDTPS1 :1;
unsigned WDTPS2 :1;
unsigned WDTPS3 :1;
};
} WDTCONbits_t;
extern volatile WDTCONbits_t WDTCONbits __at(0x105);

# 2727
extern volatile unsigned char LVDCON __at(0x109);

asm("LVDCON equ 0109h");


typedef union {
struct {
unsigned LVDL :4;
unsigned LVDEN :1;
unsigned IRVST :1;
};
struct {
unsigned LVDL0 :1;
unsigned LVDL1 :1;
unsigned LVDL2 :1;
unsigned LVDL3 :1;
};
} LVDCONbits_t;
extern volatile LVDCONbits_t LVDCONbits __at(0x109);

# 2785
extern volatile unsigned char PMDATA __at(0x10C);

asm("PMDATA equ 010Ch");




extern volatile unsigned char PMADR __at(0x10D);

asm("PMADR equ 010Dh");




extern volatile unsigned char PMDATH __at(0x10E);

asm("PMDATH equ 010Eh");




extern volatile unsigned char PMADRH __at(0x10F);

asm("PMADRH equ 010Fh");




extern volatile unsigned char PMCON1 __at(0x18C);

asm("PMCON1 equ 018Ch");


typedef union {
struct {
unsigned RD :1;
};
} PMCON1bits_t;
extern volatile PMCON1bits_t PMCON1bits __at(0x18C);

# 2839
extern volatile __bit ACKDT __at(0x48D);


extern volatile __bit ACKEN __at(0x48C);


extern volatile __bit ACKSTAT __at(0x48E);


extern volatile __bit ACQT0 __at(0x4DB);


extern volatile __bit ACQT1 __at(0x4DC);


extern volatile __bit ACQT2 __at(0x4DD);


extern volatile __bit ADCS0 __at(0xFE);


extern volatile __bit ADCS1 __at(0xFF);


extern volatile __bit ADCS2 __at(0x4FE);


extern volatile __bit ADDEN __at(0xC3);


extern volatile __bit ADFM __at(0x4FF);


extern volatile __bit ADIE __at(0x466);


extern volatile __bit ADIF __at(0x66);


extern volatile __bit ADON __at(0xF8);


extern volatile __bit BCLIE __at(0x46B);


extern volatile __bit BCLIF __at(0x6B);


extern volatile __bit BF __at(0x4A0);


extern volatile __bit BRGH __at(0x4C2);


extern volatile __bit C1INV __at(0x4E4);


extern volatile __bit C1OUT __at(0x4E6);


extern volatile __bit C2INV __at(0x4E5);


extern volatile __bit C2OUT __at(0x4E7);


extern volatile __bit CARRY __at(0x18);


extern volatile __bit CCP1IE __at(0x462);


extern volatile __bit CCP1IF __at(0x62);


extern volatile __bit CCP1M0 __at(0xB8);


extern volatile __bit CCP1M1 __at(0xB9);


extern volatile __bit CCP1M2 __at(0xBA);


extern volatile __bit CCP1M3 __at(0xBB);


extern volatile __bit CCP1X __at(0xBD);


extern volatile __bit CCP1Y __at(0xBC);


extern volatile __bit CCP2IE __at(0x468);


extern volatile __bit CCP2IF __at(0x68);


extern volatile __bit CCP2M0 __at(0xE8);


extern volatile __bit CCP2M1 __at(0xE9);


extern volatile __bit CCP2M2 __at(0xEA);


extern volatile __bit CCP2M3 __at(0xEB);


extern volatile __bit CCP2X __at(0xED);


extern volatile __bit CCP2Y __at(0xEC);


extern volatile __bit CCP3IE __at(0x469);


extern volatile __bit CCP3IF __at(0x69);


extern volatile __bit CCP3M0 __at(0x4B8);


extern volatile __bit CCP3M1 __at(0x4B9);


extern volatile __bit CCP3M2 __at(0x4BA);


extern volatile __bit CCP3M3 __at(0x4BB);


extern volatile __bit CCP3X __at(0x4BD);


extern volatile __bit CCP3Y __at(0x4BC);


extern volatile __bit CHS0 __at(0xFB);


extern volatile __bit CHS1 __at(0xFC);


extern volatile __bit CHS2 __at(0xFD);


extern volatile __bit CHS3 __at(0xF9);


extern volatile __bit CIS __at(0x4E3);


extern volatile __bit CKE __at(0x4A6);


extern volatile __bit CKP __at(0xA4);


extern volatile __bit CM0 __at(0x4E0);


extern volatile __bit CM1 __at(0x4E1);


extern volatile __bit CM2 __at(0x4E2);


extern volatile __bit CMIE __at(0x46E);


extern volatile __bit CMIF __at(0x6E);


extern volatile __bit CREN __at(0xC4);


extern volatile __bit CSRC __at(0x4C7);


extern volatile __bit CVR0 __at(0x4E8);


extern volatile __bit CVR1 __at(0x4E9);


extern volatile __bit CVR2 __at(0x4EA);


extern volatile __bit CVR3 __at(0x4EB);


extern volatile __bit CVREN __at(0x4EF);


extern volatile __bit CVROE __at(0x4EE);


extern volatile __bit CVRR __at(0x4ED);


extern volatile __bit DATA_ADDRESS __at(0x4A5);


extern volatile __bit DC __at(0x19);


extern volatile __bit D_A __at(0x4A5);


extern volatile __bit D_nA __at(0x4A5);


extern volatile __bit FERR __at(0xC2);


extern volatile __bit GCEN __at(0x48F);


extern volatile __bit GIE __at(0x5F);


extern volatile __bit GO __at(0xFA);


extern volatile __bit GO_DONE __at(0xFA);


extern volatile __bit GO_nDONE __at(0xFA);


extern volatile __bit I2C_DATA __at(0x4A5);


extern volatile __bit I2C_READ __at(0x4A2);


extern volatile __bit I2C_START __at(0x4A3);


extern volatile __bit I2C_STOP __at(0x4A4);


extern volatile __bit IBF __at(0x44F);


extern volatile __bit IBOV __at(0x44D);


extern volatile __bit INT0IE __at(0x5C);


extern volatile __bit INT0IF __at(0x59);


extern volatile __bit INTE __at(0x5C);


extern volatile __bit INTEDG __at(0x40E);


extern volatile __bit INTF __at(0x59);


extern volatile __bit IOFS __at(0x47A);


extern volatile __bit IRCF0 __at(0x47C);


extern volatile __bit IRCF1 __at(0x47D);


extern volatile __bit IRCF2 __at(0x47E);


extern volatile __bit IRP __at(0x1F);


extern volatile __bit IRVST __at(0x84D);


extern volatile __bit LVDEN __at(0x84C);


extern volatile __bit LVDIE __at(0x46D);


extern volatile __bit LVDIF __at(0x6D);


extern volatile __bit LVDL0 __at(0x848);


extern volatile __bit LVDL1 __at(0x849);


extern volatile __bit LVDL2 __at(0x84A);


extern volatile __bit LVDL3 __at(0x84B);


extern volatile __bit OBF __at(0x44E);


extern volatile __bit OERR __at(0xC1);


extern volatile __bit OSFIE __at(0x46F);


extern volatile __bit OSFIF __at(0x6F);


extern volatile __bit OSTS __at(0x47B);


extern volatile __bit PCFG0 __at(0x4F8);


extern volatile __bit PCFG1 __at(0x4F9);


extern volatile __bit PCFG2 __at(0x4FA);


extern volatile __bit PCFG3 __at(0x4FB);


extern volatile __bit PEIE __at(0x5E);


extern volatile __bit PEN __at(0x48A);


extern volatile __bit PS0 __at(0x408);


extern volatile __bit PS1 __at(0x409);


extern volatile __bit PS2 __at(0x40A);


extern volatile __bit PSA __at(0x40B);


extern volatile __bit PSPIE __at(0x467);


extern volatile __bit PSPIF __at(0x67);


extern volatile __bit PSPMODE __at(0x44C);


extern volatile __bit RA0 __at(0x28);


extern volatile __bit RA1 __at(0x29);


extern volatile __bit RA2 __at(0x2A);


extern volatile __bit RA3 __at(0x2B);


extern volatile __bit RA4 __at(0x2C);


extern volatile __bit RA5 __at(0x2D);


extern volatile __bit RA6 __at(0x2E);


extern volatile __bit RA7 __at(0x2F);


extern volatile __bit RB0 __at(0x30);


extern volatile __bit RB1 __at(0x31);


extern volatile __bit RB2 __at(0x32);


extern volatile __bit RB3 __at(0x33);


extern volatile __bit RB4 __at(0x34);


extern volatile __bit RB5 __at(0x35);


extern volatile __bit RB6 __at(0x36);


extern volatile __bit RB7 __at(0x37);


extern volatile __bit RBIE __at(0x5B);


extern volatile __bit RBIF __at(0x58);


extern volatile __bit RC0 __at(0x38);


extern volatile __bit RC1 __at(0x39);


extern volatile __bit RC2 __at(0x3A);


extern volatile __bit RC3 __at(0x3B);


extern volatile __bit RC4 __at(0x3C);


extern volatile __bit RC5 __at(0x3D);


extern volatile __bit RC6 __at(0x3E);


extern volatile __bit RC7 __at(0x3F);


extern volatile __bit RC8_9 __at(0xC6);


extern volatile __bit RC9 __at(0xC6);


extern volatile __bit RCD8 __at(0xC0);


extern volatile __bit RCEN __at(0x48B);


extern volatile __bit RCIE __at(0x465);


extern volatile __bit RCIF __at(0x65);


extern volatile __bit RD __at(0xC60);


extern volatile __bit RD0 __at(0x40);


extern volatile __bit RD1 __at(0x41);


extern volatile __bit RD2 __at(0x42);


extern volatile __bit RD3 __at(0x43);


extern volatile __bit RD4 __at(0x44);


extern volatile __bit RD5 __at(0x45);


extern volatile __bit RD6 __at(0x46);


extern volatile __bit RD7 __at(0x47);


extern volatile __bit RE0 __at(0x48);


extern volatile __bit RE1 __at(0x49);


extern volatile __bit RE2 __at(0x4A);


extern volatile __bit RE3 __at(0x4B);


extern volatile __bit READ_WRITE __at(0x4A2);


extern volatile __bit RP0 __at(0x1D);


extern volatile __bit RP1 __at(0x1E);


extern volatile __bit RSEN __at(0x489);


extern volatile __bit RX9 __at(0xC6);


extern volatile __bit RX9D __at(0xC0);


extern volatile __bit R_W __at(0x4A2);


extern volatile __bit R_nW __at(0x4A2);


extern volatile __bit SBOREN __at(0x472);


extern volatile __bit SCS0 __at(0x478);


extern volatile __bit SCS1 __at(0x479);


extern volatile __bit SEN __at(0x488);


extern volatile __bit SMP __at(0x4A7);


extern volatile __bit SPEN __at(0xC7);


extern volatile __bit SREN __at(0xC5);


extern volatile __bit SSPEN __at(0xA5);


extern volatile __bit SSPIE __at(0x463);


extern volatile __bit SSPIF __at(0x63);


extern volatile __bit SSPM0 __at(0xA0);


extern volatile __bit SSPM1 __at(0xA1);


extern volatile __bit SSPM2 __at(0xA2);


extern volatile __bit SSPM3 __at(0xA3);


extern volatile __bit SSPOV __at(0xA6);


extern volatile __bit SWDTE __at(0x828);


extern volatile __bit SWDTEN __at(0x828);


extern volatile __bit SYNC __at(0x4C4);


extern volatile __bit T0CS __at(0x40D);


extern volatile __bit T0IE __at(0x5D);


extern volatile __bit T0IF __at(0x5A);


extern volatile __bit T0SE __at(0x40C);


extern volatile __bit T1CKPS0 __at(0x84);


extern volatile __bit T1CKPS1 __at(0x85);


extern volatile __bit T1INSYNC __at(0x82);


extern volatile __bit T1OSCEN __at(0x83);


extern volatile __bit T1RUN __at(0x86);


extern volatile __bit T1SYNC __at(0x82);


extern volatile __bit T2CKPS0 __at(0x90);


extern volatile __bit T2CKPS1 __at(0x91);


extern volatile __bit TMR0IE __at(0x5D);


extern volatile __bit TMR0IF __at(0x5A);


extern volatile __bit TMR1CS __at(0x81);


extern volatile __bit TMR1IE __at(0x460);


extern volatile __bit TMR1IF __at(0x60);


extern volatile __bit TMR1ON __at(0x80);


extern volatile __bit TMR2IE __at(0x461);


extern volatile __bit TMR2IF __at(0x61);


extern volatile __bit TMR2ON __at(0x92);


extern volatile __bit TOUTPS0 __at(0x93);


extern volatile __bit TOUTPS1 __at(0x94);


extern volatile __bit TOUTPS2 __at(0x95);


extern volatile __bit TOUTPS3 __at(0x96);


extern volatile __bit TRISA0 __at(0x428);


extern volatile __bit TRISA1 __at(0x429);


extern volatile __bit TRISA2 __at(0x42A);


extern volatile __bit TRISA3 __at(0x42B);


extern volatile __bit TRISA4 __at(0x42C);


extern volatile __bit TRISA5 __at(0x42D);


extern volatile __bit TRISA6 __at(0x42E);


extern volatile __bit TRISA7 __at(0x42F);


extern volatile __bit TRISB0 __at(0x430);


extern volatile __bit TRISB1 __at(0x431);


extern volatile __bit TRISB2 __at(0x432);


extern volatile __bit TRISB3 __at(0x433);


extern volatile __bit TRISB4 __at(0x434);


extern volatile __bit TRISB5 __at(0x435);


extern volatile __bit TRISB6 __at(0x436);


extern volatile __bit TRISB7 __at(0x437);


extern volatile __bit TRISC0 __at(0x438);


extern volatile __bit TRISC1 __at(0x439);


extern volatile __bit TRISC2 __at(0x43A);


extern volatile __bit TRISC3 __at(0x43B);


extern volatile __bit TRISC4 __at(0x43C);


extern volatile __bit TRISC5 __at(0x43D);


extern volatile __bit TRISC6 __at(0x43E);


extern volatile __bit TRISC7 __at(0x43F);


extern volatile __bit TRISD0 __at(0x440);


extern volatile __bit TRISD1 __at(0x441);


extern volatile __bit TRISD2 __at(0x442);


extern volatile __bit TRISD3 __at(0x443);


extern volatile __bit TRISD4 __at(0x444);


extern volatile __bit TRISD5 __at(0x445);


extern volatile __bit TRISD6 __at(0x446);


extern volatile __bit TRISD7 __at(0x447);


extern volatile __bit TRISE0 __at(0x448);


extern volatile __bit TRISE1 __at(0x449);


extern volatile __bit TRISE2 __at(0x44A);


extern volatile __bit TRISE3 __at(0x44B);


extern volatile __bit TRMT __at(0x4C1);


extern volatile __bit TUN0 __at(0x480);


extern volatile __bit TUN1 __at(0x481);


extern volatile __bit TUN2 __at(0x482);


extern volatile __bit TUN3 __at(0x483);


extern volatile __bit TUN4 __at(0x484);


extern volatile __bit TUN5 __at(0x485);


extern volatile __bit TX8_9 __at(0x4C6);


extern volatile __bit TX9 __at(0x4C6);


extern volatile __bit TX9D __at(0x4C0);


extern volatile __bit TXD8 __at(0x4C0);


extern volatile __bit TXEN __at(0x4C5);


extern volatile __bit TXIE __at(0x464);


extern volatile __bit TXIF __at(0x64);


extern volatile __bit UA __at(0x4A1);


extern volatile __bit VCFG0 __at(0x4FC);


extern volatile __bit VCFG1 __at(0x4FD);


extern volatile __bit WCOL __at(0xA7);


extern volatile __bit WDTPS0 __at(0x829);


extern volatile __bit WDTPS1 __at(0x82A);


extern volatile __bit WDTPS2 __at(0x82B);


extern volatile __bit WDTPS3 __at(0x82C);


extern volatile __bit ZERO __at(0x1A);


extern volatile __bit nA __at(0x4A5);


extern volatile __bit nADDRESS __at(0x4A5);


extern volatile __bit nBO __at(0x470);


extern volatile __bit nBOR __at(0x470);


extern volatile __bit nDONE __at(0xFA);


extern volatile __bit nPD __at(0x1B);


extern volatile __bit nPOR __at(0x471);


extern volatile __bit nRBPU __at(0x40F);


extern volatile __bit nRC8 __at(0xC6);


extern volatile __bit nT1SYNC __at(0x82);


extern volatile __bit nTO __at(0x1C);


extern volatile __bit nTX8 __at(0x4C6);


extern volatile __bit nW __at(0x4A2);


extern volatile __bit nWRITE __at(0x4A2);


# 30 "/opt/microchip/xc8/v2.05/pic/include/pic.h"
#pragma intrinsic(__nop)
extern void __nop(void);

# 78
__attribute__((__unsupported__("The " "FLASH_READ" " macro function is no longer supported. Please use the MPLAB X MCC."))) unsigned char __flash_read(unsigned short addr);

__attribute__((__unsupported__("The " "FLASH_WRITE" " macro function is no longer supported. Please use the MPLAB X MCC."))) void __flash_write(unsigned short addr, unsigned short data);

__attribute__((__unsupported__("The " "FLASH_ERASE" " macro function is no longer supported. Please use the MPLAB X MCC."))) void __flash_erase(unsigned short addr);


# 91
#pragma intrinsic(_delay)
extern __nonreentrant void _delay(unsigned long);
#pragma intrinsic(_delaywdt)
extern __nonreentrant void _delaywdt(unsigned long);

# 137
extern __bank0 unsigned char __resetbits;
extern __bank0 __bit __powerdown;
extern __bank0 __bit __timeout;

# 30 "main.c"
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
unsigned char tape_size_high;
unsigned char tape_size_low;
unsigned char hex_char_high;
unsigned char hex_char_low;
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


void initialize();
void mem_clear();
void mem_dump();
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


void usb_file_read();


unsigned char fifo_bank_A[64];
unsigned char fifo_bank_B[64];
unsigned char fifo_bank_C[32];
unsigned char fifo_bank_D[32];
unsigned char fifo_bank_E[32];
unsigned char fifo_bank_F[32];

const char str_timeout[] = "Timeout: ";
const char str_cmd_mode[] = "Command mode\n";

const unsigned char hex_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


unsigned char fifo_push(unsigned char data)
{
switch(fifo_write_bank)
{
case 0:
case 1:
fifo_bank_A[fifo_write_idx & 0x3F] = data;
break;
case 2:
case 3:
fifo_bank_B[fifo_write_idx & 0x3F] = data;
break;
case 4:
fifo_bank_C[fifo_write_idx & 0x1F] = data;
break;
case 5:
fifo_bank_D[fifo_write_idx & 0x1F] = data;
break;
case 6:
fifo_bank_E[fifo_write_idx & 0x1F] = data;
break;
case 7:
fifo_bank_F[fifo_write_idx & 0x1F] = data;
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
temp = fifo_bank_A[fifo_read_idx & 0x3F];
break;
case 2:
case 3:
temp = fifo_bank_B[fifo_read_idx & 0x3F];
break;
case 4:
temp = fifo_bank_C[fifo_read_idx & 0x1F];
break;
case 5:
temp = fifo_bank_D[fifo_read_idx & 0x1F];
break;
case 6:
temp = fifo_bank_E[fifo_read_idx & 0x1F];
break;
case 7:
temp = fifo_bank_F[fifo_read_idx & 0x1F];
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
usb_reset_all();
delay_millis(100);
usb_autoconfig();


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
usb_file_read();
break;
case 0x10:
usb_file_create();
break;

# 428
case 0x20:
mem_clear();
break;
case 0x21:
mem_dump();
break;
case 0x22:
print_test_data();
break;
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
