##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/openpic

C_FILES = openpic.c

include_bspdir = $(includedir)/bsp
nodist_include_bsp_HEADERS = $(top_srcdir)/../shared/openpic/openpic.h

OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp

$(PROJECT_INCLUDE)/bsp/openpic.h: $(top_srcdir)/../shared/openpic/openpic.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/openpic.h

include $(top_srcdir)/../../../../automake/local.am
