##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = mc68230.h mc68681.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/motorola \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/motorola/%)

$(PROJECT_INCLUDE)/motorola:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/motorola/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(PREINSTALL_FILES)
debug-am: $(PREINSTALL_FILES)
profile-am: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
