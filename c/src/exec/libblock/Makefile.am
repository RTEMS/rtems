##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = src

include_rtemsdir = $(includedir)/rtems

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

include_rtems_HEADERS = \
    include/rtems/bdbuf.h  include/rtems/blkdev.h \
    include/rtems/diskdevs.h  include/rtems/ramdisk.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)


EXTRA_DIST = README

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
