##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

project_bspdir=$(PROJECT_ROOT)/@RTEMS_BSP@

PREINSTALLDIRS = \
    $(project_bspdir)/tests/screens/mptests \
    $(project_bspdir)/tests/screens/mptests/node1 \
    $(project_bspdir)/tests/screens/mptests/node2
$(PREINSTALLDIRS):
	@$(mkinstalldirs) $@

all-local: $(PREINSTALLDIRS)

SUBDIRS = . mp01 mp02 mp03 mp04 mp05 mp06 mp07 mp08 mp09 mp10 mp11 mp12 mp13 \
    mp14

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
