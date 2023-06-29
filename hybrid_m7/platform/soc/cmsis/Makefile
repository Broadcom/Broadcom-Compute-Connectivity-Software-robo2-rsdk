#
# $ID:$
# $Copyright: (c) 2017 Broadcom Corp.
# All Rights Reserved$
#
#

#Source paths
SRCDIR = device/bcm53158


CWD = $(shell pwd)
BLDOUT = $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

SRCS = $(wildcard $(addsuffix /*.c,$(SRCDIR)))
SRCS += $(wildcard $(addsuffix /*.s,$(SRCDIR)))
OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(OBJS)
	@cp $^ $(LIBDIR)


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

include $(TOPDIR)/make/Make.depend
