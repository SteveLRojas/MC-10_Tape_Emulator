#include "ch32v20x.h"
#include "fifo.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
//#include "ch32v203_spi.h"
//#include "ch32v203_timer.h"
#include "ch32v203_uart.h"
//#include "ch32v203_usbd.h"
#include "ch32v203_usbd_cdc.h"
//#include "ch32v203_usbfsd.h"
//#include "ch32v203_usbfsh.h"
//#include "pseudo_random.h"
#include "debug.h"
#include "support.h"
#include "lcd.h"

//Pins:
// DISP = PA0 - PA7
// TAPE_IN = PA8
// CH_RXD = PA9
// CH_TXD = PA10
// USB1DM = PA11
// USB1DP = PA12
// CH376_INT = PA13
// CH376_RST = PA14
// CH376_CS = PA15
// DISP_RW = PB0
// LED = PB1
// BOOT1 = PB2
// CH376_SCK = PB3
// CH376_MISO = PB4
// CH376_MOSI = PB5
// USB2DM = PB6
// USB2DP = PB7
// SW2 = PB8
// SW3 = PB9
// DISP_E = PB10
// DISP_RS = PB11
// RAM_CS = PB12
// RAM_SCK = PB13
// RAM_MISO = PB14
// RAM_MOSI = PB15
// SW0 = PC13
// SW1 = PC14
// TAPE_OUT = PC15

void print_bytes_as_hex(const uint8_t* source, uint8_t num_bytes)
{
	uint8_t idx;

	for(idx = 0; idx < num_bytes; ++idx)
	{
		printf("%02X: %02X\n", idx, source[idx]);
	}
}

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	rcc_ahb_clk_enable(RCC_USBFS);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_13);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);

	gpio_set_mode(GPIOB, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
	gpio_set_mode(GPIOB, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_8 | GPIO_PIN_9);

	gpio_set_mode(GPIOC, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_13 | GPIO_PIN_14);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_15);

	gpio_set_pin(GPIOA, GPIO_PIN_14);
	gpio_set_pin(GPIOB, GPIO_PIN_12);

	Delay_Init();
	uart_init(USART1, 115200);
	core_enable_irq(USART1_IRQn);

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	// blink the led once
	gpio_clear_pin(GPIOB, GPIO_PIN_1);
	Delay_Ms(100);
	gpio_set_pin(GPIOB, GPIO_PIN_1);
	printf("Unicorn\n");

	cdc_init();
	cdc_set_serial_state(0x03);

	lcd_init();
	lcd_print_string("Tape Emulator");

	uint8_t button_state_prev[4];
	button_state_prev[0] = gpio_read_pin(GPIOC, GPIO_PIN_13);
	button_state_prev[1] = gpio_read_pin(GPIOC, GPIO_PIN_14);
	button_state_prev[2] = gpio_read_pin(GPIOB, GPIO_PIN_8);
	button_state_prev[3] = gpio_read_pin(GPIOB, GPIO_PIN_9);
	while(1)
	{
		if(!gpio_read_pin(GPIOC, GPIO_PIN_13) && button_state_prev[0])
			printf("S1 Pressed\n");
		if(!gpio_read_pin(GPIOC, GPIO_PIN_14) && button_state_prev[1])
			printf("S2 Pressed\n");
		if(!gpio_read_pin(GPIOB, GPIO_PIN_8) && button_state_prev[2])
			printf("S3 Pressed\n");
		if(!gpio_read_pin(GPIOB, GPIO_PIN_9) && button_state_prev[3])
			printf("S4 Pressed\n");

		button_state_prev[0] = gpio_read_pin(GPIOC, GPIO_PIN_13);
		button_state_prev[1] = gpio_read_pin(GPIOC, GPIO_PIN_14);
		button_state_prev[2] = gpio_read_pin(GPIOB, GPIO_PIN_8);
		button_state_prev[3] = gpio_read_pin(GPIOB, GPIO_PIN_9);

		Delay_Ms(1);
	}
}

