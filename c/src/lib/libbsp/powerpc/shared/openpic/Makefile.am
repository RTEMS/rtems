##
## $Id$
##

C_FILES = openpic.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = openpic.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = openpic.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/openpic.h: openpic.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/openpic.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/openpic.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
