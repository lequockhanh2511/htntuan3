CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m3 -mthumb

CFLAGS = $(CPU) \
-Os \
-g \
-ffreestanding \
-ffunction-sections \
-fdata-sections \
-IInc \
-IDrivers/CMSIS/Include \
-IDrivers/CMSIS/Device/ST/STM32F1xx/Include \
-IDrivers/STM32F1xx_HAL_Driver/Inc \
-DSTM32F103xB \
-Wall \
-Wextra

LDFLAGS = $(CPU) \
-TSTM32F103XB_FLASH.ld \
-Wl,--gc-sections \
-specs=nosys.specs

HAL_SRC = \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c

SRC = \
Src/main.c \
Src/stm32f1xx_hal_msp.c \
Src/stm32f1xx_it.c \
Src/system_stm32f1xx.c \
$(HAL_SRC)

OBJ = $(SRC:.c=.o)

all: main.bin

Src/%.o: Src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

Drivers/STM32F1xx_HAL_Driver/Src/%.o: Drivers/STM32F1xx_HAL_Driver/Src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

startup.o: Src/startup_stm32f103xb.s
	$(AS) $(CPU) -c $< -o $@

main.elf: startup.o $(OBJ)
	$(LD) $(LDFLAGS) startup.o $(OBJ) -o $@

main.bin: main.elf
	$(OBJCOPY) -O binary $< $@

flash: main.bin
	st-flash write main.bin 0x08000000

clean:
	rm -f $(OBJ) startup.o main.elf main.bin
