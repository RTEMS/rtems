##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = chain.h console.h clockdrv.h iosupp.h ringbuf.h spurious.h \
    timerdrv.h vmeintr.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE) \
    $(noinst_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

SUBDIRS = sys motorola zilog

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
