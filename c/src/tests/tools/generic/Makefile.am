#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

noinst_SCRIPTS = difftest sorttimes

project_bspdir = $(PROJECT_ROOT)/@RTEMS_BSP@

PREINSTALL_DIRS = $(project_bspdir)/tests
PREINSTALL_FILES = \
   $(project_bspdir)/tests/difftest \
   $(project_bspdir)/tests/sorttimes

$(PREINSTALL_DIRS):
	@$(mkinstalldirs) $@

$(project_bspdir)/tests/difftest: difftest
	$(INSTALL_SCRIPT) $< $@

$(project_bspdir)/tests/sorttimes: sorttimes
	$(INSTALL_SCRIPT) $< $@

# HACK: install to build-tree
all-local: $(PREINSTALL_DIRS) $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
