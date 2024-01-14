################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USBLIB/USB-Driver/src/usb_core.c \
../USBLIB/USB-Driver/src/usb_init.c \
../USBLIB/USB-Driver/src/usb_int.c \
../USBLIB/USB-Driver/src/usb_mem.c \
../USBLIB/USB-Driver/src/usb_regs.c \
../USBLIB/USB-Driver/src/usb_sil.c 

OBJS += \
./USBLIB/USB-Driver/src/usb_core.o \
./USBLIB/USB-Driver/src/usb_init.o \
./USBLIB/USB-Driver/src/usb_int.o \
./USBLIB/USB-Driver/src/usb_mem.o \
./USBLIB/USB-Driver/src/usb_regs.o \
./USBLIB/USB-Driver/src/usb_sil.o 

C_DEPS += \
./USBLIB/USB-Driver/src/usb_core.d \
./USBLIB/USB-Driver/src/usb_init.d \
./USBLIB/USB-Driver/src/usb_int.d \
./USBLIB/USB-Driver/src/usb_mem.d \
./USBLIB/USB-Driver/src/usb_regs.d \
./USBLIB/USB-Driver/src/usb_sil.d 


# Each subdirectory must supply rules for building sources it contributes
USBLIB/USB-Driver/src/%.o: ../USBLIB/USB-Driver/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g3 -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\UART" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\USB-Driver\inc" -I"C:\Users\Steve\repos\MC-10_Tape_Emulator\SW\test_projects\ch32v203_usb_cdc\USBLIB\CONFIG" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


