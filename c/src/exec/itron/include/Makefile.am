##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = itron.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE) \
    $(noinst_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(PREINSTALL_FILES)

SUBDIRS = rtems itronsys

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
