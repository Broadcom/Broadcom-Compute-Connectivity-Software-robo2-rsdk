#
# $ID:$
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved$
# System level Makefile 
#
#

#Target Default is NS2 ARM A57
export CPU ?= a57

# Default Build env in case they aren't set
export OS ?= linux
export BUILD_FLAVOR ?= rsdk
export TARGET_IMG ?= rsdk_linux

#Preferred Shell
export SHELL=/bin/bash

#Toplevel folders
export TOPDIR ?= $(shell cd ../../..; pwd)
export CWD = $(shell pwd)

-include $(TOPDIR)/make/Make.config


#Preferred Cflags
export CFLAGS += -g
export CFLAGS += -DCFG_ASSERTION_ENABLED -DDEBUG_ERR_TRACE
export CFLAGS += -DBCM_53158_A0 -nostdlib
export CFLAGS += -DCOUNTER_THREAD
#Include options
export INCLUDE = -I. 

SRCS = main.c pktio_wrapper.c
ifeq ($(CONFIG_UART_CPU_INTF),1)
    SRCS += uart_wrapper.c
else
    SRCS += spi_wrapper.c
endif
OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

.PHONY: osal libs clean $(OUTPUTDIR)/$(TARGET_IMG).img


all: osal libs $(OUTPUTDIR)/$(TARGET_IMG).img

osal:
	$(Q)$(MAKE) -C $(TOPDIR)/os/osal/linux
	$(Q)$(MAKE) -C $(TOPDIR)/os/osal/common

$(BLDOUT)/%.o: %.c
	$(Q)mkdir -p $(BLDOUT)
	$(Q)echo [Compiling ... $(notdir $<)]
	$(Q)$(CC) -c -MMD $(CFLAGS) -o $@  $<

$(OUTPUTDIR)/$(TARGET_IMG).img: $(OBJS)
	@echo [Linking ... $(notdir $@)]
	$(Q)$(CC) -o $@ $(OBJS) \
        -L$(LIBDIR) -Wl,--start-group $(LIBS) -Wl,--end-group
	@echo "Finished building executable"
	@$(CP) $@ $(CWD)

clean: phylibs_clean
	@echo "Cleaning..."
	@$(MAKE) -C $(TOPDIR)/src clean
	@$(RM) -rf $(OUTPUTDIR)
	@$(RM) -rf $(TARGET_IMG).img

phylibs_clean:
	@make -C $(TOPDIR)/src/drivers/phy clean

include $(TOPDIR)/make/Make.libs
-include $(TOPDIR)/make/Make.targets

-include $(DEPS)
