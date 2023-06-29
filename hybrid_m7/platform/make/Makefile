export CPU = m7
export TOOLCHAIN_PATH = /projects/ccxsw/toolchains/gcc-arm-none-eabi-4_9-2015q3
# Local builds
# export TOOLCHAIN_PATH = /opt/gcc-arm-none-eabi-4_9-2015q3
export CROSSCOMPILE = $(TOOLCHAIN_PATH)/bin/arm-none-eabi-

export CC=$(CROSSCOMPILE)gcc
export LD=$(CROSSCOMPILE)ld
export AS=$(CROSSCOMPILE)as
export AR=$(CROSSCOMPILE)ar
export OBJCOPY=$(CROSSCOMPILE)objcopy
export OBJDUMP=$(CROSSCOMPILE)objdump
export STRIP=$(CROSSCOMPILE)strip
export SIZE=$(CROSSCOMPILE)size

export CFLAGS= -Werror -ffunction-sections

BUILDDIR = $(TOPDIR)/build

ARCH=armv7e-m
PROCESSOR=cortex-m7
ISA=thumb
export CFLAGS += -m$(ISA) -mcpu=$(PROCESSOR) -DCORTEX_M7 -DHYBRID_M7_MODE
export CFLAGS += -DCONFIG_SYSTEM_INIT -DBCM_53158_A0

export LIBGCC_DIR = $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`)
export LIBC_DIR = $(shell dirname `$(CC) $(CFLAGS) --print-file-name=libc.a`)
#export LDLIBS += --start-group $(LIBC_DIR)/libm.a $(LIBGCC_DIR)/libgcc.a --end-group
export LDLIBS += --start-group $(LIBC_DIR)/libc.a $(LIBC_DIR)/libm.a $(LIBGCC_DIR)/libgcc.a --end-group
export LDFLAGS += -static -nostdlib -nostartfiles
export LDSCR = bcm53158_dtcm.ld
#export LDSCR = bcm53158_rom.ld

export TOPDIR ?= $(shell cd ../../; pwd)
export SAL = $(shell cd ../../../; pwd)/os/osal/openrtos
export CWD = $(shell pwd)
export CP ?= cp

export OUTPUTDIR ?= $(TOPDIR)/build/
export LIBDIR ?= $(shell pwd)/libs
export BLDOUT ?= $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

export CFLAGS += -DM7_COMPILE
export CFLAGS += -DBCM53158_B0
export CFLAGS += -DUM_FLASH_BUILD

export CFLAGS += -I $(TOPDIR)/include/ -I $(TOPDIR)/platform/include/ -I $(TOPDIR)/platform/include/sal/ -I $(TOPDIR)/platform/soc/cmsis/include/

SRCS = $(TOPDIR)/platform/soc/cmsis/device/bcm53158/startup_bcm53158.c
SRCS += $(TOPDIR)/platform/soc/cmsis/device/bcm53158/system_bcm53158.c
SRCS += $(TOPDIR)/platform/soc/bcm53158/robo2/init.c
SRCS += $(TOPDIR)/platform/soc/bcm53158/robo2/system.c
SRCS += $(TOPDIR)/platform/soc/common/cpu_m7.c


SRCS += $(TOPDIR)/src/soc/bcm53158/main_hybrid.c
SRCS += $(TOPDIR)/src/sal/sal_console.c
SRCS += $(TOPDIR)/src/sal/sal_sscanf.c
SRCS += $(TOPDIR)/src/sal/sal_alloc.c
SRCS += $(TOPDIR)/src/sal/sal_util.c
#SRCS += $(TOPDIR)/src/sal/sal_time.c

LD_PATH = $(LIBDIR)

OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

TARGET = hybrid

all: hybrid

OBJECTS = $(subst $(TOPDIR),$(BUILDDIR),$(SRCS:.c=.o))

$(BUILDDIR)/%.o: $(TOPDIR)/%.c
	$(Q)mkdir -p $(dir $@)
	$(Q)echo [Compile ... $(notdir $<)]
	$(Q)$(CC) -c $(CFLAGS) -MMD -o $@ $<

pre-build:
	$(Q)echo PREBUILD
	$(CP) -f $(SAL)/sal_sscanf.c $(TOPDIR)/src/sal/.
	$(CP) -f $(SAL)/sal_alloc.c $(TOPDIR)/src/sal/.
	$(CP) -f $(SAL)/sal_util.c $(TOPDIR)/src/sal/.

hybrid: $(OBJECTS)
	$(Q)echo [Link ... hybrid_m7.img]
	$(Q)$(LD) $(LDFLAGS) -o $(BUILDDIR)/hybrid_m7.img -T $(LDSCR) -Map $(BUILDDIR)/hybrid_m7.map $^ $(LDLIBS)
	$(Q)$(OBJCOPY) -O binary $(BUILDDIR)/hybrid_m7.img $(BUILDDIR)/hybrid_m7.bin
	$(Q)$(OBJCOPY) -O ihex $(BUILDDIR)/hybrid_m7.img $(BUILDDIR)/hybrid_m7.ihex
	$(Q)$(OBJDUMP) -Sd $(BUILDDIR)/hybrid_m7.img > $(BUILDDIR)/hybrid_m7.asm

clean:
	$(Q)echo [Cleaning ...]
	$(Q)-rm -rf $(BUILDDIR)
#	$(Q)-rm -rf $(TOPDIR)/src/sal/sal_sscanf.c
#	$(Q)-rm -rf $(TOPDIR)/src/sal/sal_alloc.c
#	$(Q)-rm -rf $(TOPDIR)/src/sal/sal_util.c
