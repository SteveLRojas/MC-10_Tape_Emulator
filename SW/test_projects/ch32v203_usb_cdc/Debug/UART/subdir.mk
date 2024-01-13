################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UART/UART.c 

OBJS += \
./UART/UART.o 

C_DEPS += \
./UART/UART.d 


# Each subdirectory must supply rules for building sources it contributes
UART/%.o: ../UART/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g3 -I"C:\Users\esteb\Documents\Workspace\ch32v203_usb_cdc" -I"C:\Users\esteb\Documents\Workspace\ch32v203_usb_cdc\UART" -I"C:\Users\esteb\Documents\Workspace\ch32v203_usb_cdc\USBLIB\USB-Driver\inc" -I"C:\Users\esteb\Documents\Workspace\ch32v203_usb_cdc\USBLIB\CONFIG" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


