##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
EXTRA_PROGRAMS =
noinst_DATA =

if mcf5282

# include
include_mcf5282dir = $(includedir)/mcf5282
include_mcf5282_HEADERS = include/mcf5282.h
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if mcf5282
$(PROJECT_INCLUDE)/mcf5282/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/mcf5282
	@: > $(PROJECT_INCLUDE)/mcf5282/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/mcf5282/$(dirstamp)

$(PROJECT_INCLUDE)/mcf5282/mcf5282.h: include/mcf5282.h $(PROJECT_INCLUDE)/mcf5282/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5282/mcf5282.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5282/mcf5282.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
