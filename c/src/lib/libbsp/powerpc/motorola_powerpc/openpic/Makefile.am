##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

include_bspdir = $(includedir)/bsp
nodist_include_bsp_HEADERS = \
    $(top_srcdir)/../../powerpc/shared/openpic/openpic.h

powerpc_shared_openpic_C_FILES = \
    $(top_srcdir)/../../powerpc/shared/openpic/openpic.c
OBJS = \
    $(powerpc_shared_openpic_C_FILES:$(top_srcdir)/../../powerpc/shared/openpic/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/openpic/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/openpic.h: $(top_srcdir)/../../powerpc/shared/openpic/openpic.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/openpic.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/openpic.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../../automake/local.am
