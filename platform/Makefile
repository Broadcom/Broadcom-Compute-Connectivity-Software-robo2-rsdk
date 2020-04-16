#
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved$
#
#

subdirs += soc
ifneq ($(OS),linux)
subdirs += board
endif
ifeq ($(OS),openrtos)
subdirs += soc/cmsis
endif
ifeq ($(BUILD_FLAVOR),BOOT_LOADER)
subdirs += soc/loader
endif

include $(TOPDIR)/make/Make.subdirs

