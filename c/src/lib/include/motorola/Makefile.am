##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_motoroladir = $(includedir)/motorola

include_motorola_HEADERS = mc68230.h mc68681.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola \
    $(include_motorola_HEADERS:%=$(PROJECT_INCLUDE)/motorola/%)

$(PROJECT_INCLUDE)/motorola:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/motorola/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
