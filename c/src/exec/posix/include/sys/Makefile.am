##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_PIECES = utsname.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/sys \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/sys/%)

$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
