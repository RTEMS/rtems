##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5

include_rtemsdir = $(includedir)/rtems

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

include_rtems_HEADERS = \
    rtems/bdbuf.h  rtems/blkdev.h  rtems/diskdevs.h  rtems/ramdisk.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
