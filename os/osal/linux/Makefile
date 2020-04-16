#
# $ID:$
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved$
#
#

#What is being Built
TARGET = libosal

#Source paths
SRCDIR = .

CWD = $(shell pwd)
BLDOUT = $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

SRCS = $(wildcard $(addsuffix /*.c,$(SRCDIR)))
OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(LIBDIR)/$(TARGET).a


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

include $(TOPDIR)/make/Make.depend

