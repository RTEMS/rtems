#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

H_PIECES = mc68230.h mc68681.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/motorola \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/motorola/%)

$(PROJECT_INCLUDE)/motorola:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/motorola/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
