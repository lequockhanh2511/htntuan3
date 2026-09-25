CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -mfloat-abi=soft \
         -std=c11 \
         -O0 \
         -Wall \
         -ffreestanding \
         -fdata-sections \
         -ffunction-sections

LDFLAGS = -T linker.ld \
          -nostartfiles \
          -Wl,--gc-sections \
          -Wl,-Map=main.map

C_SOURCES = main.c i2c.c ds1307.c

OBJECTS = main.o i2c.o ds1307.o startup_stm32f103xb.o

all: main.bin

main.elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o main.elf

main.bin: main.elf
	$(OBJCOPY) -O binary main.elf main.bin

main.o: main.c i2c.h ds1307.h
	$(CC) $(CFLAGS) -c main.c -o main.o

i2c.o: i2c.c i2c.h
	$(CC) $(CFLAGS) -c i2c.c -o i2c.o

ds1307.o: ds1307.c ds1307.h i2c.h
	$(CC) $(CFLAGS) -c ds1307.c -o ds1307.o

startup_stm32f103xb.o: startup_stm32f103xb.s
	$(CC) $(CFLAGS) -c startup_stm32f103xb.s -o startup_stm32f103xb.o

flash: main.elf
	openocd -f interface/stlink.cfg \
		-f target/stm32f1x.cfg \
		-c "program main.elf verify reset exit"

clean:
	rm -f *.o *.elf *.bin *.map
