##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = mongoose-v.h

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/libcpu/%.h)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = mongoose-v.h

include $(top_srcdir)/../../../../../automake/local.am
