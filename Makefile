ELF=Demo.elf
BIN=Demo.bin

TEMPLATE_ROOT= .

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
GDB=arm-none-eabi-gdb



CFLAGS=-O1 -g
LDFLAGS=-g


DEVICE=$(TEMPLATE_ROOT)/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CORE=$(TEMPLATE_ROOT)/CMSIS/CM3/CoreSupport
PERIPH=$(TEMPLATE_ROOT)/StdPeriph_Driver
STARTUP=$(TEMPLATE_ROOT)/startup
SRC=$(TEMPLATE_ROOT)/Src


vpath %.c $(TEMPLATE_ROOT)/Src
vpath %.c $(CORE)
vpath %.c $(PERIPH)/src
vpath %.c $(DEVICE)
vpath %.c $(STARTUP)
vpath %.c $(SRC)


LDSCRIPT=$(TEMPLATE_ROOT)/LinkerScript.ld
STARTUP=startup_stm32f10x.o system_stm32f10x.o


OBJS=$(STARTUP) main.o
OBJS+=stm32f10x_gpio.o stm32f10x_rcc.o stm32f10x_usart.o stm32f10x_spi.o
OBJS+=uart_functions.o spi_functions.o pn532_spi.o pn532.o pn532_emulatetag.o config.o


FULLASSERT=-DUSE_FULL_ASSERT
PTYPE=STM32F10X_MD
LDFLAGS+=-T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 --specs=nosys.specs
CFLAGS+=-mcpu=cortex-m3 -mthumb --specs=nosys.specs
CFLAGS+=-I$(TEMPLATE_ROOT) -I$(DEVICE) -I$(CORE) -I$(PERIPH)/inc -I$(TEMPLATE_ROOT)/Inc -I .
CFLAGS+=-D$(PTYPE) -DUSE_STDPERIPH_DRIVER $(FULLASSERT)

build: $(ELF) bin

$(ELF) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

bin : $(ELF)
	arm-none-eabi-objcopy -Obinary $(ELF) $(BIN)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(ELF) $(BIN)

flash:
	openocd -f openocd.cfg

debug:
	gnome-terminal --command="openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg"
	$(GDB) $(ELF) \
	-ex 'target remote localhost:3333' \
	-ex 'monitor reset halt'
	-ex 'monitor arm semihosting enable'

