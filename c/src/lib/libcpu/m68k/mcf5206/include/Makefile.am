##
## $Id$
##

include_mcf5206dir = $(includedir)/mcf5206

include_mcf5206_HEADERS = mcf5206e.h mcfmbus.h mcfuart.h

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(include_mcf5206_HEADERS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/mcf5206/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/mcf5206
	@: > $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)

$(PROJECT_INCLUDE)/mcf5206/mcf5206e.h: mcf5206e.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcf5206e.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcf5206e.h

$(PROJECT_INCLUDE)/mcf5206/mcfmbus.h: mcfmbus.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcfmbus.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcfmbus.h

$(PROJECT_INCLUDE)/mcf5206/mcfuart.h: mcfuart.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcfuart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcfuart.h

include $(top_srcdir)/../../../../automake/local.am
