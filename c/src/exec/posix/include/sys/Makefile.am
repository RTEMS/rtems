##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = utsname.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys \
    $(noinst_HEADERS:%=$(PROJECT_INCLUDE)/sys/%)

$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
