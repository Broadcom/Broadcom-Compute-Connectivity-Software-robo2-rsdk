#
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved$
#
#

# makefile generated for $path
# Common top-level makefile to get things started.
#
VIEWPATH=$(shell pwd)
TEMP_DIR = $(VIEWPATH)/archive_temp
export Q ?= @

#----------------------------------------------------------------------
# Base build targets
#----------------------------------------------------------------------

all um:
	$(Q)$(MAKE) -C platform/make/

pre-build:
	$(Q)$(MAKE) -C platform/make/ pre-build
     

clean:
	$(Q)$(MAKE) -C platform/make/ clean

#----------------------------------------------------------------------
# Install targets
#----------------------------------------------------------------------
install:
	#@echo "Building target: $@"
	#@echo "BUILD_NAME=$(BUILD_NAME)"
	#@echo "VIEWPATH=$(VIEWPATH)"
	#@echo "INSTALL_DIR=$(INSTALL_DIR)"
	#@echo "ARCHIVE_NAME=$(ARCHIVE_NAME)"
	#@echo "TEMP_DIR=$(TEMP_DIR)"
	-cp platform/make/um_rom.* $(INSTALL_DIR)


