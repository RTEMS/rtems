##
## $Id$
##

PGM = $(ARCH)/console.rel

C_FILES = console-io.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

shared_C_FILES = ../../../shared/console-polled.c
OBJS += $(shared_C_FILES:../../../shared/%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = consupp.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): ../../../shared/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PGM)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../automake/local.am
