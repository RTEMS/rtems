##
## Makefile.am,v 1.3 2002/03/28 00:48:01 joel Exp
##


include_mcf5282dir = $(includedir)/mcf5282

include_mcf5282_HEADERS = mcf5282.h 

$(PROJECT_INCLUDE)/mcf5282:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/mcf5282/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE)/mcf5282 \
    $(include_mcf5282_HEADERS:%=$(PROJECT_INCLUDE)/mcf5282/%)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(include_mcf5282_HEADERS)

include $(top_srcdir)/../../../../../../automake/local.am
