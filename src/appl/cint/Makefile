# $Id: Makefile,v 1.30 2013/05/10 20:55:15 dkelley Exp $
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# All Rights Reserved.$
#
# Makefile for CINT library
#

#ifneq (,$(findstring cint,$(MAKECMDGOALS)))
LBUILD=1
#endif

#ifneq (,$(findstring utest,$(MAKECMDGOALS)))
#LBUILD=1
#endif

Q ?= @

ifndef LBUILD

# Build as part of the SDK
LOCALDIR = src/appl/cint
#include ${TOPDIR}/make/Make.config

lib = libcint

#include ${TOPDIR}/make/Make.lib
#include ${TOPDIR}/make/Make.depend

else

# Build standalone interpreter application

PERL := perl
#FLEX := /projects/ntsw-tools/bin/flex-2.5.31
#YACC := /tools/bin/yacc
#PATCH := patch
#REPLACESTRING := /projects/ntsw-tools/bin/replacestring
YEAR := $(shell date +%Y)
TMP := tmp.h

OUTPUTDIR ?= $(TOPDIR)/build/$(OS)/
BLDOUT := $(subst $(TOPDIR),$(OUTPUTDIR),$(shell pwd))
SRCDIR := .

SRCS := cint_ast.c
SRCS += cint_ast_debug.c
SRCS += cint_builtins.c
SRCS += cint_datatypes.c
SRCS += cint_debug.c
SRCS += cint_error.c
SRCS += cint_eval_ast_cint.c
SRCS += cint_eval_ast_print.c
SRCS += cint_eval_asts.c
SRCS += cint_internal.c
SRCS += cint_interpreter.c
SRCS += cint_operators.c
SRCS += cint_parser.c
SRCS += cint_test_data.c
SRCS += cint_variables.c
SRCS += main.c
SRCS += cint_stubs.c
# generated
SRCS += cint_c.lex.c
SRCS += cint_c.tab.c


CFLAGS += -g -O0
ifeq ($(OS),openrtos)
CFLAGS += -DOPENRTOS_CINT=1
CFLAGS += -I$(TOPDIR)/src/appl/
CFLAGS += -I/projects/ccxsw_tools/linaro_arm_none/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include/
endif


# portability interfaces - pick one
CFLAGS += -DCINT_CONFIG_INCLUDE_STDLIB=0
CFLAGS += -DCINT_CONFIG_INCLUDE_SDK_SAL=1
CFLAGS += -DCINT_CONFIG_INCLUDE_STUBS=0

# test features
CFLAGS += -DCINT_CONFIG_INCLUDE_LOAD=1
CFLAGS += -DCINT_CONFIG_INCLUDE_MAIN=1
CFLAGS += -DCINT_CONFIG_INCLUDE_TEST_DATA=1

# FILE IO and READLINE
CFLAGS += -DCINT_CONFIG_INCLUDE_XINCLUDE=0
CFLAGS += -DCINT_CONFIG_FILE_IO=0
ifeq ($(OS),openrtos)
CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER_READLINE=0
CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY=0
endif
# other configs
#CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_DOUBLES=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_LONGLONGS=0
ifeq ($(OS),openrtos)
CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
endif
ifeq ($(CPU),a57)
CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
endif
ifeq ($(CPU),NS2)
CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
endif
ifeq ($(CPU),NSP)
CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
endif
#CFLAGS += -DCINT_CONFIG_INCLUDE_DTRACE=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_DEBUG=0
CFLAGS += -DCINT_CONFIG_INCLUDE_POSIX_TIMER=0
#CFLAGS += -DCINT_CONFIG_YAPI=1

# unused configs
#CFLAGS += -DCINT_CONFIG_PARSER_STDIN=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_SOURCE=1

CFLAGS += -DYY_BUF_SIZE=512
CFLAGS += -Wall
CFLAGS += -Werror -fgnu89-inline

OBJS := $(addprefix $(BLDOUT)/,$(SRCS:%.c=%.o))

DEPS := $(addprefix $(BLDOUT)/,$(SRCS:%.c=%.d))

#.PHONY: gen
#all: gen

all: $(LIBDIR)/libcint.a

$(LIBDIR)/libcint.a: $(OBJS)
	$(Q)echo "[Archive ... $(notdir $@)]"
	$(Q)$(AR) rs $@ $^

#gen: cint_c.lex.c cint_c.tab.c cint_wrappers.h

cint: $(OBJS)
	$(Q)$(CC) -o $@ $+ -ldl -lrt -lreadline -lcurses

#COPYRIGHT_TAG := Copyright
#cint_c.tab.c: cint_grammar.y
#	$(Q)$(YACC) -dv -o $@ $<
## Avoid compiler warnings
#	$(Q)$(REPLACESTRING) -a "YYSTYPE yylval;" "YYSTYPE yylval = 0;" $@
#	$(Q)$(REPLACESTRING) -a "YYLTYPE yylloc;" "YYLTYPE yylloc = {0,0,0,0};" $@
## use CINT bindings via BISON interfaces
#	$(Q)$(REPLACESTRING) -a "yyps = (yypstate *) malloc (sizeof *yyps)" \
#		"yyps = (yypstate *) YYMALLOC (sizeof *yyps)" $@
#	$(Q)$(REPLACESTRING) -a "free (yyps)" "YYFREE (yyps)" $@
## use standard BRCM id/copyright
#	$(Q)echo '/* ' > $(TMP)
#	$(Q)echo -n '   $$' >> $(TMP)
#	$(Q)echo -n 'Id: ' >> $(TMP)
#	$(Q)echo '$$' >> $(TMP)
#	$(Q)echo '   $$$(COPYRIGHT_TAG): $(YEAR) Broadcom Inc.' >> $(TMP)
#	$(Q)echo '   All Rights Reserved.$$' >> $(TMP)
#	$(Q)echo '*/' >> $(TMP)
#	$(Q)cat cint_c.tab.h >> $(TMP)
#	$(Q)rm cint_c.tab.h
#	$(Q)mv tmp.h cint_c.tab.h
#
#cint_c.lex.c: cint_scanner.l lex.patch
#	$(Q)$(FLEX) --nounistd -o $@ $<
#	$(Q)$(PATCH) -s < lex.patch
#
#cint_wrappers.h: wrappergen.pl
#	$(Q)$(PERL) $< > $@

utest: cint utest-a.h utest-b.h
	$(Q)$(RM) *.code *.actual *.expected
	$(Q)$(PERL) utest.pl utests

utest-a.h:
	$(Q)echo "print 1;" > $@

utest-b.h:
	$(Q)echo "#include <utest-a.h>" > $@
	$(Q)echo "print 121;" >> $@
	$(Q)echo "#include <utest-a.h>" >> $@


clean:
#	$(Q)-$(RM) cint_wrappers.h cint_c.*.[ch] cint_c.output cint_c.lex.c.orig
	$(Q)-$(RM) cint $(OBJS) $(DEPS) utest-*.h libcint.a

ifneq ($(MAKECMDGOALS),clean)
    -include $(DEPS)
endif

# calculate C include dependencies
$(BLDOUT)/%.o: %.c
	$(Q)echo [Compile ... $<]
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) -MMD -c $(CFLAGS) $< -o $@

endif



