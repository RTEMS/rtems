##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/console

C_FILES = uart.c console.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

H_FILES = ../../shared/console/uart.h

OBJS = $(C_O_FILES)

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = ../../shared/console/uart.h

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/uart.h:../../shared/console/uart.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/uart.h

AM_CPPFLAGS += -DSTATIC_LOG_ALLOC

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
