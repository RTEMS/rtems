##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

noinst_PROGRAMS = print_dump

print_dump_SOURCES = print_dump.c

## HACK: install to build-tree
install-exec-local: $(PROGRAMS)
	$(mkinstalldirs) $(PROJECT_ROOT)/simhppa/bin
	$(INSTALL_PROGRAM) print_dump $(PROJECT_ROOT)/simhppa/bin

include $(top_srcdir)/../../../../../../../automake/local.am
