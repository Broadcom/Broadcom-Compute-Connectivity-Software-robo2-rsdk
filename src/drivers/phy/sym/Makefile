# $Id$
# $Copyright: (c) 2017 Broadcom Corp.
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
