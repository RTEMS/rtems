##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

project_bspdir = $(PROJECT_ROOT)/@RTEMS_BSP@

PREINSTALLDIRS = $(project_bspdir)/tests/screens/itrontests
$(PREINSTALLDIRS):
	@$(mkinstalldirs) $@

all-local: $(PREINSTALLDIRS)

SUBDIRS = . itronhello itrontask01 itrontask02 itrontask03 itrontask04 \
    itronmbf01 itronsem01 itrontime01


include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
