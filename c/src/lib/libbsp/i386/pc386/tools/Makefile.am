##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

EXTRA_DIST = Spec.doc

noinst_PROGRAMS = bin2boot

bin2boot_SOURCES = bin2boot.c

@PROJECT_RELEASE@/build-tools:
	$(mkinstalldirs) $@

@PROJECT_RELEASE@/build-tools/bin2boot$(EXEEXT): bin2boot$(EXEEXT)
	$(INSTALL_PROGRAM) $< $@

TMPINSTALL_FILES = @PROJECT_RELEASE@/build-tools \
    @PROJECT_RELEASE@/build-tools/bin2boot$(EXEEXT)

## HACK: install into build-tree
all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/host.am
