##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

powerpc_shared_console_C_FILES = \
    $(top_srcdir)/../../powerpc/shared/console/polled_io.c \
    $(top_srcdir)/../../powerpc/shared/console/uart.c \
    $(top_srcdir)/../../powerpc/shared/console/console.c \
    $(top_srcdir)/../../powerpc/shared/console/inch.c \
    $(top_srcdir)/../../powerpc/shared/console/reboot.c
OBJS = \
    $(powerpc_shared_console_C_FILES:$(top_srcdir)/../../powerpc/shared/console/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/console/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

# FIXME: This is nonsense.
noinst_HEADERS = $(top_srcdir)/../../powerpc/shared/console/consoleIo.h \
    $(top_srcdir)/../../powerpc/shared/console/keyboard.h \
    $(top_srcdir)/../../powerpc/shared/console/uart.h

noinst_DATA = $(OBJS)

AM_CPPFLAGS += -DSTATIC_LOG_ALLOC

all-local: $(OBJS)

include $(top_srcdir)/../../../../automake/local.am
