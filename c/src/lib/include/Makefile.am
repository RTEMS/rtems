##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_PIECES = chain.h console.h clockdrv.h iosupp.h ringbuf.h spurious.h \
    timerdrv.h vmeintr.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE) \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

SUBDIRS = sys motorola zilog rdbg

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
