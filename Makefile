# Sources

SRCS = main.c stm32f4xx_it.c system_stm32f4xx.c misc.c newlib_stub.c
SRCS += TM/tm_stm32f4_usb_vcp.c TM/tm_stm32f4_disco.c TM/tm_stm32f4_ili9341.c TM/tm_stm32f4_spi.c 
SRCS += TM/tm_stm32f4_stmpe811.c TM/tm_stm32f4_usb_vcp.c TM/tm_stm32f4_disco.c TM/tm_stm32f4_ili9341.c  TM/tm_stm32f4_fonts.c 
SRCS += TM/tm_stm32f4_stmpe811.c TM/tm_stm32f4_ili9341_button.c TM/tm_stm32f4_i2c.c  TM/tm_stm32f4_delay.c 
SRCS += TM/tm_stm32f4_rtc.c 
SRCS +=  TM/usb_device/usb_bsp.c TM/usb_device/usb_core.c TM/usb_device/usb_dcd.c TM/usb_device/usbd_cdc_core.c 
SRCS +=  TM/usb_device/usbd_cdc_vcp.c TM/usb_device/usb_dcd_int.c TM/usb_device/usbd_core.c TM/usb_device/usbd_desc.c 
SRCS += TM/usb_device/usbd_ioreq.c TM/usb_device/usbd_req.c TM/usb_device/usbd_usr.c
PROJ_NAME=lockscreen
OUTPATH=build

###################################################

# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
#override FLOAT_TYPE = hard
override FLOAT_TYPE = soft
endif
endif

###################################################

BINPATH=~/sat/bin
CC=$(BINPATH)/arm-none-eabi-gcc	
OBJCOPY=$(BINPATH)/arm-none-eabi-objcopy
SIZE=$(BINPATH)/arm-none-eabi-size

CFLAGS  = -std=gnu99 -g -O2 -Wall -Tlib/stm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m4
CFLAGS += -DSTM32F429_439xx 
CFLAGS += -DSTM32F429_439xx
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -DSTM32F4XX
CFLAGS += -DUSE_STM32_DISCOVERY 
CFLAGS += -DSTM32F10X_MD_VL
CFLAGS += -D__ASSEMBLY__

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
CFLAGS += -msoft-float
endif

###################################################

vpath %.c User:CMSIS:STM32F4xx_StdPeriph_Driver/src/:TM:TM/usb_device
vpath %.a STM32F4xx_StdPeriph_Driver

ROOT=$(shell pwd)

CFLAGS += -ISTM32F4xx_StdPeriph_Driver 
CFLAGS += -ISTM32F4xx_StdPeriph_Driver/inc -ITM -ITM/usb_device
CFLAGS += -ICMSIS -IUser -IMDK-ARM

SRCS += lib/startup_stm32f4xx.s # add startup file to build

OBJS = $(SRCS:.c=.o)

###################################################

.PHONY: lib proj

all: lib proj
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

lib:
	$(MAKE) -C STM32F4xx_StdPeriph_Driver FLOAT_TYPE=$(FLOAT_TYPE)

proj: 	$(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ -LSTM32F4xx_StdPeriph_Driver -lstm32f4 -lm
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	rm -f *.o
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
	$(MAKE) clean -C lib # Remove this line if you don't want to clean the libs as well

flash:
	openocd -f /opt/openocd/share/openocd/scripts/board/stm32f429discovery.cfg  \
	    -c init -c targets -c "reset halt"\
	    -c "flash write_image erase $(OUTPATH)/$(PROJ_NAME).elf" \
	    -c "reset run" -c "shutdown"
