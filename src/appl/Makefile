#
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved$
# $ID:$
#
# Makefile for Applications
#

ifeq ($(CONFIG_LOADER),1)
    subdirs += cli
    ifeq ($(CONFIG_ZMODEM),1)
        subdirs += zmodem
    endif
endif

ifeq ($(CONFIG_DIAG),1)
    subdirs += diag_int
    subdirs += diag
endif

ifeq ($(CONFIG_CLI),1)
    subdirs += cli
    subdirs += diag_int
    subdirs += diag
endif

ifeq ($(CONFIG_TEST),1)
    subdirs += test
endif

ifeq ($(CONFIG_WEB_SERVER),1)
    subdirs += cli
    subdirs += web
endif

ifeq ($(BUILD_FLAVOR),ROBO_OS)
    subdirs += proto/common
    ifeq ($(CONFIG_LLDP),1)
        subdirs += proto/lldp
    endif

    ifeq ($(CONFIG_IGMP_SNOOPING),1)
        subdirs += proto/snooping
    endif

    ifeq ($(CONFIG_RSTP),1)
        subdirs += proto/rstp
    endif
endif
ifeq ($(CONFIG_DOT1AS),1)
    subdirs += proto/dot1as
endif

-include $(TOPDIR)/src/appl/api/Makefile

include $(TOPDIR)/make/Make.subdirs
