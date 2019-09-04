INCLUDE_PATH = -I../include/ -I../../include

CC_SYMBOLS   =-DF_CPU=16000000
DEVICE_NAME  = stm8s103f3
LIBSPEC      =-lstm8 -mstm8
OBJS         = $(SRC_NAME).rel $(ADD_LIBS) $(SRCS)
CC_FLAGS     =--std-c99 --opt-code-size --disable-warning 197 --disable-warning 84 --disable-warning 185

CC = sdcc
LD = sdld


.PHONY: all compile clean flash complete read_eeprom

all: clean $(OBJS)
	@echo "Linking $(SRC_NAME).c with libs, Intel-Hex-File: $(SRC_NAME).ihx"
	$(CC) $(LIBSPEC) $(INCLUDE_PATH) --out-fmt-ihx $(OBJS)
	@echo "  " 1>&2
	@echo " ------ Programm build sucessfull -----" 1>&2
	@echo "  " 1>&2

compile:
	$(CC) $(LIBSPEC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATH) $(SRC_NAME).c -c $(SRC_NAME).rel

clean:
	@rm -f *.asm
	@rm -f *.rst
	@rm -f *.ihx
	@rm -f *.rel
	@rm -f *.sym
	@rm -f *.lst
	@rm -f *.map
	@rm -f *.cdb
	@rm -f *.lk
	@rm -f *.mem

	@echo "Cleaning done..."

%.rel: %.c
	$(CC) $(LIBSPEC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATH) -c $< -o $@

flash:
ifeq ($(FLASHERPROG), 1)
	stm8_bootflash /dev/ttyACM0 $(SRC_NAME).ihx notxbar 1>&2
else
	stm8flash -c stlinkv2 -p $(DEVICE_NAME) -w $(SRC_NAME).ihx
endif

read_eeprom:
	stm8flash -c stlinkv2 -p stm8s103f3 -s eeprom -r eeprom.bin && xxd eeprom.bin | head
complete: clean all flash
