##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_PIECES = z8036.h z8530.h z8536.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/zilog \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/zilog/%)

$(PROJECT_INCLUDE)/zilog:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/zilog/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
