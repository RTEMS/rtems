##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

powerpc_shared_clock_C_FILES = \
    $(top_srcdir)/../../powerpc/shared/clock/p_clock.c
OBJS = \
    $(powerpc_shared_clock_C_FILES:$(top_srcdir)/../../powerpc/shared/clock/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/clock/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

include $(top_srcdir)/../../../../automake/local.am
