#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

project_bspdir=$(PROJECT_ROOT)/@RTEMS_BSP@

PREINSTALLDIRS = $(project_bspdir)/tests/screens/sptests
$(PREINSTALLDIRS):
	@$(mkinstalldirs) $@

all-local: $(PREINSTALLDIRS)

# sp10 and spfatal are not included for now
SUBDIRS = . sp01 sp02 sp03 sp04 sp05 sp06 sp07 sp08 sp09 sp11 sp12 sp13 sp14 \
    sp15 sp16 sp17 sp19 sp20 sp21 sp22 sp23 sp24 sp25 spsize

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
