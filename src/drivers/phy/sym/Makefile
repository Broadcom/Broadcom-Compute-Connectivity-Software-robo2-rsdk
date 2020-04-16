# $Id$
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved.$
#

#What is being Built
TARGET = libphysym

CWD = $(shell pwd)
BLDOUT = $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

# Determine sources
PHY_ALLSYMS ?= 1
ifeq ($(PHY_ALLSYMS),1)
    # Allow compiler to merge duplicate strings
    SRCS = phy_allsyms.c
else
    SRCS = $(wildcard $(PHY)/sym/chip/*.c)
endif

OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(LIBDIR)/$(TARGET).a

include $(TOPDIR)/make/Make.depend

-include $(DEPS)
