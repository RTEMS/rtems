##
## $Id$
##

C_FILES = conscfg.c debugio.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

shared_C_FILES = $(top_srcdir)/../../shared/console.c
OBJS += $(shared_C_FILES:$(top_srcdir)/../../shared/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../shared/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../automake/local.am
