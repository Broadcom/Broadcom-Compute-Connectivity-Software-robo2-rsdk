#
# $ID:$
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
#
# PHY top-level makefile
#

# Set default PHY directory
PHY ?= $(shell pwd)

# Default include path
PHY_INCLUDE_PATH = -I$(PHY)/include -I$(TOPDIR)/include/osal

# Import preprocessor flags
PHY_CFLAGS += $(PHY_INCLUDE_PATH)
PHY_CFLAGS += -DPHY_SYS_USLEEP=sal_usleep

# Enable local configuration of PHYMOD library
PHY_CFLAGS += -DPHYMOD_INCLUDE_CUSTOM_CONFIG

# Phy package options
PHY_PKG_OPTIONS ?= $(PKG_OPTIONS)
ifdef CONFIG_EXT_PHY
PHY_PKG_OPTIONS += '-cbcm_gphy,bcmi_viper_xgxs,bcmi_tsce_xgxs,bcm_542xx'
else
PHY_PKG_OPTIONS += '-cbcm_gphy,bcmi_viper_xgxs,bcmi_tsce_xgxs'
endif

# Option to retrieve compiler version
CDK_CC_VERFLAGS ?= -v
CC_VERFLAGS = $(CDK_CC_VERFLAGS); 

# CDK include path
CDK_CFLAGS += -I$(PHY)/cdk/include

# Convenience Makefile flags for building specific chips
ifdef CDK_CHIPS
    CDK_CFLAGS += -DCDK_CONFIG_INCLUDE_CHIP_DEFAULT=0 
    CDK_CFLAGS += $(foreach chip,$(CDK_CHIPS),-DCDK_CONFIG_INCLUDE_${chip}=1) 
endif
ifdef CDK_NCHIPS
    CDK_CFLAGS += $(foreach chip,$(CDK_NCHIPS),-DCDK_CONFIG_INCLUDE_${chip}=0)
endif

ifdef DSYMS
    CDK_CFLAGS += -DCDK_CONFIG_CHIP_SYMBOLS_USE_DSYMS=1
endif

PHY_CFLAGS += $(CDK_CFLAGS)

# CDK build was not setting -mthumb. Leaving it as is for now.
# This can be removed once the phy driver has been tested and
# found to be working with -mthumb compile flag
#CFLAGS := $(subst -mthumb,,$(PHY_CFLAGS))
CFLAGS += $(PHY_CFLAGS)

# Default phy targets
PHY_DEFAULT_TARGETS = pkgsrc generic util

# Init phy targets to default if not specified
PHY_TARGETS ?= $(PHY_DEFAULT_TARGETS)
    
# Phy libraries
PHY_LIBNAMES = $(addprefix libphy,$(PHY_TARGETS))
PHY_LIBS = $(addsuffix .$(PHY_LIBSUFFIX),$(PHY_LIBNAMES))

.PHONY: all instpkgs clean cleanpkgs $(PHY_TARGETS)

all: instpkgs phylibs
#	$(MAKE) instpkgs
#	$(MAKE) phylibs

phylibs: $(PHY_TARGETS)

$(PHY_TARGETS): instpkgs
	$(Q)echo Processing PHY subdirectory $@
	$(Q)$(MAKE) -C $@ $(PHY_CLEAN)

relpkgs:
	$(Q)perl $(PHY)/tools/instpkgs.pl -d $(PHY) -r

instpkgs:
	$(Q)perl $(PHY)/tools/instpkgs.pl -d $(PHY) $(PHY_PKG_OPTIONS)

clean: cleanpkgs
	$(Q)echo [Clean ... $(PHY_TARGETS)]
	$(Q)for target in $(PHY_TARGETS) ; do $(MAKE) -C $$target clean ; done

cleanpkgs:
	$(Q)$(RM) $(PHY)/pkgsrc/installed-chips
	$(Q)$(RM) $(PHY)/include/phy_config_chips.h
	$(Q)$(RM) -r $(PHY)/include/phymod/chip
	$(Q)$(RM) -r $(PHY)/include/phy/chip
	$(Q)$(RM) -r $(PHY)/include/phy/bus
	$(Q)$(RM) $(PHY)/include/phy/phy_buslist.h
	$(Q)$(RM) $(PHY)/include/phy/phy_devlist.h
	$(Q)$(RM) -r $(PHY)/pkgsrc/chip
	$(Q)$(RM) -r $(PHY)/pkgsrc/bus
	$(Q)$(RM) -r $(PHY)/sym/chip
	$(Q)$(RM) $(PHY)/sym/phy_allsyms.c
