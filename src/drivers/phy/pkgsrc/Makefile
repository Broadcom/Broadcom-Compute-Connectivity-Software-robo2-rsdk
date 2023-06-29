# $Id$
# $Copyright: (c) 2017 Broadcom Corp.
# All Rights Reserved.$
#

#What is being Built
TARGET = libphypkgsrc

#Source paths
SRCDIR = .

CWD = $(shell pwd)
BLDOUT = $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

SRCS = $(shell find $(SRCDIR) -name '*.c')
OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(LIBDIR)/$(TARGET).a

include $(TOPDIR)/make/Make.depend

-include $(DEPS)
