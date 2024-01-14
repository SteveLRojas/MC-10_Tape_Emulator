################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ch32v20x_adc.c \
../ch32v20x_bkp.c \
../ch32v20x_can.c \
../ch32v20x_crc.c \
../ch32v20x_dbgmcu.c \
../ch32v20x_dma.c \
../ch32v20x_exti.c \
../ch32v20x_flash.c \
../ch32v20x_gpio.c \
../ch32v20x_i2c.c \
../ch32v20x_it.c \
../ch32v20x_iwdg.c \
../ch32v20x_misc.c \
../ch32v20x_pwr.c \
../ch32v20x_rcc.c \
../ch32v20x_rtc.c \
../ch32v20x_spi.c \
../ch32v20x_tim.c \
../ch32v20x_usart.c \
../ch32v20x_wwdg.c \
../core_riscv.c \
../debug.c \
../display.c \
../main.c \
../support.c \
../system_ch32v20x.c \
../usb.c 

S_UPPER_SRCS += \
../startup_ch32v20x_D6.S 

OBJS += \
./ch32v20x_adc.o \
./ch32v20x_bkp.o \
./ch32v20x_can.o \
./ch32v20x_crc.o \
./ch32v20x_dbgmcu.o \
./ch32v20x_dma.o \
./ch32v20x_exti.o \
./ch32v20x_flash.o \
./ch32v20x_gpio.o \
./ch32v20x_i2c.o \
./ch32v20x_it.o \
./ch32v20x_iwdg.o \
./ch32v20x_misc.o \
./ch32v20x_pwr.o \
./ch32v20x_rcc.o \
./ch32v20x_rtc.o \
./ch32v20x_spi.o \
./ch32v20x_tim.o \
./ch32v20x_usart.o \
./ch32v20x_wwdg.o \
./core_riscv.o \
./debug.o \
./display.o \
./main.o \
./startup_ch32v20x_D6.o \
./support.o \
./system_ch32v20x.o \
./usb.o 

S_UPPER_DEPS += \
./startup_ch32v20x_D6.d 

C_DEPS += \
./ch32v20x_adc.d \
./ch32v20x_bkp.d \
./ch32v20x_can.d \
./ch32v20x_crc.d \
./ch32v20x_dbgmcu.d \
./ch32v20x_dma.d \
./ch32v20x_exti.d \
./ch32v20x_flash.d \
./ch32v20x_gpio.d \
./ch32v20x_i2c.d \
./ch32v20x_it.d \
./ch32v20x_iwdg.d \
./ch32v20x_misc.d \
./ch32v20x_pwr.d \
./ch32v20x_rcc.d \
./ch32v20x_rtc.d \
./ch32v20x_spi.d \
./ch32v20x_tim.d \
./ch32v20x_usart.d \
./ch32v20x_wwdg.d \
./core_riscv.d \
./debug.d \
./display.d \
./main.d \
./support.d \
./system_ch32v20x.d \
./usb.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g3 -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\UART" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\USB-Driver\inc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\CONFIG" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross Assembler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


