#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

EXTRA_DIST = Spec.doc

noinst_PROGRAMS = \
bin2boot

bin2boot_SOURCES = bin2boot.c

PREINSTALL_DIRS=$(PROJECT_ROOT)/pc386/build-tools
PREINSTALL_FILES = $(PROJECT_ROOT)/pc386/build-tools/bin2boot

$(PREINSTALL_DIRS):
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/pc386/build-tools/bin2boot: bin2boot
	$(INSTALL_PROGRAM) $< $@

# HACK: install into build-tree
all-local: $(PREINSTALL_DIRS) $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/local.am
