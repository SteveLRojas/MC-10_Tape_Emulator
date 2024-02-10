################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UART/cdc_fifo.c \
../UART/cdc_serial.c 

OBJS += \
./UART/cdc_fifo.o \
./UART/cdc_serial.o 

C_DEPS += \
./UART/cdc_fifo.d \
./UART/cdc_serial.d 


# Each subdirectory must supply rules for building sources it contributes
UART/%.o: ../UART/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g3 -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\UART" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\USB-Driver\inc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\CONFIG" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


