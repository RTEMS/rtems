##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = vectors_init.c

H_FILES = vectors.h

S_FILES = vectors.S

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/bsp/%.h)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = vectors.S vectors.h vectors_init.c

include $(top_srcdir)/../../../../../automake/local.am
