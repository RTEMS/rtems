##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h erc32.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = bsp.h coverhd.h

include $(top_srcdir)/../../../../../../automake/local.am
