##
## $Id$
##


include_mcf5206dir = $(includedir)/mcf5206

include_mcf5206_HEADERS = mcf5206e.h mcfmbus.h mcfuart.h

$(PROJECT_INCLUDE)/mcf5206:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/mcf5206/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE)/mcf5206 \
    $(include_mcf5206_HEADERS:%=$(PROJECT_INCLUDE)/mcf5206/%)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(include_mcf5206_HEADERS)

include $(top_srcdir)/../../../../automake/local.am
