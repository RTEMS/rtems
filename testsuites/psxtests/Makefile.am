##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

project_bspdir = $(PROJECT_ROOT)/@RTEMS_BSP@

PREINSTALLDIRS = $(project_bspdir)/tests/screens/psxtests
$(PREINSTALLDIRS):
	@$(mkinstalldirs) $@

all-local: $(PREINSTALLDIRS)

POSIX_DIRS = psxhdrs psx01 psx02 psx03 psx04 psx05 psx06 psx07 psx08 psx09 \
    psx10 psx11 psx12 psxtime psxtimer psxcancel

POSIX_FILES_DIRS = psxfile01 psxreaddir psxstat psxmount psx13

SUBDIRS = . support $(POSIX_DIRS) $(POSIX_FILES_DIRS)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
