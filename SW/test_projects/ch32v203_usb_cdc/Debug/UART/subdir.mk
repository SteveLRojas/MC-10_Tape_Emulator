################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UART/UART.c \
../UART/cdc_fifo.c 

OBJS += \
./UART/UART.o \
./UART/cdc_fifo.o 

C_DEPS += \
./UART/UART.d \
./UART/cdc_fifo.d 


# Each subdirectory must supply rules for building sources it contributes
UART/%.o: ../UART/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g3 -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\UART" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\USB-Driver\inc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\CONFIG" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


