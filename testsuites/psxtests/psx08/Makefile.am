##
## $Id$
##


MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c task1.c task2.c task3.c system.h ../include/pmacros.h

DOCS = psx08.scn

OBJS = $(lib_a_OBJECTS)


PGM = ${ARCH}/psx08.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/psx08.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/screens/psxtests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/psxtests
	@: > $(project_bspdir)/tests/screens/psxtests/$(dirstamp)

$(project_bspdir)/tests/screens/psxtests/psx08.scn: psx08.scn $(project_bspdir)/tests/screens/psxtests/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/psx08.exe
TMPINSTALL_FILES += $(project_bspdir)/tests/screens/psxtests/psx08.scn

$(PGM): ${ARCH}/$(dirstamp)


AM_CPPFLAGS += -I$(top_srcdir)/include

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
