##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = z8036.h z8530.h z8536.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/zilog \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/zilog/%)

$(PROJECT_INCLUDE)/zilog:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/zilog/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(PREINSTALL_FILES)
debug-am: $(PREINSTALL_FILES)
profile-am: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
