#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

project_bspdir=$(PROJECT_ROOT)/@RTEMS_BSP@

noinst_HEADERS = tmacros.h

TMPINSTALLDIRS=$(project_bspdir)/lib/include

$(TMPINSTALLDIRS):
	$(mkinstalldirs) $@

TMPINSTALLFILES = $(project_bspdir)/lib/include/tmacros.h
	
$(project_bspdir)/lib/include/tmacros.h: tmacros.h
	$(INSTALL_DATA) $< $@

CLEANFILES = $(TMPINSTALLFILES)

install-tmp: $(TMPINSTALLFILES)

all: install-tmp
debug: install-tmp
profile: install-tmp

include $(top_srcdir)/../../../../automake/local.am
