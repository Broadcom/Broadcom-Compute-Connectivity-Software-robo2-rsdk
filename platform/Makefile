#
# $Copyright: (c) 2017 Broadcom Corp.
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

