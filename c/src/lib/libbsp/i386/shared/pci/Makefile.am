## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = pcibios.h
noinst_HEADERS = $(H_FILES)

C_FILES = pcibios.c

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../../automake/local.am
