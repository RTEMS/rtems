##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = erc32.h

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = erc32.h

include $(top_srcdir)/../../../../../automake/local.am
