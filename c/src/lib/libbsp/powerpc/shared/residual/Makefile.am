##
## $Id$
##

C_FILES = residual.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = pnp.h residual.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = residual.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/pnp.h: pnp.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pnp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pnp.h

$(PROJECT_INCLUDE)/bsp/residual.h: residual.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/residual.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/residual.h

include $(top_srcdir)/../../../automake/local.am
