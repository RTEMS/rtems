##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

project_bspdir=$(PROJECT_ROOT)/@RTEMS_BSP@

noinst_HEADERS = tmacros.h

TMPINSTALLFILES = $(project_bspdir)/lib/include \
   $(project_bspdir)/lib/include/tmacros.h

$(project_bspdir)/lib/include:
	$(mkinstalldirs) $@
$(project_bspdir)/lib/include/tmacros.h: tmacros.h
	$(INSTALL_DATA) $< $@

CLEANFILES = $(project_bspdir)/lib/include/tmacros.h

all: $(TMPINSTALLFILES)

include $(top_srcdir)/../../../../automake/local.am
