##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = pcibios.h

C_FILES = pcibios.c

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = pcibios.c pcibios.h

include $(top_srcdir)/../../../../../automake/local.am
