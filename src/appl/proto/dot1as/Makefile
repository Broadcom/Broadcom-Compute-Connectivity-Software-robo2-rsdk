#
# $ID:$
# $Copyright: (c) 2017 Broadcom Limited.
#
#

#What is being Built
TARGET = libdot1as

#Source paths
SRCDIR = .

CWD = $(shell pwd)
BLDOUT = $(subst $(TOPDIR),$(OUTPUTDIR),$(CWD))

SRCS = $(wildcard $(addsuffix /*.c,$(SRCDIR)))
OBJS = $(SRCS:%.c=$(BLDOUT)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(OUTPUTDIR)/$(TARGET).a



$(OUTPUTDIR)/$(TARGET).a: $(OBJS)
	@echo "Creating archive"
	@$(AR) rs $@ $^
	@cp $@ $(LIBDIR)


clean:
	@echo "Cleaning Object files and Libraries"
	@$(RM) -rf $(TOPDIR)/build/$(OBJS),
	@$(RM) -rf $(TOPDIR)/build/$(TARGET).a

-include $(DEPS)

include $(TOPDIR)/make/Make.depend
