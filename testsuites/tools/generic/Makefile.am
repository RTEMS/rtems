##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../aclocal

noinst_SCRIPTS = difftest sorttimes

project_bspdir = $(PROJECT_ROOT)/@RTEMS_BSP@

PREINSTALL_FILES = $(project_bspdir)/tests $(project_bspdir)/tests/difftest \
    $(project_bspdir)/tests/sorttimes

$(project_bspdir)/tests:
	@$(mkinstalldirs) $@

$(project_bspdir)/tests/difftest: difftest
	$(INSTALL_SCRIPT) $< $@

$(project_bspdir)/tests/sorttimes: sorttimes
	$(INSTALL_SCRIPT) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
