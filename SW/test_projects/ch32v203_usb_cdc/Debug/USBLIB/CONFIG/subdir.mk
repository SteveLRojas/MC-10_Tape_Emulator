################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USBLIB/CONFIG/hw_config.c \
../USBLIB/CONFIG/usb_desc.c \
../USBLIB/CONFIG/usb_endp.c \
../USBLIB/CONFIG/usb_istr.c \
../USBLIB/CONFIG/usb_prop.c \
../USBLIB/CONFIG/usb_pwr.c 

OBJS += \
./USBLIB/CONFIG/hw_config.o \
./USBLIB/CONFIG/usb_desc.o \
./USBLIB/CONFIG/usb_endp.o \
./USBLIB/CONFIG/usb_istr.o \
./USBLIB/CONFIG/usb_prop.o \
./USBLIB/CONFIG/usb_pwr.o 

C_DEPS += \
./USBLIB/CONFIG/hw_config.d \
./USBLIB/CONFIG/usb_desc.d \
./USBLIB/CONFIG/usb_endp.d \
./USBLIB/CONFIG/usb_istr.d \
./USBLIB/CONFIG/usb_prop.d \
./USBLIB/CONFIG/usb_pwr.d 


# Each subdirectory must supply rules for building sources it contributes
USBLIB/CONFIG/%.o: ../USBLIB/CONFIG/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g3 -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\UART" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\USB-Driver\inc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\CONFIG" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


