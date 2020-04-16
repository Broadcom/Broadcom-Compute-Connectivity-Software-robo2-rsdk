#  -*- Makefile -*-
# $Id: Makefile,v 1.1 2009/05/12 22:49:16 jtowns Exp $
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved.$
#
LOCALDIR = src/appl/wrapper_cint

#include $(TOPDIR)/make/Make.config

export TOPDIR ?= $(shell cd ../../..; pwd)

CFGFLAGS += -I$(TOPDIR)/src/appl/cint

TARGET  = libwrapper

CWD = $(shell pwd)
BLDOUT = $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

CFLAGS += -g -O0

CFLAGS += -Wall
CFLAGS += -Werror -fgnu89-inline
#CFLAGS += -DYYDEBUG
CFLAGS += -DCINT_CONFIG_INCLUDE_STDLIB=0

CFLAGS += -D_CTYPE_H
CFLAGS += -DDEBUG_CINT_WRAPPER
CFLAGS += -DINCLUDE_LIB_CINT
ifeq ($(OS),openrtos)
CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER_READLINE=0
CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY=0
endif
CFLAGS += -DCINT_CONFIG_INCLUDE_XINCLUDE=0
CFLAGS += -DCINT_CONFIG_FILE_IO=0

CFLAGS += -I$(TOPDIR)/include
CFLAGS += -I$(TOPDIR)/include/shared
CFLAGS += -I$(TOPDIR)/include/soc
CFLAGS += -I$(TOPDIR)/include/soc/robo2
CFLAGS += -I$(TOPDIR)/include/soc/robo2/common
CFLAGS += -I$(TOPDIR)/include/osal
CFLAGS += -I$(TOPDIR)/include/bsl_log
CFLAGS += -I$(TOPDIR)/usr/include
CFLAGS += -I$(TOPDIR)/src/appl/cint
CFLAGS += -I$(TOPDIR)/include/fsal

SRCS := fsal_stat_cint_data.c
SRCS += fsal_lag_cint_data.c
SRCS += fsal_stg_cint_data.c
SRCS += fsal_port_cint_data.c
SRCS += fsal_vlan_cint_data.c
SRCS += fsal_mirror_cint_data.c
SRCS += fsal_l2_cint_data.c
SRCS += fsal_meter_cint_data.c
SRCS += fsal_mcast_cint_data.c
SRCS += fsal_cosq_cint_data.c
SRCS += fsal_link_cint_data.c
SRCS += fsal_pktio_cint_data.c
SRCS += fsal_cfp_cint_data.c
SRCS += fsal_auth_cint_data.c
SRCS += fsal_cable_diag_cint_data.c
SRCS += soc_cint_data.c
SRCS += reg_debug_cint_data.c
SRCS += osal_cint_data.c
ifeq ($(CONFIG_FS),1)
    SRCS += osal_fs_cint_data.c
endif
ifeq ($(CONFIG_WEB_SERVER),1)
    SRCS += web_server_cint_data.c
endif
ifeq ($(CONFIG_PE),1)
    SRCS += fsal_extender_cint_data.c
endif
ifeq ($(CONFIG_DOT1AS),1)
    CFLAGS += -DCONFIG_DOT1AS=1
    SRCS += dot1as_cint_data.c
endif
ifeq ($(strip $(CONFIG_FSAL_LIF)),y)
    SRCS += fsal_lif_cint_data.c
endif
ifeq ($(strip $(CONFIG_FSAL_TIMESYNC)),y)
    CFLAGS += -DCONFIG_TIMESYNC=1
    SRCS += fsal_ts_cint_data.c
endif
ifeq ($(strip $(CONFIG_FSAL_PTP)),y)
    CFLAGS += -DCONFIG_PTP=1
    SRCS += fsal_ptp_cint_data.c
endif
SRCS += cint_cmd_main.c

OBJS := $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS := $(OBJS:%.o=%.d)

all: $(LIBDIR)/$(TARGET).a

include $(TOPDIR)/make/Make.depend
