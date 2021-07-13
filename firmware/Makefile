# All build variables can be set on command line. Build with a 
# command of this form:
#
#    TMS_BASE=0xBE SN76489_PORT=0xFF BOARD_REV=4 MSX_KEY_BASE=0xA9 PORT=/dev/tty.usbserial-00000000 make install
#

# Hardware revision (Important: must be set to correct value)
#BOARD_REV?=6

# Uncomment to enable DS1306 RTC support (used on IOX board)
# DS1306_RTC=1

# Uncomment to enable DS1302 RTC support (used on CPU/RAM/RTC board)
# DS1302_RTC=1

# Current git hash
#GITVERSION:= $(shell git log -1 --pretty='%h')
#GITVERSION:= $(shell echo no git)

# MCU type and frequency
MCU?=atmega2560
F_CPU?=16000000L

# Programmer options
PROGRAMMER?=wiring
PORT?=/dev/cu.usbserial-1410
#PORT?=/dev/cu.usbmodem14101
BAUD?=115200

AVRCC?=avr-gcc
OBJCOPY?=avr-objcopy
AVRDUDE?=avrdude
AVRSIZE=avr-size

ifeq ($(OS),Windows_NT)
CLEAN?=cmd ///c del
else
CLEAN?=rm -rf
endif

BIN=6502ctrl
FF_OBJS=ff.o diskio.o mmc_avr_spi.o
OBJS=	cli.o \
	ff.o \
	diskio.o \
	ffwrap.o \
	spi.o \
	bus.o \
	xmodem.o \
	ihex.o \
	mmc_avr_spi.o \
	util.o \
	sioemu.o \
	dmarq.o \
	w6502.o \
	disasm.o \
	uart.o

ifdef BOARD_REV
	FEATURE_DEFINES += -DBOARD_REV=$(BOARD_REV)
endif
ifdef SST_FLASH
	FEATURE_DEFINES +=-DSST_FLASH
	OBJS += flash.o
endif
ifdef TMS_BASE
	FEATURE_DEFINES += -DTMS_BASE=$(TMS_BASE)
	OBJS += tms.o
endif
ifdef SN76489_PORT
	FEATURE_DEFINES += -DSN76489_PORT=$(SN76489_PORT)
endif
ifdef BANK_BASE
	FEATURE_DEFINES += -DBANK_BASE=$(BANK_BASE)
endif
ifdef BANK_PORT
	FEATURE_DEFINES += -DBANK_PORT=$(BANK_PORT)
endif
ifdef DS1306_RTC
	FEATURE_DEFINES += -DDS1306_RTC -DUSE_RTC
	OBJS += ds1306.o
endif
ifdef DS1302_RTC
	FEATURE_DEFINES += -DDS1302_RTC -DUSE_RTC
	OBJS += ds1302.o
endif
ifdef IOX_BASE
	FEATURE_DEFINES += -DIOX_BASE=$(IOX_BASE)
endif
ifdef RTC_BASE
	FEATURE_DEFINES += -DRTC_BASE=$(RTC_BASE)
endif
ifdef MSX_KEY_BASE
	FEATURE_DEFINES += -DMSX_KEY_BASE=$(MSX_KEY_BASE)
	OBJS += msxkey.o
endif
CFLAGS=-std=c99 -flto -Os $(FEATURE_DEFINES) -DMEGA -DF_CPU=$(F_CPU) -mmcu=$(MCU) -I.

%.o: %.c
	$(AVRCC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

$(BIN).hex: $(BIN).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(BIN).elf: $(OBJS)
	$(AVRCC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(AVRSIZE) $(BIN).elf

install: $(BIN).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -D -P $(PORT) -b $(BAUD) -U flash:w:$<

clean:
	$(CLEAN) $(BIN).elf $(BIN).hex $(OBJS)
