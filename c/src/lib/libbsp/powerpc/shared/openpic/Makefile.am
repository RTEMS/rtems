## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = openpic.c

H_FILES = openpic.h
noinst_HEADERS = $(H_FILES)

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/bsp/%.h)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../../automake/local.am
