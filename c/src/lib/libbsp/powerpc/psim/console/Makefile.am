##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

C_FILES = console-io.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

shared_C_FILES = $(top_srcdir)/../../shared/console-polled.c
OBJS += \
    $(shared_C_FILES:$(top_srcdir)/../../shared/%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = consupp.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../shared/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../automake/local.am
