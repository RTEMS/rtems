##
## $Id$
##

C_FILES = motorola.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = motorola.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = motorola.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/motorola.h: motorola.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/motorola.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/motorola.h

include $(top_srcdir)/../../../automake/local.am
