#
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved$
#
#

ifeq ($(CONFIG_LOADER),1)
    subdirs += appl
    subdirs += drivers/flash
    subdirs += drivers/devfs
else
    subdirs += drivers/mac
    subdirs += drivers/fsal
    subdirs += drivers/phy
    subdirs += drivers/phy/avenger
    subdirs += drivers/i2c
    subdirs += appl
    subdirs += common
ifeq ($(OS),openrtos)
    subdirs += drivers/flash
endif

ifeq ($(CONFIG_CINT),1)
    subdirs += appl/cint
    subdirs += appl/wrapper_cint
ifeq ($(OS),openrtos)
    subdirs += appl/compilelib
endif
endif

ifeq ($(CONFIG_IP_STACK),1)
    subdirs += net/lwip
endif

endif

include $(TOPDIR)/make/Make.subdirs

