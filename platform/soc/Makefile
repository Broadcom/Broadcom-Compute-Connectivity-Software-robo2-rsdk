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
TARGET = libsoc

#Source paths
SRCDIR = common
SRCDIR += common/robo2
SRCDIR += bcm53158/robo2
ifneq ($(filter $(BUILD_FLAVOR), ROBO_OS BOOT_LOADER),)
SRCDIR += loader/common
endif

CWD = $(shell pwd)
BLDOUT = $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

ALLSRCS = $(wildcard $(addsuffix /*.c,$(SRCDIR)))

ifneq ($(CPU),m7)
    SRCS = $(filter-out common/cpu_m7.c, $(ALLSRCS))
else
    SRCS = $(ALLSRCS)
endif

OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(LIBDIR)/$(TARGET).a



ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

include $(TOPDIR)/make/Make.depend
