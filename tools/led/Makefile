# -*- Makefile -*-
# $Id: $
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved$
#
# Makefile for building BSL header files
#

LEDASM = ./ledasm
ASMFILE = led

MKLED = ./mkled.pl
GDIFF = diff -q -I Copyright -I Id 

ifeq ($(SDK),)
SDK = ../..
endif

ALL_TARGETS = led.h

LED_DESTDIR = ../../include/soc/robo2/bcm53158

TMPFILE = ./led.tmp

all: $(ALL_TARGETS)

led.h:
	$(LEDASM)  $(ASMFILE)
	$(MKLED)
	if ! $(GDIFF) $(TMPFILE) $(LED_DESTDIR)/$@; \
		then cp $(TMPFILE) $(LED_DESTDIR)/$@; fi
	rm -f $(TMPFILE) $(ASMFILE).c $(ASMFILE).h $(ASMFILE).hex $(ASMFILE).lst

.PHONY: $(ALL_TARGETS)
