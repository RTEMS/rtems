##
## $Id$
##

C_FILES = vectors_init.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = vectors.h

S_FILES = vectors.S

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = vectors.S vectors_init.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/vectors.h: vectors.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

include $(top_srcdir)/../../../automake/local.am
