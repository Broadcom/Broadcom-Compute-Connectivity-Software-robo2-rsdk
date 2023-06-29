#
# $ID:$
# $Copyright: (c) 2017 Broadcom Limited.
#
#

#What is being Built
TARGET = libfsal

#Source paths
SRCDIR = .

#Conditional inclusion of fsal drivers
#Robo2 fsal authentication (802.1x) module
ifeq ($(strip $(CONFIG_FSAL_AUTH)),y)
    FSAL_SRC += auth.c
endif
#Cable diag.
ifeq ($(strip $(CONFIG_FSAL_CDIAG)),y)
    FSAL_SRC += cable_diag.c
endif
#Robo2 CFP(ACL) module.
ifeq ($(strip $(CONFIG_FSAL_CFP_ACL)),y)
    FSAL_SRC += cfp.c
endif
#EPP Encap Handling
ifeq ($(strip $(CONFIG_FSAL_ENCAP)),y)
    FSAL_SRC += encap.c
endif
#Robo2 port extender module
ifeq ($(strip $(CONFIG_FSAL_EXTENDER)),y)
    FSAL_SRC += extender.c
endif
#Robo2 PTP module
ifeq ($(strip $(CONFIG_FSAL_PTP)),y)
    FSAL_SRC += ptp.c
    export CFLAGS += -DCONFIG_PTP
endif
#BC Mode 1
ifeq ($(strip $(CONFIG_FSAL_BCMODE1)),y)
    export CFLAGS += -DCONFIG_BC_MODE1
endif
#Robo2 LAG module.
ifeq ($(strip $(CONFIG_FSAL_LAG)),y)
    FSAL_SRC += lag.c
endif
#Defines internal functions to handle Logincal Interfaces.
ifeq ($(strip $(CONFIG_FSAL_LOGINCAL)),y)
    FSAL_SRC += lin.c
endif
ifeq ($(strip $(CONFIG_FSAL_LIF)),y)
    FSAL_SRC += lif.c
endif
#Robo2 Link Scan module.
ifeq ($(strip $(CONFIG_FSAL_LINKSCAN)),y)
    FSAL_SRC += link.c
endif
#Robo2 L2 multicast module
ifeq ($(strip $(CONFIG_FSAL_MULTICAST)),y)
    FSAL_SRC += mcast.c
endif
#Robo2 meter module.
ifeq ($(strip $(CONFIG_FSAL_METER)),y)
    FSAL_SRC += meter.c ../shared/idxres_afl.c
endif
#Robo2 mirror module.
ifeq ($(strip $(CONFIG_FSAL_MIRROR)),y)
    FSAL_SRC += mirror.c
endif
#Robo2 port module. This tracks and manages physical ports.
ifeq ($(strip $(CONFIG_FSAL_PORT)),y)
    FSAL_SRC += port.c
endif
#Robo2 SLIC TCAM programming routines
ifeq ($(strip $(CONFIG_FSAL_SLIC_TCAM)),y)
    FSAL_SRC += slic.c
endif
#Robo2 statistics module
ifeq ($(strip $(CONFIG_FSAL_STATS)),y)
    FSAL_SRC += stat.c
endif
#Robo2 stg module - Implementation of RSTP/MSTP.
ifeq ($(strip $(CONFIG_FSAL_RSTP_MSTP)),y)
    FSAL_SRC += stg.c
endif
#Robo2 trap programming routines
ifeq ($(strip $(CONFIG_FSAL_TRAP_PROGAMMING)),y)
    FSAL_SRC += trap.c
endif
#Robo2 l2 module
ifeq ($(strip $(CONFIG_FSAL_L2)),y)
    FSAL_SRC += l2.c vlan.c
endif
#Receive processor
ifeq ($(strip $(CONFIG_FSAL_RX_PROC)),y)
    FSAL_SRC += rx.c pktio.c cosq.c
endif
#TimeSync
ifeq ($(strip $(CONFIG_FSAL_TIMESYNC)),y)
    FSAL_SRC += ts.c
    export CFLAGS += -DCONFIG_TIMESYNC
endif

ifneq ($(FSAL_SRC),)
FSAL_SRC += fsal.c
endif

CWD = $(shell pwd)
BLDOUT = $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

SRCS = $(addprefix $(SRCDIR)/,$(FSAL_SRC))
OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(LIBDIR)/$(TARGET).a


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

include $(TOPDIR)/make/Make.depend
