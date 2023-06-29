#
# $Copyright: (c) 2017 Broadcom Corp.
# All Rights Reserved$
# $ID:$
#
# Top level make file for avenger repo
#

TOPDIR = $(shell pwd)
Q ?= @
export UIMAGE_DIR ?= $(TOPDIR)/../RSDK_uImage

#----------------------------------------------------------------------
# Supported build targets
#----------------------------------------------------------------------
TARGETS = robo_os_bcm953158k\
          robo_os_bcm953162k \
          rsdk_openrtos_bcm953158k \
          rsdk_openrtos_bcm953162k \
          rsdk_linux \
          emulation \
          bootloader

.PHONY: $(TARGETS) web_server_sim clean

# Default to emulation build target
# emulation targe is essentially robo_os + openrtos with EMULATION_BUILD define
emulation: OS := openrtos
emulation: BOARD_TYPE := bcm953158k
emulation: BUILD_FLAVOR := ROBO_OS
emulation: CFLAGS += -DEMULATION_BUILD

#----------------------------------------------------------------------
# Setup target-specific variables for each supported target
# To add a new target, just define the evnv variables as below
# and add the target to the TARGETS variable
#----------------------------------------------------------------------
# Robo OS always implies OPENRTOS
robo_os_bcm953158k: OS := openrtos
robo_os_bcm953158k: BOARD_TYPE := bcm953158k
robo_os_bcm953158k: BUILD_FLAVOR := ROBO_OS

# Robo OS always implies OPENRTOS
robo_os_bcm953162k: OS := openrtos
robo_os_bcm953162k: BOARD_TYPE := bcm953162k
robo_os_bcm953162k: BUILD_FLAVOR := ROBO_OS

# RSDK openrtos targets
rsdk_openrtos_bcm953158k: OS := openrtos
rsdk_openrtos_bcm953158k: BOARD_TYPE := bcm953158k
rsdk_openrtos_bcm953158k: BUILD_FLAVOR := RSDK

rsdk_openrtos_bcm953162k: OS := openrtos
rsdk_openrtos_bcm953162k: BOARD_TYPE := bcm953162k
rsdk_openrtos_bcm953162k: BUILD_FLAVOR := RSDK

# RSDK Linux target
rsdk_linux: OS := linux
rsdk_linux: OS_MODE := user
rsdk_linux: BOARD_TYPE :=
rsdk_linux: BUILD_FLAVOR := RSDK
# Uncomment the line below to build for ToD over serial
#rsdk_linux: CFLAGS += -DTS_TOD_OVER_SERIAL

# Boot loader target
bootloader: OS := openrtos
bootloader: BOARD_TYPE :=
bootloader: BUILD_FLAVOR = BOOT_LOADER

# Special build target for linux host dev env
# Webserver for host env
web_server_sim: OS := linux
web_server_sim: BOARD_TYPE :=
web_server_sim: BUILD_FLAVOR = WEB_SERVER_SIM

# Make recipe for all supported targets
define os_make_recipe
$(1):
	$(Q)echo [Target ... $(1)]
	$(Q)echo [CFLAGS=$(CFLAGS) TOPDIR=$(TOPDIR) OS=$(OS) OS_MODE=$(OS_MODE) MAKECMDGOALS=$(MAKECMDGOALS) \
	BUILD_FLAVOR=$(BUILD_FLAVOR) BOARD_TYPE=$(BOARD_TYPE)]
	$(Q)CFLAGS=$$(CFLAGS) make -C $(TOPDIR)/os/$$(OS)/$$(OS_MODE) $(filter clean,$(MAKECMDGOALS)) \
        OS=$$(OS) BUILD_FLAVOR=$$(BUILD_FLAVOR) BOARD_TYPE=$$(BOARD_TYPE) TARGET_IMG=$(1)
endef
$(foreach target,$(TARGETS),$(eval $(call os_make_recipe,$(target))))

# Recipe for web serever - linux host dev env
web_server_sim:
	$(Q)make -C $(TOPDIR)/src/appl/web TARGET=$(TOPDIR)/build/$@/web_server \
        $(subst clean,clean-web_server_sim,$(filter clean,$(MAKECMDGOALS)))

# make all shall build all targets
all: $(TARGETS)

# Empty recipe for clean as clean needs a target to be specified
# And the recipe for that particular target will handle the clean
clean: ;@:

# Map legacy targets - To keep CI builds happy
umplus: robo_os_bcm953158k
umplus_emulation: emulation
mm_linux_x86: rsdk_linux

install:
	$(Q)-cp $(TOPDIR)/build/emulation/emulation.img* $(INSTALL_DIR)
	$(Q)-cp $(TOPDIR)/build/emulation/bcm53158.img.qspi $(INSTALL_DIR)
	$(Q)-cp $(TOPDIR)/build/rsdk_linux/rsdk_linux.img $(INSTALL_DIR)
	$(Q)-cp $(TOPDIR)/upgradeable.bin $(INSTALL_DIR)

# Package building targets

# Create release tar for rsdk - includes uImages
package-rsdk_rel: package-rsdk_linux
	$(Q)make rsdk_linux > /dev/null
	$(Q)if [ "$(ROBO_REL_VERSION)" != "" ] ;\
	then \
            rel_tag=`echo "$(ROBO_REL_VERSION)" | tr . _` ;\
	    cp -f $(TOPDIR)/build/rsdk_linux/rsdk_linux.img robo2_rsdk_bcm958712_$$rel_tag.img ;\
	    cp -f $(UIMAGE_DIR)/bcm958525_uImage bcm958525_uImage_$$rel_tag ;\
	    cp -f $(UIMAGE_DIR)/bcm958712_uImage-usbserial bcm958712_uImage_$$rel_tag ;\
	    mv $(TOPDIR)/rsdk_linux_src.tgz robo2_rsdk_src_$$rel_tag.tar.gz ;\
            tar -czf $(ROBO_REL_TYPE)_$(ROBO_REL_VERSION)_Robo2_RSDK-src.tar.gz robo2_rsdk_src_$$rel_tag.tar.gz robo2_rsdk_bcm958712_$$rel_tag.img bcm958525_uImage_$$rel_tag  bcm958712_uImage_$$rel_tag ;\
            rm -f robo2_rsdk_src_$$rel_tag.tar.gz robo2_rsdk_bcm958712_$$rel_tag.img bcm958525_uImage_$$rel_tag  bcm958712_uImage_$$rel_tag ;\
        fi


# Creates source tar for rsdk
package-rsdk_linux:
	$(Q)cp -f os/osal/openrtos/sal_sscanf.c hybrid_m7/src/sal/
	$(Q)cp -f os/osal/openrtos/sal_alloc.c hybrid_m7/src/sal/
	$(Q)cp -f os/osal/openrtos/sal_util.c hybrid_m7/src/sal/
	$(Q)echo [Generating distribution package for $(subst package-,,$@)]
	$(Q)-rm -rf .pkg.*
	$(Q)$(TOPDIR)/tools/package/gen_package.pl $(subst package-,,$@)



package-umplus: package-robo_os_bcm953158k

export ROBO_REL_TYPE ?= BU
package-robo_os_bcm953158k:
	$(Q)echo [Generating distribution package for $(subst package-,,$@)]
	$(Q)-rm -rf .pkg.*
	$(Q)make bootloader
	$(Q)$(TOPDIR)/tools/package/gen_package.pl $(subst package-,,$@)
	$(Q)make -C $(TOPDIR)/tools/flash/ PRI_IMAGE=build/robo_os_bcm953158k/robo_os_bcm953158k.img flashable > /dev/null
	$(Q)make -C $(TOPDIR)/tools/flash/ PRI_IMAGE=build/robo_os_bcm953158k/robo_os_bcm953158k.img > /dev/null
	$(Q)if [ "$(ROBO_REL_VERSION)" != "" ] ;\
        then \
            rel_tag=`echo "$(ROBO_REL_VERSION)" | tr . _` ;\
            cp -f $(TOPDIR)/flashable.bin robo2_robo-os_$$rel_tag.img ;\
            cp -f $(TOPDIR)/upgradeable.bin robo2_robo-os_$${rel_tag}_upgradeable.img ;\
            mv robo_os_bcm953158k_src.tgz robo2_robo-os_src_$$rel_tag.tar.gz ;\
            tar -czf $(ROBO_REL_TYPE)_$(ROBO_REL_VERSION)_Robo2_Robo-OS-src.tar.gz robo2_robo-os_src_$$rel_tag.tar.gz robo2_robo-os_$$rel_tag.img robo2_robo-os_$${rel_tag}_upgradeable.img;\
            rm -f robo2_robo-os_src_$$rel_tag.tar.gz robo2_robo-os_$$rel_tag.img robo2_robo-os_$${rel_tag}_upgradeable.img;\
        fi

# make clean needs a target or all
ifeq ($(MAKECMDGOALS),clean)
$(info Please specify a target to be cleaned ... like below)
$(info - make clean all)
$(info - make clean rsdk_linux)
endif
